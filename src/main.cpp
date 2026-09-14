/*
 * @file    src/main.cpp
 * @brief   Entry point to TIGCC-Qt
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <QApplication>

#include "appearance.h"
#include "mainwindow.h"



int
main(int argc, char *argv[])
{
	QApplication application(argc, argv);

	Appearance::applyDarkTheme();

	application.setApplicationName(
		QStringLiteral("TIGCC-Qt")
	);

	application.setApplicationVersion(
		QStringLiteral("0.2.1-alpha")
	);

	application.setApplicationDisplayName(
		QStringLiteral("TIGCC-Qt") +
		QStringLiteral(" v") +
		application.applicationVersion()
	);

	MainWindow mainWindow;
	mainWindow.show();

	return application.exec();
}
