#include <QApplication>
#include "mainwindow.h"

int
main(int argc, char *argv[])
{
	QApplication application(argc, argv);

	application.setApplicationName(
	QStringLiteral("TIGCC-Qt")
	);

	application.setApplicationDisplayName(
	QStringLiteral("TIGCC-Qt")
	);

	application.setApplicationVersion(
	QStringLiteral("0.1.0")
	);

	MainWindow mainWindow;
	mainWindow.show();

	return application.exec();
}
