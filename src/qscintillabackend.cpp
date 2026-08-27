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

#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QSignalBlocker>
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

	connect(
		m_editor,
		&QsciScintilla::textChanged,
		this,
		[this]() {
			emit modificationChanged(
				m_editor->isModified()
			);
		}
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

	QFile file(
		fileInfo.absoluteFilePath()
	);

	if (!file.open(
			QIODevice::ReadOnly
		)) {
		if (errorMessage != nullptr) {
			*errorMessage =
				file.errorString();
		}

		return false;
	}

	const QByteArray fileData =
		file.readAll();

	file.close();

	const QString fileText =
		QString::fromUtf8(
			fileData
		);

	/*
	 * Prevent the intermediate setText() operation
	 * from producing a spurious application-level
	 * modification notification.
	 */
	const QSignalBlocker signalBlocker(
		m_editor
	);

	m_editor->setText(
		fileText
	);

	m_editor->setModified(
		false
	);

	m_filePath =
		fileInfo.absoluteFilePath();

	emit currentFileChanged(
		m_filePath
	);

	emit modificationChanged(
		false
	);

	return true;
} // End openFile


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
	return m_editor->isModified();
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
	return m_filePath;
}


bool
QScintillaBackend::isModified() const
{
	return m_editor->isModified();
}
