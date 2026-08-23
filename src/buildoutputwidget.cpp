/*
 * @file    src/buildoutputwidget.cpp
 * @brief   Provides widget for compiler output.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "buildoutputwidget.h"

BuildOutputWidget::BuildOutputWidget(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);

    setPlaceholderText(
        QStringLiteral("Compiler output")
    );
}

void
BuildOutputWidget::appendOutput(const QString &text)
{
    appendPlainText(text);
}
