#ifndef TIGCC_QT_MAINWINDOW_H
#define TIGCC_QT_MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "project.h"




class BuildOutputWidget;
class EditorWidget;
class ProjectTreeWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
	void createNewProject();

    void openProject();	

	void saveCurrentProject();

    void updateProjectInterface();

	void createSourceFile();

	void createHeaderFile();

	void createProjectFile(
		const QString &subdirectory,
		const QString &extension,
		bool headerFile
	);
	
	Project currentProject;
	QString currentProjectFile;

	ProjectTreeWidget *projectTree;
	EditorWidget *editor;
	BuildOutputWidget *buildOutput;
};

#endif // TIGCC_QT_MAINWINDOW_H
