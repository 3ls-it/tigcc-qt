/*
 * @file    src/projecttreewidget.cpp
 * @brief   Provides tree view of opened projects.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QApplication>
#include <QIcon>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QSize>
#include <QStyle>
#include <QTreeWidgetItem>

#include "projecttreewidget.h"



namespace
{

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

	if (icon.isNull()) {
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
}



ProjectTreeWidget::ProjectTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setHeaderLabel(QStringLiteral("Project"));
	connect(
		this,
		&QTreeWidget::itemDoubleClicked,
		this,
		&ProjectTreeWidget::handleItemDoubleClicked
    );

	setIconSize(
		QSize(
			22,
			22
		)
	);

}


void
ProjectTreeWidget::setProject(const Project &project)
{
	const QColor folderColor(
		QStringLiteral("#f5d676")
	);

	const QColor projectColor(
		QStringLiteral("#81a1c1")
	);

	clear();

	// Root of tree
	auto *projectItem = new QTreeWidgetItem(this);

	projectItem->setIcon(
		0,
		visibleTreeIcon(
			this,
			QStringLiteral("project-development"),
			QStyle::SP_DirHomeIcon,
			projectColor
		)
	);

	projectItem->setText(
		0,
		project.name().isEmpty()
			? QStringLiteral("Untitled")
			: project.name()
	);

	// Header files branch
	auto *headersItem = new QTreeWidgetItem(
		projectItem
	);

	headersItem->setText(
		0,
		QStringLiteral("Header Files")
	);

	headersItem->setIcon(
		0,
		visibleTreeIcon(
			this,
			QStringLiteral("folder"),
			QStyle::SP_DirIcon,
			folderColor
		)
	);

	for (const QString &path : project.headerFiles()) {
		auto *fileItem = new QTreeWidgetItem(
			headersItem
		);

		fileItem->setText(
			0,
			path
		);

		fileItem->setData(
			0,
			Qt::UserRole,
			path
		);	
	}

	// Source files branch
	auto *sourcesItem = new QTreeWidgetItem(
		projectItem
	);

	sourcesItem->setText(
		0,
		QStringLiteral("Source Files")
	);

	sourcesItem->setIcon(
		0,
		visibleTreeIcon(
			this,
			QStringLiteral("folder"),
			QStyle::SP_DirIcon,
			folderColor
		)
	);

	for (const QString &path : project.sourceFiles()) {
		auto *fileItem = new QTreeWidgetItem(
			sourcesItem
		);

		fileItem->setText(
			0,
			path
		);

		fileItem->setData(
			0,
			Qt::UserRole,
			path
		);
	}

	// GNU Assembly files branch
	auto *gasItem = new QTreeWidgetItem(
		projectItem
	);

	gasItem->setText(
		0,
		QStringLiteral("GNU Assembly Files")
	);

	gasItem->setIcon(
		0,
		visibleTreeIcon(
			this,
			QStringLiteral("folder"),
			QStyle::SP_DirIcon,
			folderColor
		)
	);

	for (const QString &path : project.gasFiles()) {
		auto *fileItem = new QTreeWidgetItem(
			gasItem
		);

		fileItem->setText(
			0,
			path
		);

		fileItem->setData(
			0,
			Qt::UserRole,
			path
		);
	}

	projectItem->setExpanded(true);
	headersItem->setExpanded(true);
	sourcesItem->setExpanded(true);
	gasItem->setExpanded(true);
}


void
ProjectTreeWidget::handleItemDoubleClicked(
	QTreeWidgetItem *item,
	int column
)
{
	Q_UNUSED(column);

	if (item == nullptr) {
		return;
	}

	const QVariant fileData =
		item->data(
			0,
			Qt::UserRole
		);

	if (!fileData.isValid()) {
		return;
	}

	const QString relativePath =
		fileData.toString();

	if (relativePath.isEmpty()) {
		return;
	}

	emit fileActivated(
		relativePath
	);
}
