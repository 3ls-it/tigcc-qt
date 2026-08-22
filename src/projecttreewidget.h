#ifndef TIGCC_QT_PROJECTTREEWIDGET_H
#define TIGCC_QT_PROJECTTREEWIDGET_H

#include <QTreeWidget>

#include "project.h"

class ProjectTreeWidget : public QTreeWidget
{
public:
    explicit ProjectTreeWidget(QWidget *parent = nullptr);
	void setProject(const Project &project);
};

#endif // TIGCC_QT_PROJECTTREEWIDGET_H
