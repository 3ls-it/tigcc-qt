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

#include "editorbackend.h"



class QTabWidget;
class QWidget;
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
	void
	configureEditorAppearance(
		QsciLexerCPP *lexer
	);

	QTabWidget *m_tabs;
	QsciScintilla *m_editor;
	QString m_filePath;
};

#endif // TIGCC_QT_QSCINTILLABACKEND_H
