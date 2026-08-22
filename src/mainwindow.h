#ifndef TIGCC_QT_MAINWINDOW_H
#define TIGCC_QT_MAINWINDOW_H

#include <QMainWindow>

#include "project.h"

class BuildOutputWidget;
class EditorWidget;
class ProjectTreeWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
	void
	createNewProject();

	Project currentProject;

	ProjectTreeWidget *projectTree;
	EditorWidget *editor;
	BuildOutputWidget *buildOutput;
};

#endif // TIGCC_QT_MAINWINDOW_H
