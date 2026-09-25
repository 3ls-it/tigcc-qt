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

// Test includes
#include "completiondatabackend.h"
#include "completiondatalocator.h"
#include "vimcompletionadapter.h"

#include <QDir>
#include <QDebug>
#include <QStandardPaths>
//

#include <QApplication>

#include "appearance.h"
#include "configuration.h"
#include "mainwindow.h"



int
main(int argc, char *argv[])
{
	QApplication application(argc, argv);

	Appearance::applyDarkTheme();

	application.setApplicationName(
		QStringLiteral("tigcc-qt")
	);

	application.setApplicationVersion(
		QStringLiteral("0.4.2-alpha")
	);

	application.setApplicationDisplayName(
		QStringLiteral("TIGCC-Qt") +
		QStringLiteral(" v") +
		application.applicationVersion()
	);

	Configuration configuration;

	QString configurationError;

	if (!configuration.load(
			&configurationError
		)) {
		qWarning()
			<< "Could not load configuration:"
			<< configurationError;
	}

	MainWindow mainWindow(
		configuration
	);

	mainWindow.show();

	return application.exec();
}
