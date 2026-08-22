#include "mainwindow.h"

#include "buildoutputwidget.h"
#include "editorwidget.h"
#include "projecttreewidget.h"

#include <QMenu>
#include <QMenuBar>
#include <QSplitter>
#include <QStatusBar>

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

    resize(1100, 700);

	currentProject.addSourceFile(
		QStringLiteral("main.c")
	);

	currentProject.addSourceFile(
		QStringLiteral("startup.c")
	);

	currentProject.addHeaderFile(
		QStringLiteral("project.h")
	);

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
        QStringLiteral("&File")
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

    projectMenu->addAction(
        QStringLiteral("New Project")
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
