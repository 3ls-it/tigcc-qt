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

#include "completiondatabackend.h"
#include "configuration.h"
#include "editorbackendfactory.h"
#include "project.h"



class BuildOutputWidget;
class EditorBackend;
class ProjectTreeWidget;
class QAction;
class QCloseEvent;
class QSplitter;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(
			Configuration &configuration,
			QWidget *parent = nullptr
	);

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
	void loadCompletionData();

	CompletionDataBackend * completionDataBackend();

	void saveConfiguration();

	void updateEditorBackendActions();

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

	void connectEditorBackend();

	void
	switchEditorBackend(
		EditorBackendType type
	);

	void
	openProjectFile(
		const QString &relativePath
	);
	
	void saveCurrentFile();

	void saveAllFiles();

	void updateEditorInterface();

	void
	adjustEditorFontSize(
		int adjustment
	);

	QSplitter *rightSplitter;
	EditorBackendType editorBackendType;
	QAction *qscintillaBackendAction;
	QAction *ktextEditorBackendAction;
	Project currentProject;
	QString currentProjectFile;
	Configuration *m_configuration;
	CompletionDataBackend m_completionData;
	ProjectTreeWidget *projectTree;
	EditorBackend *editor;
	BuildOutputWidget *buildOutput;
	QAction *vimBackendAction;
	QAction *saveFileAction;
};

#endif // TIGCC_QT_MAINWINDOW_H
