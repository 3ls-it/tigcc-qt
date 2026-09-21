/*
 * @file    src/completiondatalocator.cpp
 * @brief	  
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

#include "completiondatalocator.h"



namespace
{

const QString kCompletionFileName =
	QStringLiteral(
		"tigcc-completions.json"
	);


QString
existingFile(
	const QString &path
)
{
	const QFileInfo fileInfo(
		path
	);

	if (fileInfo.exists() &&
		fileInfo.isFile() &&
		fileInfo.isReadable()) {
		return fileInfo.absoluteFilePath();
	}

	return QString();
}

}


QString
CompletionDataLocator::locate(
	QString *errorMessage
)
{
	/*
	 * Installed/application-data locations.
	 */
	const QString installedPath =
		QStandardPaths::locate(
			QStandardPaths::AppDataLocation,
			kCompletionFileName,
			QStandardPaths::LocateFile
		);

	if (!installedPath.isEmpty()) {
		return installedPath;
	}

	/*
	 * Prefix-relative fallback:
	 *
	 * /prefix/bin/tigcc-qt
	 * /prefix/share/tigcc-qt/tigcc-completions.json
	 *
	 * This covers normal FreeBSD, Linux, and Termux
	 * installations.
	 */
	const QString applicationDirectory =
		QCoreApplication::applicationDirPath();

	const QString prefixRelativePath =
		QDir(
			applicationDirectory
	).filePath(
		QStringLiteral(
			"../share/tigcc-qt/%1"
		).arg(
			kCompletionFileName
		)
	);

	const QString installedFallback =
		existingFile(
			prefixRelativePath
		);

	if (!installedFallback.isEmpty()) {
		return installedFallback;
	}

	/*
	 * Development-tree fallback:
	 *
	 * build/tigcc-qt
	 * ../data/tigcc-completions.json
	 */
	const QString developmentPath =
		QDir(
			applicationDirectory
		).filePath(
			QStringLiteral(
				"../data/%1"
			).arg(
				kCompletionFileName
			)
		);

	const QString developmentFile =
		existingFile(
			developmentPath
		);

	if (!developmentFile.isEmpty()) {
		return developmentFile;
	}

	/*
	 * Final development convenience fallback.
	 * Useful when launching from the repository root.
	 */
	const QString currentDirectoryPath =
		QDir::current().filePath(
			QStringLiteral(
				"data/%1"
			).arg(
				kCompletionFileName
			)
		);

	const QString currentDirectoryFile =
		existingFile(
			currentDirectoryPath
		);

	if (!currentDirectoryFile.isEmpty()) {
		return currentDirectoryFile;
	}

	if (errorMessage != nullptr) {
		*errorMessage =
			QStringLiteral(
				"Could not locate %1."
			).arg(
				kCompletionFileName
			);
	}

	return QString();
}
