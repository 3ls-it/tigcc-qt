/*
 * @file    src/editorwidget.h
 * @brief   Header file for editorwidget.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_EDITORWIDGET_H
#define TIGCC_QT_EDITORWIDGET_H

#include <QPlainTextEdit>



class EditorWidget : public QPlainTextEdit
{
public:
    explicit EditorWidget(QWidget *parent = nullptr);
};

#endif // TIGCC_QT_EDITORWIDGET_H
