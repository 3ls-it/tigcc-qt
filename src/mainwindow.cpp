/*
 * @file    src/mainwindow.cpp
 * @brief   Provides the application main window.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QActionGroup>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QAction>
#include <QFileInfo>
#include <QKeySequence>
#include <QStringList>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QCloseEvent>

#include "config.h"
#include "mainwindow.h"
#include "buildoutputwidget.h"
#include "editorbackend.h"
#include "editorbackendfactory.h"
#include "projectmanager.h"
#include "projecttreewidget.h"



MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  currentProject(
		  QStringLiteral("Untitled"),
		  QString()
	  ),
	  projectTree(new ProjectTreeWidget(this)),
	  editor(
		  createEditorBackend(
			  EditorBackendType::QScintilla,
			  this
		  )
	  ),
	  buildOutput(new BuildOutputWidget(this)),
	  rightSplitter(nullptr),
	  editorBackendType(
		  EditorBackendType::QScintilla
	  ),
	  qscintillaBackendAction(nullptr),
#ifdef USE_KTEXTEDITOR
	  ktextEditorBackendAction(nullptr),
#endif
	  saveFileAction(nullptr)
{
    setWindowTitle(
		QStringLiteral("TIGCC-Qt")
    );

    resize(1200, 829);

	updateProjectInterface();

	rightSplitter = new QSplitter(
		Qt::Vertical,
		this
	);

	rightSplitter->addWidget(
		editor->widget()
	);
    rightSplitter->addWidget(buildOutput);

    rightSplitter->setStretchFactor(0, 4);
    rightSplitter->setStretchFactor(1, 1);
	rightSplitter->setSizes({634, 195});

	editor->widget()->setMinimumHeight(160);
	buildOutput->setMinimumHeight(80);

    auto *mainSplitter = new QSplitter(
		Qt::Horizontal,
		this
    );

    mainSplitter->addWidget(projectTree);
    mainSplitter->addWidget(rightSplitter);

    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 4);

    setCentralWidget(mainSplitter);

	// File menu
    auto *fileMenu = menuBar()->addMenu(
		QStringLiteral("&File")
    );

	// File>Save
	saveFileAction = fileMenu->addAction(
		QStringLiteral("&Save")
	);

	saveFileAction->setShortcut(
		QKeySequence::Save
	);

	saveFileAction->setEnabled(
		false
	);

	connect(
		saveFileAction,
		&QAction::triggered,
		this,
		&MainWindow::saveCurrentFile
	);

	// File>Quit
    auto *quitAction = fileMenu->addAction(
		QStringLiteral("&Quit")
    );

    connect(
		quitAction,
		&QAction::triggered,
		this,
		&QMainWindow::close
    );

	// Project menus
    auto *projectMenu = menuBar()->addMenu(
		QStringLiteral("&Project")
    );

	auto *newProjectAction = projectMenu->addAction(
		QStringLiteral("&New Project")
	);

	connect(
		newProjectAction,
		&QAction::triggered,
		this,
		&MainWindow::createNewProject
	);

	auto *openProjectAction = projectMenu->addAction(
		QStringLiteral("&Open Project")
	);

	connect(
		openProjectAction,
		&QAction::triggered,
		this,
		&MainWindow::openProject
	);

    projectMenu->addAction(
		QStringLiteral("Build Project")
    );

	auto *saveProjectAction = projectMenu->addAction(
		QStringLiteral("&Save Project")
	);

	connect(
		saveProjectAction,
		&QAction::triggered,
		this,
		&MainWindow::saveCurrentProject
	);

	projectMenu->addSeparator();

	// Project>New header files
	auto *newHeaderFileAction = projectMenu->addAction(
		QStringLiteral("New Header File")
	);

	connect(
		newHeaderFileAction,
		&QAction::triggered,
		this,
		&MainWindow::createHeaderFile
	);//End

	// Project>New source files
	auto *newSourceFileAction = projectMenu->addAction(
		QStringLiteral("New Source File")
	);

	connect(
		newSourceFileAction,
		&QAction::triggered,
		this,
		&MainWindow::createSourceFile
	);//End

	// Project>New GAS files
	auto *newGasFileAction = projectMenu->addAction(
		QStringLiteral("New GNU Assembly File")
	);

	connect(
		newGasFileAction,
		&QAction::triggered,
		this,
		&MainWindow::createGasFile
	);//End

	connect(
		projectTree,
		&ProjectTreeWidget::fileActivated,
		this,
		&MainWindow::openProjectFile
	);

	// View menu
		auto *viewMenu = menuBar()->addMenu(
		QStringLiteral("&View")
	);

	auto *editorBackendMenu =
		viewMenu->addMenu(
			QStringLiteral("Editor Backend")
		);

	auto *editorBackendActionGroup =
		new QActionGroup(
			editorBackendMenu
		);

	editorBackendActionGroup->setExclusive(
		true
	);

	// QScintilla
	qscintillaBackendAction =
		editorBackendMenu->addAction(
			QStringLiteral("QScintilla")
		);

	qscintillaBackendAction->setCheckable(
		true
	);

	editorBackendActionGroup->addAction(
		qscintillaBackendAction
	);

	// Vim
	vimBackendAction =
		editorBackendMenu->addAction(
			QStringLiteral("Vim")
		);

	vimBackendAction->setCheckable(
		true
	);

	editorBackendActionGroup->addAction(
		vimBackendAction
	);

#ifdef USE_KTEXTEDITOR
  // Kate
	ktextEditorBackendAction =
		editorBackendMenu->addAction(
			QStringLiteral("KTextEditor")
		);

	ktextEditorBackendAction->setCheckable(
		true
	);

	editorBackendActionGroup->addAction(
		ktextEditorBackendAction
	);
#endif

	qscintillaBackendAction->setChecked(
		editorBackendType ==
		EditorBackendType::QScintilla
	);

#ifdef USE_KTEXTEDITOR
	ktextEditorBackendAction->setChecked(
		editorBackendType ==
		EditorBackendType::KTextEditor
	);
#endif

	// Conect the backends
	connect(
		qscintillaBackendAction,
		&QAction::triggered,
		this,
		[this]() {
			switchEditorBackend(
				EditorBackendType::QScintilla
			);
		}
	);

	connect(
		vimBackendAction,
		&QAction::triggered,
		this,
		[this]() {
			switchEditorBackend(
				EditorBackendType::Vim
			);
		}
	);

#ifdef USE_KTEXTEDITOR
	connect(
		ktextEditorBackendAction,
		&QAction::triggered,
		this,
		[this]() {
			switchEditorBackend(
				EditorBackendType::KTextEditor
			);
		}
	);
#endif

	auto *editorFontSizeMenu =
		viewMenu->addMenu(
			QStringLiteral("Editor Font Size")
		);

	auto *increaseFontSizeAction =
		editorFontSizeMenu->addAction(
			QStringLiteral("Increase")
		);

	increaseFontSizeAction->setShortcut(
		QKeySequence(
			Qt::CTRL | Qt::Key_Equal
		)
	);

	connect(
		increaseFontSizeAction,
		&QAction::triggered,
		this,
		[this]() {
			adjustEditorFontSize(
				1
			);
		}
	);

	auto *decreaseFontSizeAction =
		editorFontSizeMenu->addAction(
			QStringLiteral("Decrease")
		);

	decreaseFontSizeAction->setShortcut(
		QKeySequence::ZoomOut
	);

	connect(
		decreaseFontSizeAction,
		&QAction::triggered,
		this,
		[this]() {
			adjustEditorFontSize(
				-1
			);
		}
	);

	// Help menu
    auto *helpMenu = menuBar()->addMenu(
		QStringLiteral("&Help")
    );

	// About
    helpMenu->addAction(
		QStringLiteral("About TIGCC-Qt")
    );

    statusBar()->showMessage(
		QStringLiteral("Ready")
    );

	connectEditorBackend();
} // End constructor
 

void
MainWindow::connectEditorBackend()
{
	connect(
		editor,
		&EditorBackend::currentFileChanged,
		this,
		[this](const QString &filePath) {
			Q_UNUSED(filePath);

			updateEditorInterface();
		}
	);

	connect(
		editor,
		&EditorBackend::modificationChanged,
		this,
		[this](bool modified) {
			Q_UNUSED(modified);

			updateEditorInterface();
		}
	);

	connect(
		editor,
		&EditorBackend::fontPointSizeChanged,
		this,
		[this](int pointSize) {
			statusBar()->showMessage(
				QStringLiteral(
					"Editor font size: %1 pt"
				).arg(
					pointSize
				)
			);
		}
	);
} // End connectEditorBackend


void
MainWindow::switchEditorBackend(
	EditorBackendType type
)
{
	if (type == editorBackendType) {
		return;
	}

	if (!prepareForProjectChange()) {
		return;
	}

	const int currentFontPointSize =
		editor->fontPointSize();

	EditorBackend *newEditor =
		createEditorBackend(
			type,
			this
		);

	if (newEditor == nullptr) {
		QMessageBox::critical(
			this,
			QStringLiteral(
				"Cannot Create Editor Backend"
			),
			QStringLiteral(
				"The requested editor backend "
				"could not be created."
			)
		);

		return;
	}

	QWidget *oldEditorWidget =
		editor->widget();

	QWidget *newEditorWidget =
		newEditor->widget();

	if (newEditorWidget == nullptr) {
		newEditor->deleteLater();

		QMessageBox::critical(
			this,
			QStringLiteral(
				"Cannot Create Editor Widget"
			),
			QStringLiteral(
				"The requested editor backend did "
				"not provide an editor widget."
			)
		);

		return;
	}

	rightSplitter->replaceWidget(
		0,
		newEditorWidget
	);

	oldEditorWidget->deleteLater();
	editor->deleteLater();

	editor =
		newEditor;

	editorBackendType =
		type;

	updateEditorBackendActions();
	connectEditorBackend();

	if (editor->fontPointSize() !=
		currentFontPointSize) {
		QString errorMessage;

		editor->setFontPointSize(
			currentFontPointSize,
			&errorMessage
		);
	}

	updateEditorInterface();

	statusBar()->showMessage(
		QStringLiteral(
			"Editor backend changed"
		)
	);
} // End switchEditorBackend


void
MainWindow::updateEditorBackendActions()
{
	if (qscintillaBackendAction != nullptr) {
		qscintillaBackendAction->setChecked(
			editorBackendType ==
			EditorBackendType::QScintilla
		);
	}

	if (vimBackendAction != nullptr) {
		vimBackendAction->setChecked(
			editorBackendType ==
			EditorBackendType::Vim
		);
	}

#ifdef USE_KTEXTEDITOR
	if (ktextEditorBackendAction != nullptr) {
		ktextEditorBackendAction->setChecked(
			editorBackendType ==
			EditorBackendType::KTextEditor
		);
	}
#endif
} // End updateEditorBackendActions


void
MainWindow::createNewProject()
{
	const QString parentDirectory =
		QFileDialog::getExistingDirectory(
			this,
			QStringLiteral("Select Project Location")
		);

	if (parentDirectory.isEmpty()) {
		return;
	}

	bool accepted = false;

	const QString projectName =
		QInputDialog::getText(
			this,
			QStringLiteral("New Project"),
			QStringLiteral("Project name:"),
			QLineEdit::Normal,
			QString(),
			&accepted
		);

	if (!accepted) {
		return;
	}

	const QString trimmedName =
		projectName.trimmed();

	if (trimmedName.isEmpty()) {
		QMessageBox::warning(
			this,
			QStringLiteral("Invalid Project Name"),
			QStringLiteral(
				"The project name cannot be empty."
			)
		);

		return;
	}

	const QDir parentDirectoryObject(
		parentDirectory
	);

	const QString projectDirectory =
		parentDirectoryObject.filePath(
			trimmedName
		);

	QDir directory;

	if (!directory.mkpath(projectDirectory)) {
		QMessageBox::critical(
			this,
			QStringLiteral("Cannot Create Project"),
			QStringLiteral(
				"The project directory could not be created."
			)
		);

		return;
	}

	const Project newProject(
		trimmedName,
		projectDirectory
	);

	const QString newProjectFile =
		QDir(projectDirectory).filePath(
			QStringLiteral("%1.tigcc-project")
				.arg(trimmedName)
		);

	ProjectManager projectManager;
	QString errorMessage;

	if (!projectManager.saveProject(
			newProject,
			newProjectFile,
			&errorMessage
		)) {
		QMessageBox::critical(
			this,
			QStringLiteral("Cannot Save Project"),
			errorMessage
		);

		return;
	}

	if (!prepareForProjectChange()) {
		return;
	}

	currentProject =
		newProject;

	currentProjectFile =
		newProjectFile;

	updateProjectInterface();

	setWindowTitle(
		QStringLiteral("%1 - TIGCC-Qt")
			.arg(currentProject.name())
	);

	statusBar()->showMessage(
		QStringLiteral("Created project “%1”")
			.arg(currentProject.name())
	);
} // End createNewProject


void
MainWindow::saveCurrentProject()
{
	if (currentProjectFile.isEmpty()) {
		statusBar()->showMessage(
			QStringLiteral("No project file is associated")
		);

		return;
	}

	ProjectManager projectManager;
	QString errorMessage;

	if (!projectManager.saveProject(
			currentProject,
			currentProjectFile,
			&errorMessage
		)) {
		QMessageBox::critical(
			this,
			QStringLiteral("Cannot Save Project"),
			errorMessage
		);

		return;
	}

	statusBar()->showMessage(
		QStringLiteral("Project saved")
	);
}


void
MainWindow::updateProjectInterface()
{
	projectTree->setProject(
		currentProject
	);

	if (currentProject.name().isEmpty()) {
		setWindowTitle(
			QStringLiteral("TIGCC-Qt")
		);
	} else {
		setWindowTitle(
			QStringLiteral("%1 - TIGCC-Qt")
				.arg(currentProject.name())
		);
	}
}


void
MainWindow::openProject()
{
	const QString filePath =
		QFileDialog::getOpenFileName(
			this,
			QStringLiteral("Open Project"),
			QString(),
			QStringLiteral(
				"TIGCC-Qt Projects "
				"(*.tigcc-project);;"
				"All Files (*)"
			)
		);

	if (filePath.isEmpty()) {
		return;
	}

	Project loadedProject;
	ProjectManager projectManager;
	QString errorMessage;

	if (!projectManager.loadProject(
			filePath,
			&loadedProject,
			&errorMessage
		)) {
		QMessageBox::critical(
			this,
			QStringLiteral("Cannot Open Project"),
			errorMessage
		);

		return;
	}

	const QString loadedProjectFile =
		QFileInfo(filePath).absoluteFilePath();

	if (!prepareForProjectChange()) {
		return;
	}

	currentProject =
		loadedProject;

	currentProjectFile =
		loadedProjectFile;

	updateProjectInterface();

	statusBar()->showMessage(
		QStringLiteral("Opened project \"%1\"")
			.arg(currentProject.name())
	);
} // End openProject


void
MainWindow::createHeaderFile()
{
	createProjectFile(
		QStringLiteral("include"),
		QStringLiteral(".h"),
		MainWindow::FType::FHEAD
	);
}

void
MainWindow::createSourceFile()
{
	createProjectFile(
		QStringLiteral("src"),
		QStringLiteral(".c"),
		MainWindow::FType::FSRC
	);
}

void
MainWindow::createGasFile()
{
	createProjectFile(
		QStringLiteral("gasm"),
		QStringLiteral(".asm"),
		MainWindow::FType::FGAS
	);
}


void
MainWindow::createProjectFile(
	const QString &subdirectory,
	const QString &extension,
	MainWindow::FType ftype
)
{
	if (currentProject.directory().isEmpty()) {
		QMessageBox::warning(
			this,
			QStringLiteral("No Project"),
			QStringLiteral(
				"Create or open a project before "
				"creating files."
			)
		);

		return;
	}

	QString description;
	switch (ftype) {
		case MainWindow::FType::FHEAD:
			description = QStringLiteral("header");
			break;
		case MainWindow::FType::FSRC:
			description = QStringLiteral("source");
			break;
		case MainWindow::FType::FGAS:
			description = QStringLiteral("gas");
			break;
	}

	bool accepted = false;

	QString fileName =
		QInputDialog::getText(
			this,
			QStringLiteral("New %1 File")
				.arg(description),
			QStringLiteral("File name:"),
			QLineEdit::Normal,
			QString(),
			&accepted
		);

	if (!accepted) {
		return;
	}

	fileName = fileName.trimmed();

	if (fileName.isEmpty()) {
		return;
	}

	const QFileInfo fileInfo(fileName);

	if (fileInfo.fileName() != fileName ||
		fileName == QStringLiteral(".") ||
		fileName == QStringLiteral("..")) {
		QMessageBox::warning(
			this,
			QStringLiteral("Invalid File Name"),
			QStringLiteral(
				"Please enter a file name, not a path."
			)
		);

		return;
	}

	if (!fileName.endsWith(
			extension,
			Qt::CaseInsensitive
		)) {
		fileName.append(extension);
	}

	const QDir projectDirectory(
		currentProject.directory()
	);

	if (!projectDirectory.mkpath(
			subdirectory
		)) {
		QMessageBox::critical(
			this,
			QStringLiteral("Cannot Create Directory"),
			QStringLiteral(
				"The project subdirectory could not be created."
			)
		);

		return;
	}

	const QString relativePath =
		QDir::cleanPath(
			QDir(subdirectory).filePath(fileName)
		);

	const QString absolutePath =
		projectDirectory.filePath(relativePath);

	if (QFile::exists(absolutePath)) {
		QMessageBox::warning(
			this,
			QStringLiteral("File Already Exists"),
			QStringLiteral(
				"A file with that name already exists."
			)
		);

		return;
	}

	QFile file(absolutePath);

	if (!file.open(
			QIODevice::WriteOnly |
			QIODevice::NewOnly
		)) {
		QMessageBox::critical(
			this,
			QStringLiteral("Cannot Create File"),
			file.errorString()
		);

		return;
	}

	file.close();

	switch (ftype) {
		case MainWindow::FType::FHEAD:
			currentProject.addHeaderFile(
				relativePath
			);
			break;
		case MainWindow::FType::FSRC:
			currentProject.addSourceFile(
				relativePath
			);
			break;
		case MainWindow::FType::FGAS:
			currentProject.addGasFile(
				relativePath
			);
			break;
    }

	updateProjectInterface();

	updateEditorInterface();

	saveCurrentProject();

	statusBar()->showMessage(
		QStringLiteral("Created %1")
			.arg(relativePath)
	);
} // End createProjectFile


void
MainWindow::openProjectFile(
	const QString &relativePath
)
{
	if (currentProject.directory().isEmpty()) {
		return;
	}

	const QString absolutePath =
		QDir(
			currentProject.directory()
		).filePath(
			relativePath
		);

	QString errorMessage;

	if (!editor->openFile(
			absolutePath,
			&errorMessage
		)) {
		QMessageBox::critical(
			this,
			QStringLiteral("Cannot Open File"),
			errorMessage
		);

		return;
	}

	statusBar()->showMessage(
		QStringLiteral("Opened %1")
			.arg(relativePath)
	);
} // End openProjectFile


void
MainWindow::saveCurrentFile()
{
	if (editor->currentFilePath().isEmpty()) {
		statusBar()->showMessage(
			QStringLiteral("No file is open")
		);

		return;
	}

	QString errorMessage;

	if (!editor->saveCurrentFile(
			&errorMessage
		)) {
		QMessageBox::critical(
			this,
			QStringLiteral("Cannot Save File"),
			errorMessage
		);

		return;
	}

	updateEditorInterface();

	statusBar()->showMessage(
		QStringLiteral("File saved")
	);
} // End saveCurrentFile


void
MainWindow::updateEditorInterface()
{
	QStringList titleParts;

	if (!currentProject.name().isEmpty()) {
		titleParts.append(
			currentProject.name()
		);
	}

	const QString filePath =
		editor->currentFilePath();

	if (!filePath.isEmpty()) {
		titleParts.append(
			QFileInfo(filePath).fileName()
		);
	}

	if (titleParts.isEmpty()) {
		titleParts.append(
			QStringLiteral("TIGCC-Qt")
		);
	} else {
		titleParts.append(
			QStringLiteral("TIGCC-Qt")
		);
	}

	QString windowTitle =
		titleParts.join(
			QStringLiteral(" — ")
		);

	if (editor->isModified()) {
		windowTitle.append(
			QStringLiteral(" *")
		);
	}

	setWindowTitle(
		windowTitle
	);

	if (saveFileAction != nullptr) {
		saveFileAction->setEnabled(
			!filePath.isEmpty() &&
			editor->isModified()
		);
	}
} // End updateEditorInterface


bool
MainWindow::confirmEditorChanges()
{
	if (!editor->hasModifiedFiles()) {
		return true;
	}

	QMessageBox messageBox(
		QMessageBox::Warning,
		QStringLiteral("Unsaved Changes"),
		QStringLiteral(
			"One or more files have unsaved changes.\n"
			"Do you want to save them before quitting?"
		),
		QMessageBox::Save |
		QMessageBox::Discard |
		QMessageBox::Cancel,
		this
	);

	messageBox.setDefaultButton(
		QMessageBox::Save
	);

	messageBox.setEscapeButton(
		QMessageBox::Cancel
	);

	const QMessageBox::StandardButton result =
		static_cast<QMessageBox::StandardButton>(
			messageBox.exec()
		);

	switch (result) {
		case QMessageBox::Save:
		{
			QString errorMessage;

			if (!editor->saveAllFiles(
					&errorMessage
				)) {
				QMessageBox::critical(
					this,
					QStringLiteral(
						"Cannot Save Files"
					),
					errorMessage
				);

				return false;
			}

			return true;
		}

		case QMessageBox::Discard:
			return true;

		case QMessageBox::Cancel:
		default:
			return false;
	}
} // End confirmEditorChanges


bool
MainWindow::prepareForProjectChange()
{
	if (!confirmEditorChanges()) {
		return false;
	}

	/*
	 * If modified files still exist, the user selected
	 * Discard in confirmEditorChanges().
	 */
	if (editor->hasModifiedFiles()) {
		QString errorMessage;

		if (!editor->discardAllChanges(
				&errorMessage
			)) {
			QMessageBox::critical(
				this,
				QStringLiteral(
					"Cannot Discard Changes"
				),
				errorMessage
			);

			return false;
		}
	}

	QString errorMessage;

	if (!editor->closeAllFiles(
			&errorMessage
		)) {
		QMessageBox::critical(
			this,
			QStringLiteral(
				"Cannot Close Editor Files"
			),
			errorMessage
		);

		return false;
	}

	return true;
} // End prepareForProjectChange


void
MainWindow::adjustEditorFontSize(
	int adjustment
)
{
	const int currentSize =
		editor->fontPointSize();

	const int newSize =
		currentSize + adjustment;

	QString errorMessage;

	if (!editor->setFontPointSize(
			newSize,
			&errorMessage
		)) {
		if (!errorMessage.isEmpty()) {
			statusBar()->showMessage(
				errorMessage
			);
		}

		return;
	}

	statusBar()->showMessage(
		QStringLiteral(
			"Editor font size: %1 pt"
		).arg(
			editor->fontPointSize()
		)
	);
}


void
MainWindow::closeEvent(
	QCloseEvent *event
)
{
	if (confirmEditorChanges()) {
		event->accept();
	} else {
		event->ignore();
	}
}
