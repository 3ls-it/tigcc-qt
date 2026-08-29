/*
 * @file    src/qscintillabackend.h
 * @brief   Header file for src/qscintillabackend.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_QSCINTILLABACKEND_H
#define TIGCC_QT_QSCINTILLABACKEND_H

#include <QList>

#include "editorbackend.h"



class QTabWidget;
class QWidget;
class QLabel;
class QsciScintilla;
class QsciLexerCPP;


class QScintillaBackend : public EditorBackend
{
public:
	explicit QScintillaBackend(
		QWidget *parent = nullptr
	);

	QWidget *
	widget() override;

	int
	fontPointSize() const override;

	bool
	setFontPointSize(
			int pointSize,
			QString *errorMessage = nullptr
	) override;

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
	discardAllChanges(
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


private:
	struct DocumentEntry
	{
		QsciScintilla *editor;
		QsciLexerCPP *lexer;
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

	bool
	saveDocumentEntry(
		DocumentEntry *entry,
		QString *errorMessage
	);

	void
	emitCurrentDocumentState();

	void
	updateTabTitle(
		DocumentEntry *entry
	);

	QLabel *
	createEmptyStateWidget();

	void
	showEmptyState();

	void
	hideEmptyState();

	void
	configureEditorAppearance(
		QsciLexerCPP *lexer,
		QsciScintilla *editor
	);

	QTabWidget *m_tabs;
	QLabel *m_emptyState;
	QList<DocumentEntry *> m_documents;
	int m_fontPointSize;
};

#endif // TIGCC_QT_QSCINTILLABACKEND_H
