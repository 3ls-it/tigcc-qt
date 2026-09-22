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
		QStringLiteral("0.3.3-alpha")
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

	// Testing 1,2,3
	CompletionDataBackend completionData;

	QString completionError;

	const QString completionPath =
		CompletionDataLocator::locate(
			&completionError
		);

	if (completionPath.isEmpty()) {
		qWarning()
			<< "Completion data unavailable:"
			<< completionError;
	} else if (!completionData.load(
			completionPath,
			&completionError
		)) {
		qWarning()
			<< "Completion data could not be loaded:"
			<< completionError;
	} else {
		const QString completionScriptPath =
			QDir(
				QStandardPaths::writableLocation(
					QStandardPaths::TempLocation
				)
			).filePath(
				QStringLiteral(
					"tigcc-qt-vim-completion-test.vim"
				)
			);

		if (!VimCompletionAdapter::writeCompletionScript(
				&completionData,
				completionScriptPath,
				&completionError
			)) {
			qWarning()
				<< "Could not write Vim completion script:"
				<< completionError;
		} else {
			qDebug()
				<< "Vim completion script:"
				<< completionScriptPath;
		}
	}	
	//


	MainWindow mainWindow(
		configuration
	);

	mainWindow.show();

	return application.exec();
}
