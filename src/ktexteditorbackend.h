/*
 * @file    src/keditorbackend.h
 * @brief   Header for keditorbackend.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_KTEXTEDITORBACKEND_H
#define TIGCC_QT_KTEXTEDITORBACKEND_H

#include <QList>

#include "editorbackend.h"



class QLabel;
class QTabWidget;
class QWidget;

namespace KTextEditor
{
	class Document;
	class View;
}


class KTextEditorBackend : public EditorBackend
{
	Q_OBJECT

public:
	explicit KTextEditorBackend(
		QWidget *parent = nullptr
	);

	QWidget *
	widget() override;

	bool
	openFile(
		const QString &filePath,
		QString *errorMessage = nullptr
	) override;

	bool
	saveCurrentFile(
		QString *errorMessage = nullptr
	) override;

	bool
	hasModifiedFiles() const override;

	bool
	saveAllFiles(
		QString *errorMessage = nullptr
	) override;

	bool
	closeCurrentFile(
		QString *errorMessage = nullptr
	) override;

	bool
	closeAllFiles(
		QString *errorMessage = nullptr
	) override;

	QString
	currentFilePath() const override;

	bool
	isModified() const override;

private slots:
	void
	handleCurrentTabChanged(
		int index
	);

	void
	handleTabCloseRequested(
		int index
	);

	void
	documentModifiedChanged(
		KTextEditor::Document *document
	);

private:
	struct DocumentEntry
	{
		KTextEditor::Document *document;
		KTextEditor::View *view;
		QWidget *widget;
		QString filePath;
		int tabIndex;
	};

	DocumentEntry *
	currentDocumentEntry() const;

	DocumentEntry *
	documentEntryForPath(
		const QString &filePath
	) const;

	QLabel *
	createEmptyStateWidget();

	void
	configureView(
		KTextEditor::View *view
	);

	void
	showEmptyState();

	void
	hideEmptyState();

	void
	emitCurrentDocumentState();

	QLabel *m_emptyState;
	QTabWidget *m_tabs;
	QList<DocumentEntry *> m_documents;
};
#endif // TIGCC_QT_KTEXTEDITORBACKEND_H
