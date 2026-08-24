/*
 * @file    src/appearance.cpp
 * @brief   Sets the application theme.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "appearance.h"

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QPalette>
#include <QStyleFactory>



namespace Appearance
{

void
applyDarkTheme()
{
    qDebug() << "Applying TIGCC-Qt dark theme";

    qDebug() << "Available styles:"
             << QStyleFactory::keys();

    qApp->setStyle(
        QStyleFactory::create(
            QStringLiteral("Fusion")
        )
    );

    QPalette palette =
        qApp->palette();

    palette.setColor(
        QPalette::Window,
        QColor(53, 53, 53)
    );

    palette.setColor(
        QPalette::WindowText,
        Qt::white
    );

    palette.setColor(
        QPalette::Base,
        QColor(35, 35, 35)
    );

    palette.setColor(
        QPalette::AlternateBase,
        QColor(53, 53, 53)
    );

    palette.setColor(
        QPalette::ToolTipBase,
        Qt::white
    );

    palette.setColor(
        QPalette::ToolTipText,
        Qt::white
    );

    palette.setColor(
        QPalette::Text,
        Qt::white
    );

    palette.setColor(
        QPalette::Button,
        QColor(53, 53, 53)
    );

    palette.setColor(
        QPalette::ButtonText,
        Qt::white
    );

    palette.setColor(
        QPalette::BrightText,
        Qt::red
    );

    palette.setColor(
        QPalette::Link,
        QColor(42, 130, 218)
    );

    palette.setColor(
        QPalette::Highlight,
        QColor(42, 130, 218)
    );

    palette.setColor(
        QPalette::HighlightedText,
        Qt::black
    );

    palette.setColor(
        QPalette::PlaceholderText,
        QColor(180, 180, 180)
    );

    qApp->setPalette(
        palette
    );

    qDebug() << "Window color:"
             << qApp->palette().color(
                    QPalette::Window
                );

    qDebug() << "Base color:"
             << qApp->palette().color(
                    QPalette::Base
                );
}

}
