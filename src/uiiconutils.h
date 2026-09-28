/*
 * @file    src/uiiconutils.h
 * @brief   Shared application icon utilities.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_UIICONUTILS_H
#define TIGCC_QT_UIICONUTILS_H

#include <QColor>
#include <QIcon>
#include <QSize>
#include <QString>
#include <QStyle>



class QWidget;

QIcon
themeOrStandardIcon(
	QWidget *widget,
	const QString &themeName,
	QStyle::StandardPixmap fallback
);

QIcon
tintedIcon(
	const QIcon &source,
	const QColor &color,
	const QSize &size
);

QIcon
visibleTreeIcon(
	QWidget *widget,
	const QString &themeName,
	QStyle::StandardPixmap fallback,
	const QColor &color
);

#endif // TIGCC_QT_UIICONUTILS_H
