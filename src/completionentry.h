/*
 * @file    src/completionentry.h
 * @brief   Canonical completion-data value types.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_COMPLETIONENTRY_H
#define TIGCC_QT_COMPLETIONENTRY_H

#include <QString>
#include <QList>



struct CompletionParameter
{
	QString name;
	QString type;
};


struct CompletionEntry
{
	QString name;
	QString kind;
	QString signature;
	QString returnType;
	QString description;
	QString header;

	QList<CompletionParameter>
	parameters;
};

#endif // TIGCC_QT_COMPLETIONENTRY_H
