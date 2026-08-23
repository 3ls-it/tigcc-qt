/*
 * @file    src/buildoutputwidget.h
 * @brief   Header file for buildoutputwidget.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_BUILDOUTPUTWIDGET_H
#define TIGCC_QT_BUILDOUTPUTWIDGET_H

#include <QPlainTextEdit>

class BuildOutputWidget : public QPlainTextEdit
{
public:
    explicit BuildOutputWidget(QWidget *parent = nullptr);

    void appendOutput(const QString &text);
};

#endif // TIGCC_QT_BUILDOUTPUTWIDGET_H
