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

#include <QStyle>
#include <QTreeWidgetItem>

#include "projectfilecategory.h"
#include "projecttreewidget.h"
#include "uiiconutils.h"



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
ProjectTreeWidget::setProject(
        const Project &project
)
{
        clear();

        const UiIconColors iconColors =
                defaultUiIconColors();

        // Root of tree
        auto *projectItem =
                new QTreeWidgetItem(this);

        projectItem->setIcon(
                0,
                visibleTreeIcon(
                        this,
                        QStringLiteral("project-development"),
                        QStyle::SP_DirHomeIcon,
                        iconColors.project
                )
        );

        projectItem->setText(
                0,
                project.name().isEmpty()
                        ? QStringLiteral("Untitled")
                        : project.name()
        );

        struct CategoryBranch
        {
                ProjectFileCategory category;
                QTreeWidgetItem *item;
        };

        const QList<ProjectFileCategory> categories = {
                ProjectFileCategory::Header,
                ProjectFileCategory::CSource,
                ProjectFileCategory::GnuAssembly,
                ProjectFileCategory::A68kAssembly,
                ProjectFileCategory::Text,
                ProjectFileCategory::Other,
                ProjectFileCategory::Output
        };

        QList<CategoryBranch> branches;

        for (const ProjectFileCategory category : categories) {
                auto *categoryItem =
                        new QTreeWidgetItem(projectItem);

                categoryItem->setText(
                        0,
                        projectFileCategoryName(category)
                );

                categoryItem->setIcon(
                        0,
                        visibleTreeIcon(
                                this,
                                QStringLiteral("folder"),
                                QStyle::SP_DirIcon,
                                iconColors.folder
                        )
                );

                categoryItem->setExpanded(true);

                branches.append(
                        CategoryBranch{
                                category,
                                categoryItem
                        }
                );
        }

        const QList<ProjectFile> files =
                project.visibleFiles();

        for (const ProjectFile &projectFile : files) {
                QTreeWidgetItem *categoryItem = nullptr;
                for (const CategoryBranch &branch : branches) {
                        if (branch.category ==
                                projectFile.category) {
                                categoryItem = branch.item;
                                break;
                        }
                }
                if (categoryItem == nullptr) {
                        continue;
                }
                auto *fileItem =
                        new QTreeWidgetItem(categoryItem);
                fileItem->setText(
                        0,
                        projectFile.relativePath
                );
                fileItem->setData(
                        0,
                        Qt::UserRole,
                        projectFile.relativePath
                );
                if (!isEditableProjectFileCategory(
                        projectFile.category)) {
                        fileItem->setFlags(
                                fileItem->flags() &
                                ~Qt::ItemIsSelectable
                        );
                }
        }

        projectItem->setExpanded(true);
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
