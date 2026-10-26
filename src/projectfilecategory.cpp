/*
 * @file    src/projectfilecategory.cpp
 * @brief   Project file category definitions and classification helpers.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QFileInfo>
#include <QString>

#include "projectfilecategory.h"



ProjectFileCategory
classifyProjectFile(
	const QString &relativePath
)
{
	const QFileInfo fileInfo(relativePath);

	const QString fileName =
		fileInfo.fileName();

	const QString lowerFileName =
		fileName.toLower();

	const QString suffix =
		fileInfo.suffix().toLower();

	/*
	 * Build output files are recognized before ordinary extension
	 * classification. These are generated binaries rather than editor
	 * documents.
	 */
	if (suffix == QStringLiteral("89z") ||
		suffix == QStringLiteral("9xz") ||
		suffix == QStringLiteral("v2z"))
	{
		return ProjectFileCategory::Output;
	}

	/*
	 * Header files.
	 */
	if (suffix == QStringLiteral("h") ||
		suffix == QStringLiteral("hh") ||
		suffix == QStringLiteral("hpp"))
	{
		return ProjectFileCategory::Header;
	}

	/*
	 * C source files.
	 */
	if (suffix == QStringLiteral("c"))
	{
		return ProjectFileCategory::CSource;
	}

	/*
	 * GNU assembler files.

	 * The current New GNU Assembly File action creates .asm files,
	 * while .s and .S are also conventional GNU assembler suffixes.
	 */
	if (suffix == QStringLiteral("asm") ||
		suffix == QStringLiteral("s"))
	{
		return ProjectFileCategory::GnuAssembly;
	}

	/*
	 * A68k assembler files.
	 */
	if (suffix == QStringLiteral("a68k"))
	{
		return ProjectFileCategory::A68kAssembly;
	}

	/*
	 * Common text/document and scripting files relevant to projects.
	 */
	if (suffix == QStringLiteral("md") ||
		suffix == QStringLiteral("markdown") ||
		suffix == QStringLiteral("txt") ||
		suffix == QStringLiteral("rst") ||
		suffix == QStringLiteral("py") ||
		suffix == QStringLiteral("rle") ||
		suffix == QStringLiteral("json") ||
		suffix == QStringLiteral("yaml") ||
		suffix == QStringLiteral("yml") ||
		suffix == QStringLiteral("toml") ||
		suffix == QStringLiteral("ini") ||
		suffix == QStringLiteral("sh") ||
		suffix == QStringLiteral("bash"))
	{
		return ProjectFileCategory::Text;
	}

	/*
	 * Common extensionless project-control and documentation files.
	 */
	if (lowerFileName == QStringLiteral("makefile") ||
		lowerFileName == QStringLiteral("gnumakefile") ||
		lowerFileName == QStringLiteral("license") ||
		lowerFileName == QStringLiteral("readme"))
	{
		return ProjectFileCategory::Text;
	}

	return ProjectFileCategory::Other;
}


QString
projectFileCategoryName(
	ProjectFileCategory category
)
{
	switch (category) {
		case ProjectFileCategory::Header:
			return QStringLiteral("Header Files");

		case ProjectFileCategory::CSource:
			return QStringLiteral("Source/C Files");

		case ProjectFileCategory::GnuAssembly:
			return QStringLiteral("GNU Assembly Files");

		case ProjectFileCategory::A68kAssembly:
			return QStringLiteral("A68k Assembly Files");

		case ProjectFileCategory::Text:
			return QStringLiteral("Text Files");

		case ProjectFileCategory::Other:
			return QStringLiteral("Other Files");

		case ProjectFileCategory::Output:
			return QStringLiteral("Output Files");
	}

	return QStringLiteral("Other Files");
}


bool
isEditableProjectFileCategory(
	ProjectFileCategory category
)
{
	switch (category) {
		case ProjectFileCategory::Output:
			return false;

		case ProjectFileCategory::Header:
		case ProjectFileCategory::CSource:
		case ProjectFileCategory::GnuAssembly:
		case ProjectFileCategory::A68kAssembly:
		case ProjectFileCategory::Text:
		case ProjectFileCategory::Other:
			return true;
	}

	return false;
}


bool
isCreatableProjectFileCategory(
	ProjectFileCategory category
)
{
	switch (category) {
		case ProjectFileCategory::Header:
		case ProjectFileCategory::CSource:
		case ProjectFileCategory::GnuAssembly:
			return true;

		case ProjectFileCategory::A68kAssembly:
		case ProjectFileCategory::Text:
		case ProjectFileCategory::Other:
		case ProjectFileCategory::Output:
			return false;
	}

	return false;
}
