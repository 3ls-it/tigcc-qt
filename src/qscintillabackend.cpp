/*
 * @file    src/qscintillabackend.cpp
 * @brief   
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Qsci/qsciscintilla.h>
#include <QTabWidget>

#include "qscintillabackend.h"



QScintillaBackend::QScintillaBackend(
	QWidget *parent
)
	: EditorBackend(parent),
	  m_tabs(new QTabWidget(parent)),
	  m_editor(new QsciScintilla(m_tabs))
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

	m_editor->setUtf8(
		true
	);

	m_tabs->addTab(
		m_editor,
		QStringLiteral("QScintilla")
	);
}

QWidget *
QScintillaBackend::widget()
{
	return m_tabs;
}

bool
QScintillaBackend::openFile(
	const QString &filePath,
	QString *errorMessage
)
{
	Q_UNUSED(filePath);

	if (errorMessage != nullptr) {
		*errorMessage =
			QStringLiteral(
				"QScintilla file loading is not "
				"implemented yet."
			);
	}

	return false;
}

bool
QScintillaBackend::saveCurrentFile(
	QString *errorMessage
)
{
	if (errorMessage != nullptr) {
		*errorMessage =
			QStringLiteral(
				"QScintilla file saving is not "
				"implemented yet."
			);
	}

	return false;
}

bool
QScintillaBackend::hasModifiedFiles() const
{
	return false;
}

bool
QScintillaBackend::saveAllFiles(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	return true;
}

bool
QScintillaBackend::discardAllChanges(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	return true;
}

bool
QScintillaBackend::closeCurrentFile(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	return true;
}

bool
QScintillaBackend::closeAllFiles(
	QString *errorMessage
)
{
	Q_UNUSED(errorMessage);

	return true;
}

QString
QScintillaBackend::currentFilePath() const
{
	return QString();
}

bool
QScintillaBackend::isModified() const
{
	return false;
}
