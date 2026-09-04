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
#include <QStandardPaths>
#include <QStringList>
#include <QTabWidget>
#include <QTimer>
#include <QUuid>

#include "vimbackend.h"

//#include <QDebug>


VimBackend::VimBackend(
	QWidget *parent
)
	: EditorBackend(parent),
	  m_tabs(new QTabWidget(parent)),
	  m_welcomeWidget(nullptr),
	  m_terminal(nullptr),
	  m_stateFilePath(),
	  m_saveAckFilePath(),
	  m_stateWatcher(
		  new QFileSystemWatcher(this)
	  ),
	  m_modified(false),
	  m_fontPointSize(12),
	  m_lastVimEvent(),
	  m_saveLoop(nullptr),
	  m_savePending(false),
	  m_saveSucceeded(false),
	  m_saveSession(nullptr),
	  m_editLoop(nullptr),
	  m_editPending(false),
	  m_editSucceeded(false),
	  m_discardLoop(nullptr),
	  m_discardPending(false),
	  m_discardSucceeded(false),
	  m_closeLoop(nullptr),
	  m_closePending(false),
	  m_closeSucceeded(false),
	  m_closeSession(nullptr),
	  m_pendingFilePath()
{
	m_tabs->setDocumentMode(
		true
	);

	m_tabs->setTabsClosable(
		false
	);

	m_tabs->setMovable(
		true
	);

	// Welcome panel
	m_welcomeWidget =
		new QLabel(
			QStringLiteral(
				"No file is open.\n\n"
				"Double-click a source, header, "
				"or other file in the "
				"project tree to edit."
			),
			m_tabs
		);

	m_welcomeWidget->setAlignment(
		Qt::AlignCenter
	);

	m_welcomeWidget->setMinimumHeight(
		160
	);

	m_tabs->addTab(
		m_welcomeWidget,
		QStringLiteral("Welcome")
	);

	m_tabs->setCurrentWidget(
		m_welcomeWidget
	);

	// set up stateFile path
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

	m_saveAckFilePath =
		m_stateFilePath +
		QStringLiteral(".write");

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

	QFile saveAckFile(
		m_saveAckFilePath
	);

	if (saveAckFile.open(
			QIODevice::WriteOnly |
			QIODevice::Truncate
		)) {
		saveAckFile.close();
	}

	m_stateWatcher->addPath(
		m_stateFilePath
	);

	m_stateWatcher->addPath(
		m_saveAckFilePath
	);

	// and connect
	connect(
		m_tabs,
		&QTabWidget::currentChanged,
		this,
		[this](int index) {
			Q_UNUSED(index);

			emitCurrentDocumentState();
		}
	);

	connect(
		m_tabs,
		&QTabWidget::tabCloseRequested,
		this,
		&VimBackend::handleTabCloseRequested
	);
} // End constructor


QWidget *
VimBackend::widget()
{
	return m_tabs;
}


VimBackend::VimSession *
VimBackend::currentSession() const
{
	// Use current tab widget not tabIndex, which can
	// change if tabs are moved.
	QWidget *currentWidget =
		m_tabs->currentWidget();

	if (currentWidget == nullptr) {
		return nullptr;
	}

	for (VimSession *session :
			m_sessions) {
		if (session->terminal ==
			currentWidget) {
			return session;
		}
	}

	return nullptr;
} // End currentSession


VimBackend::VimSession *
VimBackend::sessionForPath(
	const QString &filePath
) const
{
	const QString normalizedPath =
		QFileInfo(
			filePath
		).absoluteFilePath();

	for (VimSession *session :
			m_sessions) {
		if (session->filePath ==
			normalizedPath) {
			return session;
		}
	}

	return nullptr;
} // End sessionForPath
 

void
VimBackend::sendVimCommand(
	VimSession *session,
	const QString &command
)
{
	if (session == nullptr ||
		session->terminal == nullptr) {
		return;
	}

	const QString vimCommand =
		QString(QChar(0x1b)) +
		QStringLiteral(":") +
		command +
		QStringLiteral("\r");

	session->terminal->sendText(
		vimCommand
	);
} // End sendVimCommand


void
VimBackend::removeWelcomeTab()
{
	const int welcomeTabIndex =
		m_tabs->indexOf(
			m_welcomeWidget
		);

	if (welcomeTabIndex >= 0) {
		m_tabs->removeTab(
			welcomeTabIndex
		);
	}
} // End removeWelcomeTab


void
VimBackend::restoreWelcomeTab()
{
	if (m_tabs->indexOf(
			m_welcomeWidget
		) < 0) {
		m_tabs->addTab(
			m_welcomeWidget,
			QStringLiteral("Welcome")
		);
	}

	m_tabs->setCurrentWidget(
		m_welcomeWidget
	);

	m_tabs->setTabsClosable(
		false
	);
} // End restoreWelcomeTab


void
VimBackend::handleTabCloseRequested(
	int index
)
{
	if (index < 0 ||
		index >= m_tabs->count()) {
		return;
	}

	if (m_tabs->widget(index) ==
		m_welcomeWidget) {
		return;
	}

	m_tabs->setCurrentIndex(
		index
	);

	QString errorMessage;

	if (!closeCurrentFile(
			&errorMessage
		)) {
		if (!errorMessage.isEmpty()) {
			emit editorError(
				errorMessage
			);
		}
	}
} // End handleTabCloseRequested


QString
VimBackend::vimStateCommand(
	const QString &stateFilePath,
	const QString &saveAckFilePath
) const
{
	QString escapedStateFilePath =
		stateFilePath;

	escapedStateFilePath.replace(
		QStringLiteral("'"),
		QStringLiteral("''")
	);

	QString escapedSaveAckFilePath =
		saveAckFilePath;

	escapedSaveAckFilePath.replace(
		QStringLiteral("'"),
		QStringLiteral("''")
	);

	// Construct a Vimscript string
	return QStringLiteral(
		"let g:tigcc_qt_state_file = '%1' | "
		"let g:tigcc_qt_save_ack_file = '%2' | "
		"augroup TigccQtState | "
		"autocmd! | "

		"execute \"autocmd "
		"BufEnter,BufFilePost,TextChanged,TextChangedI * "
		"call writefile([expand('%:p'), "
		"&modified ? '1' : '0', 'state'], "
		"g:tigcc_qt_state_file)\" | "

		"execute \"autocmd "
		"BufReadPost * "
		"call writefile([expand('%:p'), "
		"&modified ? '1' : '0', 'discard'], "
		"g:tigcc_qt_state_file)\" | "

		"execute \"autocmd "
		"BufWritePost * "
		"call writefile([expand('%:p'), "
		"&modified ? '1' : '0', 'state'], "
		"g:tigcc_qt_state_file)\" | "

		"execute \"autocmd "
		"BufWritePost * "
		"call writefile([expand('%:p'), "
		"'write'], "
		"g:tigcc_qt_save_ack_file)\" | "

		"execute \"autocmd "
		"VimLeavePre * "
		"call writefile(['', '0', 'exit'], "
		"g:tigcc_qt_state_file)\" | "

		"augroup END"
	).arg(
		escapedStateFilePath,
		escapedSaveAckFilePath
	);
} // End vimStateCommand
 

void
VimBackend::readVimState(
	VimBackend::VimSession *session
	)
{
	if (session == nullptr) {
		return;
	}

	QFile stateFile(
		session->stateFilePath
	);

	if (!stateFile.open(
			QIODevice::ReadOnly
		)) {
		return;
	}

	const QByteArray stateData =
		stateFile.readAll();

	const QStringList lines =
		QString::fromUtf8(
			stateData
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

		session->filePath =
			normalizedReportedPath;

		session->modified =
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
		session->filePath =
			QFileInfo(
				reportedFilePath
			).absoluteFilePath();
	}

	session->modified =
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
		session->modified =
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
VimBackend::readVimSaveAcknowledgement(
	VimBackend::VimSession *session
)
{
	if (session == nullptr) {
		return;
	}

	QFile saveAckFile(
		session->saveAckFilePath
	);

	if (!saveAckFile.open(
			QIODevice::ReadOnly
		)) {
		return;
	}

	const QStringList lines =
		QString::fromUtf8(
			saveAckFile.readAll()
		).split(
			QChar('\n')
		);

	saveAckFile.close();

	if (lines.size() < 2) {
		return;
	}

	const QString reportedFilePath =
		lines.at(0).trimmed();

	const QString reportedEvent =
		lines.at(1).trimmed();

	if (reportedEvent !=
		QStringLiteral("write")) {
		return;
	}

	if (!m_savePending ||
		m_saveSession != session) {
		return;
	}

	const QString normalizedReportedPath =
		QFileInfo(
			reportedFilePath
		).absoluteFilePath();

	const QString normalizedCurrentPath =
		QFileInfo(
			session->filePath
		).absoluteFilePath();

	if (normalizedReportedPath !=
		normalizedCurrentPath) {
		return;
	}

	m_savePending = false;

	m_saveSucceeded = true;

	session->modified = false;

	if (m_saveLoop != nullptr) {
		m_saveLoop->quit();
	}

	emitCurrentDocumentState();
} // End readVimSaveAcknowledgemen


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
VimBackend::handleSessionFinished(
	VimSession *session
)
{
	if (session == nullptr) {
		return;
	}

	const int tabIndex =
		m_tabs->indexOf(
			session->terminal
		);

	if (tabIndex >= 0) {
		m_tabs->removeTab(
			tabIndex
		);
	}

	m_sessions.removeOne(
		session
	);

	if (m_closePending && m_closeSession == session) {
		m_closePending = false;
		m_closeSucceeded =true;

		if (m_closeLoop != nullptr)
			m_closeLoop->quit();
		
	}

	if (session->stateWatcher != nullptr) {
		session->stateWatcher->removePath(
			session->stateFilePath
		);

		session->stateWatcher->removePath(
			session->saveAckFilePath
		);

		session->stateWatcher->deleteLater();
	}

	QFile::remove(
		session->stateFilePath
	);

	QFile::remove(
		session->saveAckFilePath
	);

	if (session->terminal != nullptr) {
		session->terminal->deleteLater();
	}

	delete session;

	for (VimSession *remaining :
			m_sessions) {
		remaining->tabIndex =
			m_tabs->indexOf(
				remaining->terminal
			);
	}

	if (m_sessions.isEmpty()) {
		restoreWelcomeTab();
	} else {
		m_tabs->setTabsClosable(
			true
		);
	}

	emitCurrentDocumentState();
} // End handleSessionFinished


bool
VimBackend::openFile(
	const QString &filePath,
	QString *errorMessage
)
{
	// Validation and existing session lookup
	const QFileInfo fileInfo(
		filePath
	);

	if (!fileInfo.exists() ||
		!fileInfo.isFile()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The file does not exist:\n%1"
				).arg(
					filePath
				);
		}

		return false;
	}

	const QString normalizedPath =
		fileInfo.absoluteFilePath();

	VimSession *existingSession =
		sessionForPath(
			normalizedPath
		);

	if (existingSession != nullptr) {
		m_tabs->setCurrentWidget(
			existingSession->terminal
		);

		return true;
	}

	// Create a new session
	const QString temporaryDirectory =
		QStandardPaths::writableLocation(
			QStandardPaths::TempLocation
		);

	const QString sessionId =
		QUuid::createUuid().toString(
			QUuid::WithoutBraces
		);

	const QString stateFilePath =
		QDir(temporaryDirectory).filePath(
			QStringLiteral(
				"tigcc-qt-vim-%1.state"
			).arg(
				sessionId
			)
		);

	const QString saveAckFilePath =
		stateFilePath +
		QStringLiteral(".write");

	auto *terminal =
		new QTermWidget(
			0,
			m_tabs
		);

	terminal->setWorkingDirectory(
		fileInfo.absolutePath()
	);

	terminal->setShellProgram(
		QStringLiteral("vim")
	);

	terminal->setAutoClose(
		true
	);

	QFont terminalFont =
		terminal->getTerminalFont();

	terminalFont.setPointSize(
		m_fontPointSize
	);

	terminal->setTerminalFont(
		terminalFont
	);

	auto *stateWatcher =
		new QFileSystemWatcher(
			this
		);

	auto *session =
		new VimSession{
			terminal,
			normalizedPath,
			stateFilePath,
			saveAckFilePath,
			stateWatcher,
			false,
			-1
		};

	m_sessions.append(
		session
	);

	// Create per-session state files
	QFile stateFile(
		session->stateFilePath
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

	QFile saveAckFile(
		session->saveAckFilePath
	);

	if (saveAckFile.open(
			QIODevice::WriteOnly |
			QIODevice::Truncate
		)) {
		saveAckFile.close();
	}

	session->stateWatcher->addPath(
		session->stateFilePath
	);

	session->stateWatcher->addPath(
		session->saveAckFilePath
	);

	// Connect session-specific singnals
	connect(
		session->terminal,
		&QTermWidget::finished,
		this,
		[this, session]() {
			handleSessionFinished(
				session
			);
		}
	);

	connect(
		session->stateWatcher,
		&QFileSystemWatcher::fileChanged,
		this,
		[this, session](const QString &path) {
			if (path ==
				session->stateFilePath) {
				readVimState(
					session
				);

				if (!session->stateWatcher->files().contains(
						session->stateFilePath
					)) {
					session->stateWatcher->addPath(
						session->stateFilePath
					);
				}
			}

			if (path ==
				session->saveAckFilePath) {
				readVimSaveAcknowledgement(
					session
				);

				if (!session->stateWatcher->files().contains(
						session->saveAckFilePath
					)) {
					session->stateWatcher->addPath(
						session->saveAckFilePath
					);
				}
			}
		}
	);

	// Configure Vim
	QStringList arguments;

	arguments.append(
		QStringLiteral("--cmd")
	);

	arguments.append(
		vimStateCommand(
			session->stateFilePath,
			session->saveAckFilePath
		)
	);

	arguments.append(
		session->filePath
	);

	session->terminal->setArgs(
		arguments
	);

	// Remove welcome tab and add new tab
	removeWelcomeTab();

	const int tabIndex =
		m_tabs->addTab(
			session->terminal,
			fileInfo.fileName()
		);

	session->tabIndex =
		tabIndex;

	m_tabs->setTabsClosable(
		true
	);

	m_tabs->setCurrentIndex(
		tabIndex
	);

	// Start Vim
	session->terminal->startShellProgram();

	emit currentFileChanged(
		session->filePath
	);

	emit modificationChanged(
		false
	);

	return true;
} // End openFile


bool
VimBackend::saveCurrentFile(
	QString *errorMessage
)
{
	VimSession *session =
		currentSession();

	if (session == nullptr) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"No Vim session is currently active."
				);
		}

		return false;
	}

	QEventLoop saveLoop;
	QTimer timeoutTimer;

	timeoutTimer.setSingleShot(
		true
	);

	m_saveLoop = &saveLoop;

	m_saveSession = session;

	m_savePending = true;

	m_saveSucceeded = false;

	QFile saveAckFile(
		session->saveAckFilePath
	);

	if (!saveAckFile.open(
			QIODevice::WriteOnly |
			QIODevice::Truncate
		)) {
		if (errorMessage != nullptr) {
			*errorMessage =
				saveAckFile.errorString();
		}

		m_saveLoop = nullptr;

		m_saveSession = nullptr;

		m_savePending = false;

		return false;
	}

	saveAckFile.close();

	connect(
		&timeoutTimer,
		&QTimer::timeout,
		&saveLoop,
		&QEventLoop::quit
	);

	timeoutTimer.start(
		5000
	);

	sendVimCommand(
		session,
		QStringLiteral("write")
	);

	saveLoop.exec();

	m_saveLoop = nullptr;

	m_saveSession = nullptr;

	if (!m_saveSucceeded) {
		m_savePending = false;

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
	for (VimSession *session :
			m_sessions) {
		if (session->modified) {
			return true;
		}
	}

	return false;
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
	VimSession *session =
		currentSession();

	if (session == nullptr) {
		return true;
	}

	if (session->modified) {
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

	m_closeSession =
		session;

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
		session,
		QStringLiteral("quit")
	);

	closeLoop.exec();

	m_closeLoop =
		nullptr;

	m_closeSession =
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

	return closeCurrentFile(
		errorMessage
	);
}


QString
VimBackend::currentFilePath() const
{
	VimSession *session =
		currentSession();

	if (session == nullptr) {
		return QString();
	}

	return session->filePath;
}


bool
VimBackend::isModified() const
{
	VimSession *session =
		currentSession();

	if (session == nullptr) {
		return false;
	}

	return session->modified;
}


void
VimBackend::emitCurrentDocumentState()
{
	VimSession *session =
		currentSession();

	if (session == nullptr) {
		emit currentFileChanged(
			QString()
		);

		emit modificationChanged(
			false
		);

		return;
	}

	emit currentFileChanged(
		session->filePath
	);

	emit modificationChanged(
		session->modified
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
