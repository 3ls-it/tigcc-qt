/*
 * @file    src/projectmanagement.cpp
 * @brief   Provides methods for managing a project.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "projectmanager.h"




bool
ProjectManager::saveProject(
	const Project &project,
	const QString &filePath,
	QString *errorMessage
) const
{
	QJsonObject projectObject;

	projectObject.insert(
		QStringLiteral("formatVersion"),
		1
	);

	projectObject.insert(
		QStringLiteral("name"),
		project.name()
	);

	QJsonArray sourceFiles;

	for (const QString &path : project.sourceFiles()) {
		sourceFiles.append(path);
	}

	projectObject.insert(
		QStringLiteral("sourceFiles"),
		sourceFiles
	);

	QJsonArray headerFiles;

	for (const QString &path : project.headerFiles()) {
		headerFiles.append(path);
	}

	projectObject.insert(
		QStringLiteral("headerFiles"),
		headerFiles
	);

	QFile file(filePath);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		if (errorMessage != nullptr) {
			*errorMessage = file.errorString();
		}

		return false;
	}

	const QJsonDocument document(projectObject);

	file.write(
		document.toJson(QJsonDocument::Indented)
	);

	file.close();

	return true;
}

bool
ProjectManager::loadProject(
	const QString &filePath,
	Project *project,
	QString *errorMessage
) const
{
	if (project == nullptr) {
		if (errorMessage != nullptr) {
			*errorMessage = QStringLiteral(
				"No destination project was provided."
			);
		}

		return false;
	}

	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) {
		if (errorMessage != nullptr) {
			*errorMessage = file.errorString();
		}

		return false;
	}

	const QByteArray data = file.readAll();
	file.close();

	QJsonParseError parseError;

	const QJsonDocument document =
		QJsonDocument::fromJson(
			data,
			&parseError
		);

	if (parseError.error != QJsonParseError::NoError ||
		!document.isObject()) {
		if (errorMessage != nullptr) {
			*errorMessage = parseError.errorString();
		}

		return false;
	}

	const QJsonObject projectObject =
		document.object();

	const QString name =
		projectObject.value(
			QStringLiteral("name")
		).toString();

	const QFileInfo projectFileInfo(filePath);

	Project loadedProject(
		name,
		projectFileInfo.absolutePath()
	);

	const QJsonArray sourceFiles =
		projectObject.value(
			QStringLiteral("sourceFiles")
		).toArray();

	for (const QJsonValue &value : sourceFiles) {
		loadedProject.addSourceFile(
			value.toString()
		);
	}

	const QJsonArray headerFiles =
		projectObject.value(
			QStringLiteral("headerFiles")
		).toArray();

	for (const QJsonValue &value : headerFiles) {
		loadedProject.addHeaderFile(
			value.toString()
		);
	}

	*project = loadedProject;

	return true;
}
