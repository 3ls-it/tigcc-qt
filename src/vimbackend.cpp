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
#include <QFont>

#include "vimbackend.h"



VimBackend::VimBackend(
	QWidget *parent
)
	: EditorBackend(parent),
	  m_terminal(
		  new QTermWidget(
			  0,
			  parent
		  )
	  )
{
	m_terminal->setWorkingDirectory(
		QDir::homePath()
	);

	m_terminal->setShellProgram(
		QStringLiteral("vim")
	);

	m_terminal->setAutoClose(
		true
	);

	m_terminal->startShellProgram();
} // End constructor

QWidget *
VimBackend::widget()
{
	return m_terminal;
}

bool
VimBackend::openFile(
	const QString &filePath,
	QString *errorMessage
)
{
	Q_UNUSED(filePath);

	if (errorMessage != nullptr) {
		*errorMessage =
			QStringLiteral(
				"Vim file opening is not implemented yet."
			);
	}

	return false;
}

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
	return QString();
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
