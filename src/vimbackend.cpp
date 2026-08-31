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

#include <QDir>
#include <QLabel>
#include <QFileInfo>
#include <QFont>
#include <QStackedWidget>
#include <QStringList>

#include "vimbackend.h"



VimBackend::VimBackend(
	QWidget *parent
)
	: EditorBackend(parent),
	  m_stack(new QStackedWidget(parent)),
	  m_welcomeWidget(nullptr),
	  m_terminal(nullptr)
{
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
} // End constructor


QWidget *
VimBackend::widget()
{
	return m_stack;
}


bool
VimBackend::openFile(
	const QString &filePath,
	QString *errorMessage
)
{
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

	if (m_terminal != nullptr) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"A Vim session is already active."
				);
		}

		return false;
	}

	m_terminal =
		new QTermWidget(
			0,
			m_stack
		);

	m_terminal->setWorkingDirectory(
		fileInfo.absolutePath()
	);

	m_terminal->setShellProgram(
		QStringLiteral("vim")
	);

	QStringList arguments;

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

	emit currentFileChanged(
		m_filePath
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
	if (errorMessage != nullptr) {
		*errorMessage =
			QStringLiteral(
				"Vim file saving is not implemented yet."
			);
	}

	return false;
}


bool
VimBackend::hasModifiedFiles() const
{
	return false;
}


bool
VimBackend::saveAllFiles(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	return true;
}


bool
VimBackend::discardAllChanges(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	return true;
}


bool
VimBackend::closeCurrentFile(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	return true;
}


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
	return false;
}


int
VimBackend::fontPointSize() const
{
	return 10;
}


bool
VimBackend::setFontPointSize(
	int pointSize,
	QString *errorMessage
)
{
	Q_UNUSED(pointSize);

	if (errorMessage != nullptr) {
		*errorMessage =
			QStringLiteral(
				"Font-size changes are not implemented "
				"for the Vim backend."
			);
	}

	return false;
}
