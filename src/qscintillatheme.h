/*
 * @file    src/qscintillatheme.h
 * @brief   Header file for src/qscintillatheme.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_QSCINTILLATHEME_H
#define TIGCC_QT_QSCINTILLATHEME_H

#include <QColor>



class QScintillaTheme
{
public:
	QScintillaTheme();

	static QScintillaTheme
	tigccDark();

	QColor editorBackground;
	QColor editorForeground;

	QColor marginBackground;
	QColor marginForeground;
	QColor caretForeground;

	QColor comment;
	QColor number;
	QColor keyword;
	QColor string;
	QColor character;
	QColor operatorColor;
	QColor identifier;
	QColor preprocessor;

};

#endif // TIGCC_QT_QSCINTILLATHEME_H
