#include <QTreeWidgetItem>

#include "projecttreewidget.h"


ProjectTreeWidget::ProjectTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setHeaderLabel(QStringLiteral("Project"));
}


void
ProjectTreeWidget::setProject(const Project &project)
{
    clear();

    auto *sourcesItem = new QTreeWidgetItem(this);
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
    }

    auto *headersItem = new QTreeWidgetItem(this);
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
    }

    sourcesItem->setExpanded(true);
    headersItem->setExpanded(true);
}
