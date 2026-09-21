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
#include "configuration.h"
#include "mainwindow.h"

#include "completiondatabackend.h"
#include "completiondatalocator.h"
#include <QDebug>



int
main(int argc, char *argv[])
{
	QApplication application(argc, argv);

	Appearance::applyDarkTheme();

	application.setApplicationName(
		QStringLiteral("tigcc-qt")
	);

	application.setApplicationVersion(
		QStringLiteral("0.2.4-alpha")
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



	// Completion loading tests
	CompletionDataBackend completionData;

	QString completionError;

	const QString completionPath =
		CompletionDataLocator::locate(
			&completionError
		);

	if (completionPath.isEmpty()) {
		qWarning()
			<< completionError;
	} else {
		qDebug()
			<< "Completion data path:"
			<< completionPath;

		if (!completionData.load(
				completionPath,
				&completionError
			)) {
			qWarning()
				<< "Completion data load failed:"
				<< completionError;
		} else {
			qDebug()
				<< "Loaded completion entries:"
				<< completionData.entries().size();

			const QList<CompletionEntry> matches =
				completionData.findCompletions(
					QStringLiteral("pri")
				);

			qDebug()
				<< "Completion matches for 'pri':"
				<< matches.size();

			for (const CompletionEntry &entry :
					matches) {
				qDebug()
					<< "  "
					<< entry.name
					<< entry.kind
					<< entry.signature;
			}
		}
	}
	//



	MainWindow mainWindow(
		configuration
	);

	mainWindow.show();

	return application.exec();
}
