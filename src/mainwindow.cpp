#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QInputDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>

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

	projectMenu->addSeparator();

	auto *newSourceFileAction = projectMenu->addAction(
		QStringLiteral("New Source File")
	);

	connect(
		newSourceFileAction,
		&QAction::triggered,
		this,
		&MainWindow::createSourceFile
	);

	auto *newHeaderFileAction = projectMenu->addAction(
		QStringLiteral("New Header File")
	);

	connect(
		newHeaderFileAction,
		&QAction::triggered,
		this,
		&MainWindow::createHeaderFile
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


void
MainWindow::createSourceFile()
{
	createProjectFile(
		QStringLiteral("src"),
		QStringLiteral(".c"),
		false
	);
}


void
MainWindow::createHeaderFile()
{
	createProjectFile(
		QStringLiteral("include"),
		QStringLiteral(".h"),
		true
	);
}


void
MainWindow::createProjectFile(
	const QString &subdirectory,
	const QString &extension,
	bool headerFile
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

	const QString description =
		headerFile
			? QStringLiteral("header")
			: QStringLiteral("source");

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

	if (headerFile) {
		currentProject.addHeaderFile(
			relativePath
		);
	} else {
		currentProject.addSourceFile(
			relativePath
		);
	}

	updateProjectInterface();

	saveCurrentProject();

	statusBar()->showMessage(
		QStringLiteral("Created %1")
			.arg(relativePath)
	);
}

