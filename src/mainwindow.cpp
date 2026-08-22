#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QStatusBar>
#include <QFileInfo>

#include "mainwindow.h"
#include "buildoutputwidget.h"
#include "editorwidget.h"
#include "projectmanager.h"
#include "projecttreewidget.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
	  currentProject(
		  QStringLiteral("Untitled"),
		  QString()
	  ),
	  projectTree(new ProjectTreeWidget(this)),
	  editor(new EditorWidget(this)),
	  buildOutput(new BuildOutputWidget(this))
{
    setWindowTitle(
		QStringLiteral("TIGCC-Qt")
    );

    resize(1440, 810);

	updateProjectInterface();

    auto *rightSplitter = new QSplitter(
		Qt::Vertical,
		this
    );

    rightSplitter->addWidget(editor);
    rightSplitter->addWidget(buildOutput);

    rightSplitter->setStretchFactor(0, 3);
    rightSplitter->setStretchFactor(1, 1);

    auto *mainSplitter = new QSplitter(
		Qt::Horizontal,
		this
    );

    mainSplitter->addWidget(projectTree);
    mainSplitter->addWidget(rightSplitter);

    mainSplitter->setStretchFactor(0, 1);
    mainSplitter->setStretchFactor(1, 4);

    setCentralWidget(mainSplitter);

    auto *fileMenu = menuBar()->addMenu(
		QStringLiteral("&Project")
    );


    auto *quitAction = fileMenu->addAction(
		QStringLiteral("&Quit")
    );

    connect(
		quitAction,
		&QAction::triggered,
		this,
		&QMainWindow::close
    );

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

    auto *helpMenu = menuBar()->addMenu(
		QStringLiteral("&Help")
    );

    helpMenu->addAction(
		QStringLiteral("About TIGCC-Qt")
    );

    statusBar()->showMessage(
		QStringLiteral("Ready")
    );
}



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

	currentProject = Project(
		trimmedName,
		projectDirectory
	);

	currentProjectFile =
		QDir(projectDirectory).filePath(
			QStringLiteral("%1.tigcc-project")
				.arg(trimmedName)
		);

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

	updateProjectInterface();

	setWindowTitle(
		QStringLiteral("%1 - TIGCC-Qt")
			.arg(currentProject.name())
	);

	statusBar()->showMessage(
		QStringLiteral("Created project “%1”")
			.arg(currentProject.name())
	);
}


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

	currentProject = loadedProject;

	currentProjectFile =
		QFileInfo(filePath).absoluteFilePath();

	updateProjectInterface();

	statusBar()->showMessage(
		QStringLiteral("Opened project \"%1\"")
			.arg(currentProject.name())
	);
}
