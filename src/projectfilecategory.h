/*
 * @file    src/projectfilecategory.h
 * @brief   Project file category definitions and classification helpers.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_PROJECTFILECATEGORY_H
#define TIGCC_QT_PROJECTFILECATEGORY_H

#include <QString>



enum class ProjectFileCategory
{
	Header,
	CSource,
	GnuAssembly,
	A68kAssembly,
	Text,
	Other,
	Output
};


ProjectFileCategory
classifyProjectFile(
	const QString &relativePath
);


QString
projectFileCategoryName(
	ProjectFileCategory category
);


bool
isEditableProjectFileCategory(
	ProjectFileCategory category
);


bool
isCreatableProjectFileCategory(
	ProjectFileCategory category
);

#endif // TIGCC_QT_PROJECTFILECATEGORY_H
