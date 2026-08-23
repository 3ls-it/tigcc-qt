/*
 * @file    src/mainwindow.h
 * @brief   Header file for mainwindow.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_MAINWINDOW_H
#define TIGCC_QT_MAINWINDOW_H

#include <QMainWindow>
#include <QString>

#include "project.h"




class BuildOutputWidget;
class EditorBackend;
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
	EditorBackend *editor;
	BuildOutputWidget *buildOutput;
};

#endif // TIGCC_QT_MAINWINDOW_H
