/*
 * @file    src/vimbackend.cpp
 * @brief   Vim editor backend using QTermWidget.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <qtermwidget.h>

#include <QApplication>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QStringList>
#include <QTimer>
#include <QUuid>

#include "vimbackend.h"

#include <QDebug>



VimBackend::VimBackend(
	QWidget *parent
)
	: EditorBackend(parent),
	  m_stack(new QStackedWidget(parent)),
	  m_welcomeWidget(nullptr),
	  m_terminal(nullptr),
	  m_stateFilePath(),
	  m_stateWatcher(
		  new QFileSystemWatcher(this)
	  ),
	  m_modified(false),
	  m_fontPointSize(
		  QApplication::font().pointSize()
	  ),
	  m_lastVimEvent(),
	  m_saveLoop(nullptr),
	  m_savePending(false),
	  m_saveSucceeded(false),
	  m_editLoop(nullptr),
	  m_editPending(false),
	  m_editSucceeded(false),
	  m_discardLoop(nullptr),
	  m_discardPending(false),
	  m_discardSucceeded(false),
	  m_closeLoop(nullptr),
	  m_closePending(false),
	  m_closeSucceeded(false),
	  m_pendingFilePath()
{
	// Welcome panel
	m_welcomeWidget =
		new QLabel(
			QStringLiteral(
				"No file is open.\n\n"
				"Double-click a source, header, "
				"or other file in the "
				"project tree to edit."
			),
			m_stack
		);

	m_welcomeWidget->setAlignment(
		Qt::AlignCenter
	);

	m_welcomeWidget->setMinimumHeight(
		160
	);

	m_stack->addWidget(
		m_welcomeWidget
	);

	m_stack->setCurrentWidget(
		m_welcomeWidget
	);

	// Ensure default font size as minimum
	if (m_fontPointSize <= 0) {
		m_fontPointSize = 10;
	}

	// set up statFile path
	const QString temporaryDirectory =
		QStandardPaths::writableLocation(
			QStandardPaths::TempLocation
		);

	m_stateFilePath =
		QDir(temporaryDirectory).filePath(
			QStringLiteral(
				"tigcc-qt-vim-%1.state"
			).arg(
				QUuid::createUuid().toString(
					QUuid::WithoutBraces
				)
			)
		);

	// initial stateFile
	QFile stateFile(
		m_stateFilePath
	);

	if (stateFile.open(
			QIODevice::WriteOnly |
			QIODevice::Truncate
		)) {
		stateFile.write(
			"\n0\nstate\n"
		);

		stateFile.close();
	}

	m_stateWatcher->addPath(
		m_stateFilePath
	);

	// and connect
	connect(
		m_stateWatcher,
		&QFileSystemWatcher::fileChanged,
		this,
		[this](const QString &path) {
			if (path == m_stateFilePath) {
				readVimState();

				if (!m_stateWatcher->files().contains(
						m_stateFilePath
					)) {
					m_stateWatcher->addPath(
						m_stateFilePath
					);
				}
			}
		}
	);
} // End constructor


QWidget *
VimBackend::widget()
{
	return m_stack;
}


QString
VimBackend::vimStateCommand() const
{
	QString stateFilePath =
		m_stateFilePath;

	stateFilePath.replace(
		QStringLiteral("'"),
		QStringLiteral("''")
	);

	// Construct a Vimscript string
	return QStringLiteral(
		"let g:tigcc_qt_state_file = '%1' | "
		"augroup TigccQtState | "
		"autocmd! | "
		"autocmd BufEnter,BufFilePost,"
		"TextChanged,TextChangedI * "
		"call writefile([expand('%%:p'), "
		"&modified ? '1' : '0', 'state'], "
		"g:tigcc_qt_state_file) | "
		"autocmd BufReadPost * "
		"call writefile([expand('%%:p'), "
		"&modified ? '1' : '0', 'discard'], "
		"g:tigcc_qt_state_file) | "
		"autocmd BufWritePost * "
		"call writefile([expand('%%:p'), "
		"&modified ? '1' : '0', 'write'], "
		"g:tigcc_qt_state_file) | "
		"autocmd VimLeavePre * "
		"call writefile(['', '0', 'exit'], "
		"g:tigcc_qt_state_file) | "
		"augroup END"
	).arg(
		stateFilePath
	);
} // End vimStateCommand
 

void
VimBackend::readVimState()
{
	QFile stateFile(
		m_stateFilePath
	);

	if (!stateFile.open(
			QIODevice::ReadOnly
		)) {
		return;
	}

	const QStringList lines =
		QString::fromUtf8(
			stateFile.readAll()
		).split(
			QChar('\n')
		);

	stateFile.close();

	if (lines.size() < 3) {
		return;
	}

	const QString reportedFilePath =
		lines.at(0).trimmed();

	const bool reportedModified =
		lines.at(1).trimmed() ==
		QStringLiteral("1");

	const QString reportedEvent =
		lines.at(2).trimmed();

	const QString normalizedReportedPath =
		reportedFilePath.isEmpty()
			? QString()
			: QFileInfo(
				reportedFilePath
			).absoluteFilePath();

	if (m_editPending) {
		if (normalizedReportedPath !=
			m_pendingFilePath) {
			return;
		}

		m_filePath =
			normalizedReportedPath;

		m_modified =
			reportedModified;

		m_lastVimEvent =
			reportedEvent;

		m_editPending =
			false;

		m_editSucceeded =
			true;

		if (m_editLoop != nullptr) {
			m_editLoop->quit();
		}

		emitCurrentDocumentState();

		return;
	}

	if (!reportedFilePath.isEmpty()) {
		m_filePath =
			QFileInfo(
				reportedFilePath
			).absoluteFilePath();
	}

	m_modified =
		reportedModified;

	m_lastVimEvent =
		reportedEvent;

	// Acknowlege pending save
	if (m_lastVimEvent ==
			QStringLiteral("write") &&
		m_savePending) {
		m_savePending =
			false;

		m_saveSucceeded =
			true;

		if (m_saveLoop != nullptr) {
			m_saveLoop->quit();
		}
	}

	// Acknowledge pending discard
	if (m_discardPending &&
		m_lastVimEvent ==
			QStringLiteral("discard") &&
		!reportedModified) {
		m_modified =
			false;

		m_discardPending =
			false;

		m_discardSucceeded =
			true;

		if (m_discardLoop != nullptr) {
			m_discardLoop->quit();
		}
	}

	emitCurrentDocumentState();
} // End readVimState


void
VimBackend::sendVimCommand(
	const QString &command
)
{
	const QString vim_cmd =
		QString(QChar(0x1b)) +
		QStringLiteral(":") +
		command +
		QStringLiteral("\r");

	m_terminal->sendText(
		vim_cmd
	);
}


void
VimBackend::sendVimEditCommand(
	const QString &filePath
)
{
	if (m_terminal == nullptr) {
		return;
	}

	QString vimPath =
		filePath;

	/*
	 * Vim's single-quoted strings represent a literal
	 * quote by doubling it.
	 */
	vimPath.replace(
		QStringLiteral("'"),
		QStringLiteral("''")
	);

	const QString command =
		QStringLiteral(
			":execute 'edit ' . fnameescape('%1')\r"
		).arg(
			vimPath
		);

	m_terminal->sendText(
		QString(QChar(0x1b)) +
		command
	);
} // End sendVimEditCommand


void
VimBackend::handleTerminalFinished()
{
	m_terminal =
		nullptr;

	m_filePath.clear();

	m_stack->setCurrentWidget(
		m_welcomeWidget
	);

	if (m_closePending) {
		m_closePending =
			false;

		m_closeSucceeded =
			true;

		if (m_closeLoop != nullptr) {
			m_closeLoop->quit();
		}
	}

	emitCurrentDocumentState();
} // End handleTerminalFinished


bool
VimBackend::openFile(
	const QString &filePath,
	QString *errorMessage
)
{
	const QFileInfo fileInfo(
		filePath
	);

	if (!fileInfo.exists() || !fileInfo.isFile()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The file does not exist:\n%1"
				).arg(filePath);
		}

		return false;
	}

	if (m_terminal != nullptr) {
		const QString normalizedPath =
			fileInfo.absoluteFilePath();

		if (normalizedPath == m_filePath) {
			return true;
		}

		QEventLoop editLoop;

		QTimer timeoutTimer;

		timeoutTimer.setSingleShot(
			true
		);

		m_pendingFilePath =
			normalizedPath;

		m_editLoop =
			&editLoop;

		m_editPending =
			true;

		m_editSucceeded =
			false;

		connect(
			&timeoutTimer,
			&QTimer::timeout,
			&editLoop,
			&QEventLoop::quit
		);

		timeoutTimer.start(
			5000
		);

		sendVimEditCommand(
			normalizedPath
		);

		editLoop.exec();

		m_editLoop =
			nullptr;

		if (!m_editSucceeded) {
			m_editPending =
				false;

			m_pendingFilePath.clear();

			if (errorMessage != nullptr) {
				*errorMessage =
					QStringLiteral(
						"Vim did not confirm that "
						"the requested file was opened."
					);
			}

			return false;
		}

		m_pendingFilePath.clear();

		m_stack->setCurrentWidget(
			m_terminal
		);

		return true;
	}

	m_terminal =
		new QTermWidget(
			0,
			m_stack
		);

	// Apply font
	QFont terminalFont =
		m_terminal->getTerminalFont();

	terminalFont.setPointSize(
		m_fontPointSize
	);

	m_terminal->setTerminalFont(
		terminalFont
	);

	connect(
		m_terminal,
		&QTermWidget::finished,
		this,
		&VimBackend::handleTerminalFinished
	);

	m_terminal->setWorkingDirectory(
		fileInfo.absolutePath()
	);

	m_terminal->setShellProgram(
		QStringLiteral("vim")
	);

	// First launch
	QStringList arguments;

	arguments.append(
		QStringLiteral("--cmd")
	);

	arguments.append(
		vimStateCommand()
	);

	arguments.append(
		fileInfo.absoluteFilePath()
	);

	m_terminal->setArgs(
		arguments
	);

	m_terminal->setAutoClose(
		true
	);

	m_stack->addWidget(
		m_terminal
	);

	m_terminal->startShellProgram();

	m_filePath =
		fileInfo.absoluteFilePath();

	m_stack->setCurrentWidget(
		m_terminal
	);

	m_modified =
		false;

	emitCurrentDocumentState();

	return true;
} // End openFile


bool
VimBackend::saveCurrentFile(
	QString *errorMessage
)
{
	if (m_terminal == nullptr) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"No Vim session is currently active."
				);
		}

		return false;
	}

	QEventLoop saveLoop;

	m_saveLoop =
		&saveLoop;

	m_savePending =
		true;

	m_saveSucceeded =
		false;

	sendVimCommand(
		QStringLiteral("write")
	);

	QTimer timeoutTimer;

	timeoutTimer.setSingleShot(
		true
	);

	connect(
		&timeoutTimer,
		&QTimer::timeout,
		&saveLoop,
		&QEventLoop::quit
	);

	timeoutTimer.start(
		5000
	);

	saveLoop.exec();

	m_saveLoop =
		nullptr;

	if (!m_saveSucceeded) {
		m_savePending =
			false;

		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Vim did not confirm that "
					"the file was saved."
				);
		}

		return false;
	}

	return true;
} // End saveCurrentFile


bool
VimBackend::hasModifiedFiles() const
{
	qDebug()
		<< "VimBackend::hasModifiedFiles():"
		<< m_modified
		<< "file:"
		<< m_filePath
		<< "event:"
		<< m_lastVimEvent;

	return m_modified;
}


bool
VimBackend::saveAllFiles(
	QString *errorMessage
)
{
	if (m_terminal == nullptr ||
		m_filePath.isEmpty()) {
		return true;
	}

	if (!m_modified) {
		return true;
	}

	return saveCurrentFile(
		errorMessage
	);
} // End saveAllFiles


bool
VimBackend::discardAllChanges(
	QString *errorMessage
)
{
	if (m_terminal == nullptr ||
		m_filePath.isEmpty() ||
		!m_modified) {
		return true;
	}

	QEventLoop discardLoop;

	QTimer timeoutTimer;

	timeoutTimer.setSingleShot(
		true
	);

	m_discardLoop =
		&discardLoop;

	m_discardPending =
		true;

	m_discardSucceeded =
		false;

	connect(
		&timeoutTimer,
		&QTimer::timeout,
		&discardLoop,
		&QEventLoop::quit
	);

	timeoutTimer.start(
		5000
	);

	sendVimCommand(
		QStringLiteral("edit!")
	);

	discardLoop.exec();

	m_discardLoop =
		nullptr;

	if (!m_discardSucceeded) {
		m_discardPending =
			false;

		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Vim did not confirm that "
					"changes were discarded."
				);
		}

		return false;
	}

	emitCurrentDocumentState();

	return true;
} // End discardAllChanges


bool
VimBackend::closeCurrentFile(
	QString *errorMessage
)
{
	if (m_terminal == nullptr ||
		m_filePath.isEmpty()) {
		return true;
	}

	if (m_modified) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The current file has "
					"unsaved changes."
				);
		}

		return false;
	}

	QEventLoop closeLoop;

	QTimer timeoutTimer;

	timeoutTimer.setSingleShot(
		true
	);

	m_closeLoop =
		&closeLoop;

	m_closePending =
		true;

	m_closeSucceeded =
		false;

	connect(
		&timeoutTimer,
		&QTimer::timeout,
		&closeLoop,
		&QEventLoop::quit
	);

	timeoutTimer.start(
		5000
	);

	sendVimCommand(
		QStringLiteral("quit")
	);

	closeLoop.exec();

	m_closeLoop =
		nullptr;

	if (!m_closeSucceeded) {
		m_closePending =
			false;

		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Vim did not confirm that "
					"the session closed."
				);
		}

		return false;
	}

	return true;
} // End closeCurrentFile


bool
VimBackend::closeAllFiles(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	return true;
}


QString
VimBackend::currentFilePath() const
{
	return m_filePath;
}


bool
VimBackend::isModified() const
{
	return m_modified;
}


void
VimBackend::emitCurrentDocumentState()
{
	if (m_terminal == nullptr ||
		m_filePath.isEmpty()) {
		emit currentFileChanged(
			QString()
		);

		emit modificationChanged(
			false
		);

		return;
	}

	emit currentFileChanged(
		m_filePath
	);

	emit modificationChanged(
		isModified()
	);
} // End emitCurrentDocumentState


int
VimBackend::fontPointSize() const
{
	return m_fontPointSize;
}


bool
VimBackend::setFontPointSize(
	int pointSize,
	QString *errorMessage
)
{
	if (pointSize < 6 ||
		pointSize > 36) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Font size must be between "
					"6 and 36 points."
				);
		}

		return false;
	}

	if (pointSize == m_fontPointSize) {
		return true;
	}

	m_fontPointSize =
		pointSize;

	if (m_terminal != nullptr) {
		QFont terminalFont =
			m_terminal->getTerminalFont();

		terminalFont.setPointSize(
			m_fontPointSize
		);

		m_terminal->setTerminalFont(
			terminalFont
		);
	}

	emit fontPointSizeChanged(
		m_fontPointSize
	);

	return true;
} // End setFontPointSize
