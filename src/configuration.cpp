
/*
 * @file    src/configuration.cpp
 * @brief   Implements application configuration values.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

#include "configuration.h"



Configuration::Configuration()
	: m_defaultEditor(
		EditorBackendType::QScintilla
	),
	  m_editorFontPointSize(12)
{
}


EditorBackendType
Configuration::defaultEditor() const
{
	return m_defaultEditor;
}


void
Configuration::setDefaultEditor(
	EditorBackendType editor
)
{
	m_defaultEditor =
		editor;
}


int
Configuration::editorFontPointSize() const
{
	return m_editorFontPointSize;
}


void
Configuration::setEditorFontPointSize(
	int pointSize
)
{
	if (pointSize < 6 ||
		pointSize > 36) {
		return;
	}

	m_editorFontPointSize =
		pointSize;
}


QString
Configuration::m_configFilePath() const
{
	const QString configDirectory =
		QStandardPaths::writableLocation(
			QStandardPaths::AppConfigLocation
		);

	return QDir(
		configDirectory
	).filePath(
		QStringLiteral("config.json")
	);
}


QString
Configuration::configFilePath() const
{
	return m_configFilePath();
}


QString
Configuration::editorTypeToString(
	EditorBackendType editor
)
{
	switch (editor) {
		case EditorBackendType::QScintilla:
			return QStringLiteral("qscintilla");

		case EditorBackendType::Vim:
			return QStringLiteral("vim");

#ifdef USE_KTEXTEDITOR
		case EditorBackendType::KTextEditor:
			return QStringLiteral("ktexteditor");
#endif
	}

	return QStringLiteral("qscintilla");
}


EditorBackendType
Configuration::editorTypeFromString(
	const QString &value,
	EditorBackendType fallback
)
{
	const QString normalizedValue =
		value.trimmed().toLower();

	if (normalizedValue ==
		QStringLiteral("qscintilla")) {
		return EditorBackendType::QScintilla;
	}

	if (normalizedValue ==
		QStringLiteral("vim")) {
		return EditorBackendType::Vim;
	}

#ifdef USE_KTEXTEDITOR
	if (normalizedValue ==
		QStringLiteral("ktexteditor")) {
		return EditorBackendType::KTextEditor;
	}
#endif

	return fallback;
}


bool
Configuration::load(
	QString *errorMessage
)
{
	const QString filePath =
		m_configFilePath();

	QFile file(
		filePath
	);

	if (!file.exists()) {
		return true;
	}

	if (!file.open(
			QIODevice::ReadOnly
		)) {
		if (errorMessage != nullptr) {
			*errorMessage =
				file.errorString();
		}

		return false;
	}

	const QByteArray data =
		file.readAll();

	file.close();

	QJsonParseError parseError;

	const QJsonDocument document =
		QJsonDocument::fromJson(
			data,
			&parseError
		);

	if (parseError.error !=
			QJsonParseError::NoError ||
		!document.isObject()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				parseError.errorString();
		}

		return false;
	}

	const QJsonObject root =
		document.object();

	const QJsonObject editorObject =
		root.value(
			QStringLiteral("editor")
		).toObject();

	const QString editorName =
		editorObject.value(
			QStringLiteral("defaultBackend")
		).toString();

	m_defaultEditor =
		editorTypeFromString(
			editorName,
			m_defaultEditor
		);

	const int fontPointSize =
		editorObject.value(
			QStringLiteral("fontPointSize")
		).toInt(
			m_editorFontPointSize
		);

	setEditorFontPointSize(
		fontPointSize
	);

	return true;
}


bool
Configuration::save(
	QString *errorMessage
) const
{
	const QString filePath =
		m_configFilePath();

	const QFileInfo fileInfo(
		filePath
	);

	QDir configDirectory;

	if (!configDirectory.mkpath(
			fileInfo.absolutePath()
		)) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Could not create the "
					"configuration directory."
				);
		}

		return false;
	}

	QJsonObject editorObject;

	editorObject.insert(
		QStringLiteral("defaultBackend"),
		editorTypeToString(
			m_defaultEditor
		)
	);

	editorObject.insert(
		QStringLiteral("fontPointSize"),
		m_editorFontPointSize
	);

	QJsonObject root;

	root.insert(
		QStringLiteral("formatVersion"),
		1
	);

	root.insert(
		QStringLiteral("editor"),
		editorObject
	);

	const QJsonDocument document(
		root
	);

	QFile file(
		filePath
	);

	if (!file.open(
			QIODevice::WriteOnly |
			QIODevice::Truncate
		)) {
		if (errorMessage != nullptr) {
			*errorMessage =
				file.errorString();
		}

		return false;
	}

	const QByteArray data =
		document.toJson(
			QJsonDocument::Indented
		);

	if (file.write(
			data
		) != data.size()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				file.errorString();
		}

		file.close();

		return false;
	}

	file.close();

	return true;
}
