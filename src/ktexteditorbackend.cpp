/*
 * @file    src/ktexteditorbackend.cpp
 * @brief   KTextEditor extends the EditorBackend
 *          interface.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>

#include <QUrl>

#include "ktexteditorbackend.h"




KTextEditorBackend::KTextEditorBackend(
	QWidget *parent
)
	: EditorBackend(parent),
	  m_document(nullptr),
	  m_view(nullptr),
	  m_editorWidget(nullptr)
{
	KTextEditor::Editor *editor =
		KTextEditor::Editor::instance();

	m_document = editor->createDocument(this);

	m_view = m_document->createView(parent);

	if (m_view != nullptr) {
		m_editorWidget = m_view->editorWidget();
	}

	connect(
		m_document,
		&KTextEditor::Document::modifiedChanged,
		this,
		&KTextEditorBackend::documentModifiedChanged
	);
}


QWidget *
KTextEditorBackend::widget()
{
	return m_editorWidget;
}


bool
KTextEditorBackend::openFile(
	const QString &filePath,
	QString *errorMessage
)
{
	const QUrl fileUrl =
		QUrl::fromLocalFile(filePath);

	if (!m_document->openUrl(fileUrl)) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The editor could not open the file."
				);
		}

		return false;
	}

	emit currentFileChanged(
		currentFilePath()
	);

	emit modificationChanged(
		isModified()
	);

	return true;
}


bool
KTextEditorBackend::saveCurrentFile(
	QString *errorMessage
)
{
	if (!m_document->documentSave()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"The editor could not save the file."
				);
		}

		return false;
	}

	emit modificationChanged(
		isModified()
	);

	return true;
}


QString
KTextEditorBackend::currentFilePath() const
{
	if (m_document == nullptr) {
		return QString();
	}

	return m_document->url().toLocalFile();
}


bool
KTextEditorBackend::isModified() const
{
	if (m_document == nullptr) {
		return false;
	}

	return m_document->isModified();
}


void
KTextEditorBackend::documentModifiedChanged(
    KTextEditor::Document *document
)
{
	Q_UNUSED(document);

	emit modificationChanged(
		isModified()
	);
}
