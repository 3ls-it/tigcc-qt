/*
 * @file    src/qscintillatheme.cpp
 * @brief   Defines QScintilla theme colours and factory method.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QString>

#include "qscintillatheme.h"



QScintillaTheme::QScintillaTheme()
	: editorBackground(
		QStringLiteral("#121212")
	),
	  editorForeground(
		QStringLiteral("#b2b2b2")
	),
	  marginBackground(
		QStringLiteral("#31363b")
	),
	  marginForeground(
		QStringLiteral("#767676")
	),
	  caretForeground(
		QStringLiteral("#b2b2b2")
	),
	  comment(
		QStringLiteral("#505050")
	),
	  number(
		QStringLiteral("#9775da")
	),
	  keyword(
		QStringLiteral("#5bcdf3")
	),
	  string(
		QStringLiteral("#f5d676")
	),
	  character(
		QStringLiteral("#f5d676")
	),
	  operatorColor(
		QStringLiteral("#f8f8f8")
	),
	  identifier(
		QStringLiteral("#b2b2b2")
	),
	  preprocessor(
		QStringLiteral("#c22323")
	)
{
}


QScintillaTheme
QScintillaTheme::tigccDark()
{
	return QScintillaTheme();
}
