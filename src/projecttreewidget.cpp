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

#include <QHash>
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

	// Category icon colour
	const QColor categoryColor =
		QColor(
			QStringLiteral("#b48ead")
		);

	const UiIconColors iconColors =
		defaultUiIconColors();

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
		QHash<QString, QTreeWidgetItem *> directories;
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
				QStringLiteral("project-development"),
				QStyle::SP_DirHomeIcon,
				categoryColor
			)
			);

		categoryItem->setExpanded(true);

		branches.append(
			CategoryBranch{
				category,
				categoryItem,
				{}
			}
		);
	}

	const QList<ProjectFile> files =
		project.visibleFiles();

	for (const ProjectFile &projectFile : files) {
		CategoryBranch *branch =
			nullptr;

		for (CategoryBranch &candidate : branches) {
			if (candidate.category ==
				projectFile.category) {
				branch = &candidate;
				break;
			}
		}

		if (branch == nullptr) {
			continue;
		}

		const QStringList components =
			projectFile.relativePath.split(
				'/',
				Qt::SkipEmptyParts
			);

		if (components.isEmpty()) {
			continue;
		}

		QTreeWidgetItem *parentItem =
			branch->item;

		QString directoryPath;

		for (int index = 0;
			index < components.size() - 1;
			++index) {
			const QString &directoryName =
				components.at(index);

			if (!directoryPath.isEmpty()) {
				directoryPath +=
					QStringLiteral("/");
			}

			directoryPath +=
				directoryName;

			QTreeWidgetItem *directoryItem =
				branch->directories.value(
					directoryPath,
					nullptr
				);

			if (directoryItem == nullptr) {
				directoryItem =
					new QTreeWidgetItem(parentItem);

				directoryItem->setText(
					0,
					directoryName
				);

				directoryItem->setIcon(
					0,
					visibleTreeIcon(
						this,
						QStringLiteral("folder"),
						QStyle::SP_DirIcon,
						iconColors.folder
					)
				);

				directoryItem->setExpanded(true);

				branch->directories.insert(
					directoryPath,
					directoryItem
				);
			}

			parentItem = directoryItem;
		}

		auto *fileItem =
			new QTreeWidgetItem(parentItem);

		fileItem->setText(
			0,
			components.last()
		);

		fileItem->setIcon(
			0,
			visibleTreeIcon(
				this,
				QStringLiteral("text-x-generic"),
				QStyle::SP_FileIcon,
				iconColors.file
			)
		);

		fileItem->setData(
			0,
			Qt::UserRole,
			projectFile.relativePath
		);

		fileItem->setToolTip(
			0,
			projectFile.relativePath
		);

		if (!isEditableProjectFileCategory(
				projectFile.category
			)) {
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

   const ProjectFileCategory category =
       classifyProjectFile(relativePath);

   if (!isEditableProjectFileCategory(category)) {
       return;
   }

	emit fileActivated(
		relativePath
	);
}
