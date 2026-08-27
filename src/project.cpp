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

#include "project.h"



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
void

// Source files add/remove
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
