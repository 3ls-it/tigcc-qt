/*
 * @file    src/configuration.h
 * @brief   Application configuration values.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_CONFIGURATION_H
#define TIGCC_QT_CONFIGURATION_H

#include <QString>

#include "editorbackendfactory.h"



class Configuration
{
public:
	Configuration();

	EditorBackendType
	defaultEditor() const;

	void
	setDefaultEditor(
		EditorBackendType editor
	);

	int
	editorFontPointSize() const;

	void
	setEditorFontPointSize(
		int pointSize
	);

	QString
	configFilePath() const;

	bool
	load(
		QString *errorMessage = nullptr
	);

	bool
	save(
		QString *errorMessage = nullptr
	) const;

private:
	static QString
	editorTypeToString(
		EditorBackendType editor
	);

	static EditorBackendType
	editorTypeFromString(
		const QString &value,
		EditorBackendType fallback
	);

	QString m_configFilePath() const;
	EditorBackendType m_defaultEditor;
	int m_editorFontPointSize;
};

#endif // TIGCC_QT_CONFIGURATION_H
