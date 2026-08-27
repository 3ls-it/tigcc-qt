/*
 * @file    src/projecttreewidget.h
 * @brief   Header file for projecttreewidget.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_PROJECTTREEWIDGET_H
#define TIGCC_QT_PROJECTTREEWIDGET_H

#include <QTreeWidget>

#include "project.h"



class ProjectTreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	explicit ProjectTreeWidget(QWidget *parent = nullptr);
	void setProject(const Project &project);

signals:
	void fileActivated(
		const QString &relativePath
	);

private slots:
	void handleItemDoubleClicked(
		QTreeWidgetItem *item,
		int column
	);
};

#endif // TIGCC_QT_PROJECTTREEWIDGET_H
