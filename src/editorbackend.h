/*
 * @file    src/editorbackend.h
 * @brief   Header file for editorbackend.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_EDITORBACKEND_H
#define TIGCC_QT_EDITORBACKEND_H

#include <QObject>
#include <QString>




class QWidget;

class EditorBackend : public QObject
{
    Q_OBJECT

public:
	explicit EditorBackend(
		QObject *parent = nullptr
	);

	~EditorBackend() override = default;

	virtual QWidget *
	widget() = 0;

	virtual bool
	openFile(
		const QString &filePath,
		QString *errorMessage = nullptr
	) = 0;

	virtual bool
	saveCurrentFile(
		QString *errorMessage = nullptr
	) = 0;

	virtual bool
	hasModifiedFiles() const = 0;

	virtual bool
	saveAllFiles(
		QString *errorMessage = nullptr
	) = 0;

	virtual bool
	discardAllChanges(
		QString *errorMessage = nullptr
	) = 0;

	virtual bool
	closeCurrentFile(
		QString *errorMessage = nullptr
	) = 0;

	virtual bool
	closeAllFiles(
		QString *errorMessage = nullptr
	) = 0;
	
	virtual QString
	currentFilePath() const = 0;

	virtual bool
	isModified() const = 0;

	virtual int
	fontPointSize() const = 0;

	virtual bool
	setFontPointSize(
		int pointSize,
		QString *errorMessage = nullptr
	) = 0;

signals:
	void
	currentFileChanged(
		const QString &filePath
	);

	void
	modificationChanged(
		bool modified
	);

	void
	editorError(
		const QString &message
	);

	void
	fontPointSizeChanged(
		int pointSize
	);
};

#endif // TIGCC_QT_EDITORBACKEND_H
