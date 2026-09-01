/*
 * @file    src/vimbackend.h
 * @brief   Vim editor backend using QTermWidget.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_VIMBACKEND_H
#define TIGCC_QT_VIMBACKEND_H

class QLabel;
class QStackedWidget;
class QTermWidget;

#include "editorbackend.h"



class QFileSystemWatcher;
class QTermWidget;
class QWidget;

class VimBackend : public EditorBackend
{
public:
	explicit VimBackend(
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

	int
	fontPointSize() const override;

	bool
	setFontPointSize(
		int pointSize,
		QString *errorMessage = nullptr
	) override;

private:
	void
	emitCurrentDocumentState();

	QString
	vimStateCommand() const;

	void
	readVimState();

	void
	handleTerminalFinished();

	void
	sendVimEditCommand(
		const QString &filePath
	);

	void
	sendVimCommand(
		const QString &command
	);

	void
	updateVimState();

	QStackedWidget *m_stack;
	QLabel *m_welcomeWidget;
	QTermWidget *m_terminal;
	QString m_filePath;
	QString m_stateFilePath;
	QFileSystemWatcher *m_stateWatcher;
	bool m_modified;
	int m_fontPointSize;
};

#endif // TIGCC_QT_VIMBACKEND_H
