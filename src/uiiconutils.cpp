/*
 * @file    src/uiiconutils.cpp
 * @brief   Implements shared application icon utilities.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QPainter>
#include <QPixmap>
#include <QWidget>

#include "uiiconutils.h"



QIcon
themeOrStandardIcon(
	QWidget *widget,
	const QString &themeName,
	QStyle::StandardPixmap fallback
)
{
	QIcon icon =
		QIcon::fromTheme(
			themeName
		);

	if (icon.isNull() &&
		widget != nullptr) {
		icon =
			widget->style()->standardIcon(
				fallback
			);
	}

	return icon;
}


QIcon
tintedIcon(
	const QIcon &source,
	const QColor &color,
	const QSize &size
)
{
	if (source.isNull()) {
		return QIcon();
	}

	const QPixmap sourcePixmap =
		source.pixmap(
			size
		);

	if (sourcePixmap.isNull()) {
		return QIcon();
	}

	QPixmap tintedPixmap(
		sourcePixmap.size()
	);

	tintedPixmap.fill(
		Qt::transparent
	);

	QPainter painter(
		&tintedPixmap
	);

	painter.drawPixmap(
		0,
		0,
		sourcePixmap
	);

	painter.setCompositionMode(
		QPainter::CompositionMode_SourceIn
	);

	painter.fillRect(
		tintedPixmap.rect(),
		color
	);

	painter.end();

	return QIcon(
		tintedPixmap
	);
}


QIcon
visibleTreeIcon(
	QWidget *widget,
	const QString &themeName,
	QStyle::StandardPixmap fallback,
	const QColor &color
)
{
	const QIcon sourceIcon =
		themeOrStandardIcon(
			widget,
			themeName,
			fallback
		);

	return tintedIcon(
		sourceIcon,
		color,
		QSize(
			22,
			22
		)
	);
}
