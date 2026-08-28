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




class QAction;
class BuildOutputWidget;
class EditorBackend;
class ProjectTreeWidget;
class QCloseEvent;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

	// Project file types
	enum class FType
	{
		FHEAD = 0,
		FSRC = 1,
		FGAS = 2
	};

protected:
	void
	closeEvent(
		QCloseEvent *event
	) override;

private:
	void createNewProject();

    void openProject();	

	void saveCurrentProject();

	bool confirmEditorChanges();

	bool prepareForProjectChange();

    void updateProjectInterface();

	void createHeaderFile();

	void createSourceFile();

	void createGasFile();

	void createProjectFile(
		const QString &subdirectory,
		const QString &extension,
		FType ftype
	);

	void
	openProjectFile(
		const QString &relativePath
	);
	
	void
	saveCurrentFile();

	void
	updateEditorInterface();

	void
	adjustEditorFontSize(
		int adjustment
	);

	Project currentProject;

	QString currentProjectFile;

	ProjectTreeWidget *projectTree;

	EditorBackend *editor;

	BuildOutputWidget *buildOutput;

	QAction *saveFileAction;
};

#endif // TIGCC_QT_MAINWINDOW_H
