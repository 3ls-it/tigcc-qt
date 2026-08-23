/*
 * @file    src/editorwidget.cpp
 * @brief   Provides the editor base widget.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "editorwidget.h"



EditorWidget::EditorWidget(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setPlaceholderText(
        QStringLiteral("Source editor")
    );
}
