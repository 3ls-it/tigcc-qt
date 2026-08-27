/*
 * @file    src/projectmanager.h
 * @brief   Header file for projectmanager.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_PROJECTMANAGER_H
#define TIGCC_QT_PROJECTMANAGER_H

#include <QString>

#include "project.h"



class ProjectManager
{
public:
	bool
	saveProject(
		const Project &project,
		const QString &filePath,
		QString *errorMessage = nullptr
	) const;

	bool
	loadProject(
		const QString &filePath,
		Project *project,
		QString *errorMessage = nullptr
	) const;
};

#endif // TIGCC_QT_PROJECTMANAGER_H
