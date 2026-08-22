#include "projecttreewidget.h"

#include <QTreeWidgetItem>

ProjectTreeWidget::ProjectTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setHeaderLabel(QStringLiteral("Project"));

    auto *sourcesItem = new QTreeWidgetItem(this);
    sourcesItem->setText(
        0,
        QStringLiteral("Source Files")
    );

    auto *headersItem = new QTreeWidgetItem(this);
    headersItem->setText(
        0,
        QStringLiteral("Header Files")
    );
}
