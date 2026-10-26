/*
 * @file    src/project.cpp
 * @brief   Encapsulates a TIGCC project.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */
#include <QDir>
#include <QDirIterator>
//#include <QFileInfo>
#include <QSet>

#include "project.h"
#include "projectfilecategory.h"



Project::Project()
{
}

Project::Project(
	const QString &name,
	const QString &directory
)
	: m_name(name),
	  m_directory(directory)
{
}

QString
Project::name() const
{
	return m_name;
}

void
Project::setName(const QString &name)
{
	m_name = name;
}

QString
Project::directory() const
{
	return m_directory;
}

void
Project::setDirectory(const QString &directory)
{
	m_directory = directory;
}

QStringList
Project::headerFiles() const
{
	return m_headerFiles;
}

QStringList
Project::sourceFiles() const
{
	return m_sourceFiles;
}

QStringList
Project::gasFiles() const
{
	return m_gasFiles;
}

QStringList
Project::additionalFiles() const
{
	return m_additionalFiles;
}


// Additional files add/remove
void
Project::addAdditionalFile(const QString &path)
{
	if (!path.isEmpty() &&
		!m_additionalFiles.contains(path)) {
		m_additionalFiles.append(path);
	}
}

void
Project::removeAdditionalFile(const QString &path)
{
	m_additionalFiles.removeAll(path);
}


// Header files add/remove
void
Project::addHeaderFile(const QString &path)
{
	if (!path.isEmpty() && !m_headerFiles.contains(path)) {
		m_headerFiles.append(path);
	}
}

void
Project::removeHeaderFile(const QString &path)
{
	m_headerFiles.removeAll(path);
}


// Source files add/remove
void
Project::addSourceFile(const QString &path)
{
	if (!path.isEmpty() && !m_sourceFiles.contains(path)) {
		m_sourceFiles.append(path);
	}
}

void
Project::removeSourceFile(const QString &path)
{
	m_sourceFiles.removeAll(path);
}


// GAS files add/remove
void
Project::addGasFile(const QString &path)
{
	if (!path.isEmpty() && !m_gasFiles.contains(path)) {
		m_gasFiles.append(path);
	}
}

void
Project::removeGasFile(const QString &path)
{
	m_gasFiles.removeAll(path);
}


QList<ProjectFile>
Project::visibleFiles() const
{
	QList<ProjectFile> result;
	QSet<QString> seenPaths;

	auto appendFile =
		[&result, &seenPaths](
			const QString &path,
			bool explicitlyListed,
			bool automaticallyDiscovered
		) {
			const QString relativePath =
				QDir::cleanPath(path);

			if (relativePath.isEmpty() ||
				relativePath == QStringLiteral(".")) {
				return;
			}

			if (seenPaths.contains(relativePath)) {
				return;
			}

			seenPaths.insert(relativePath);

			ProjectFile projectFile;
			projectFile.relativePath =
				relativePath;

			projectFile.category =
				classifyProjectFile(relativePath);

			projectFile.explicitlyListed =
				explicitlyListed;

			projectFile.automaticallyDiscovered =
				automaticallyDiscovered;

			result.append(projectFile);
		};

	/*
	 * Preserve files explicitly stored in the project model.
	 *
	 * These lists are retained for compatibility with the existing
	 * project representation.
	 */
	for (const QString &path : m_headerFiles) {
		appendFile(
			path,
			true,
			false
		);
	}

	for (const QString &path : m_sourceFiles) {
		appendFile(
			path,
			true,
			false
		);
	}

	for (const QString &path : m_gasFiles) {
		appendFile(
			path,
			true,
			false
		);
	}

	for (const QString &path : m_additionalFiles) {
		appendFile(
			path,
			true,
			false
		);
	}

	if (m_directory.isEmpty()) {
		return result;
	}

	const QDir projectDirectory(m_directory);

	/*
	 * Discover every non-hidden file beneath the project directory.
	 *
	 * QDir::Hidden is deliberately omitted so that dot-files and
	 * dot-directories such as .git and .cache are not included.
	 */
	QDirIterator iterator(
		m_directory,
		QDir::Files |
		QDir::NoDotAndDotDot |
		QDir::NoSymLinks,
		QDirIterator::Subdirectories
	);

	while (iterator.hasNext()) {
		const QString absolutePath =
			iterator.next();

		const QString relativePath =
			QDir::cleanPath(
				projectDirectory.relativeFilePath(
					absolutePath
				)
			);

		/*
		 * The TIGCC-Qt project metadata file is not an
		 * editable project file and should not appear in the tree.
		 */
		if (relativePath.endsWith(
				QStringLiteral(".tigcc-project"),
				Qt::CaseInsensitive
			)) {
			continue;
		}

		appendFile(
			relativePath,
			false,
			true
		);
	}

	return result;
}
