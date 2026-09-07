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

#include <QList>

#include "editorbackend.h"



class QEventLoop;
class QFileSystemWatcher;
class QLabel;
class QTabWidget;
class QTermWidget;
class QWidget;

class VimBackend : public EditorBackend
{
public:
	explicit VimBackend(
		QWidget *parent = nullptr
	);

	~VimBackend() override;

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
	struct VimSession
	{
		QTermWidget *terminal;
		QString filePath;
		QString stateFilePath;
		QString saveAckFilePath;
		QString discardAckFilePath;
		QFileSystemWatcher *stateWatcher;
		bool modified;
		int tabIndex;
	};

	VimSession *
	currentSession() const;

	VimSession *
	sessionForPath(
		const QString &filePath
	) const;

	void
	handleTabCloseRequested(
		int index
	);

	void
	removeWelcomeTab();

	void
	restoreWelcomeTab();

	void
	sendVimCommand(
		const QString &command
	);

	void
	sendVimCommand(
		VimSession *session,
		const QString &command
	);

	void
	emitCurrentDocumentState();

	void
	readVimState(
		VimSession *session
	);

	void
	readVimSaveAcknowledgement(
		VimSession *session
	);

	void
	readVimDiscardAcknowledgement(
		VimSession *session
	);

	QString
	vimStateCommand(
		const QString &stateFilePath,
		const QString &saveAckFilePath,
		const QString &discardAckFilePath
	) const;

	void
	sendVimEditCommand(
		const QString &filePath
	);

	void
	handleSessionFinished(
		VimSession *session
	);

	void
	updateVimState();

	void
	removeSessionFiles(
		VimSession *session
	);

	void
	removeAllSessionFiles();

	QTabWidget *m_tabs;
	QLabel *m_welcomeWidget;

	QTermWidget *m_terminal;
	QString m_filePath;
	bool m_modified;

	QList<VimSession *> m_sessions;

	QString m_lastVimEvent;
	int m_fontPointSize;
	QEventLoop *m_saveLoop;
	bool m_savePending;
	bool m_saveSucceeded;
	VimSession *m_saveSession;
	QEventLoop *m_editLoop;
	bool m_editPending;
	bool m_editSucceeded;
	QEventLoop *m_discardLoop;
	bool m_discardPending;
	bool m_discardSucceeded;
	VimSession *m_discardSession;
	QEventLoop *m_closeLoop;
	bool m_closePending;
	bool m_closeSucceeded;
	VimSession *m_closeSession;
	QString m_pendingFilePath;
};

#endif // TIGCC_QT_VIMBACKEND_H
