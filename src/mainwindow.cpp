#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QStatusBar>

#include "mainwindow.h"
#include "buildoutputwidget.h"
#include "editorwidget.h"
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

    resize(1200, 675);

	projectTree->setProject(
		currentProject
	);

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

    projectMenu->addAction(
		QStringLiteral("Open Project")
    );

    projectMenu->addAction(
		QStringLiteral("Build Project")
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

	projectTree->setProject(
		currentProject
	);

	setWindowTitle(
		QStringLiteral("%1 - TIGCC-Qt")
			.arg(currentProject.name())
	);

	statusBar()->showMessage(
		QStringLiteral("Created project “%1”")
			.arg(currentProject.name())
	);
}
