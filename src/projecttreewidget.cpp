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

#include <QTreeWidgetItem>

#include "projecttreewidget.h"



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
}


void
ProjectTreeWidget::setProject(const Project &project)
{
	clear();

	auto *projectItem = new QTreeWidgetItem(this);

	projectItem->setText(
		0,
		project.name().isEmpty()
			? QStringLiteral("Untitled")
			: project.name()
	);

	auto *sourcesItem = new QTreeWidgetItem(
		projectItem
	);

	sourcesItem->setText(
		0,
		QStringLiteral("Source Files")
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

	auto *headersItem = new QTreeWidgetItem(
		projectItem
	);

	headersItem->setText(
		0,
		QStringLiteral("Header Files")
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

	projectItem->setExpanded(true);
	sourcesItem->setExpanded(true);
	headersItem->setExpanded(true);
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
