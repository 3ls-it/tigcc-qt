/*
 * @file    src/projectfile.h
 * @brief   Describes a file visible in a TIGCC-Qt project.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_PROJECTFILE_H
#define TIGCC_QT_PROJECTFILE_H

#include <QString>

#include "projectfilecategory.h"



struct ProjectFile
{
	QString relativePath;
	ProjectFileCategory category;
	bool explicitlyListed;
	bool automaticallyDiscovered;
};


#endif // TIGCC_QT_PROJECTFILE_H
