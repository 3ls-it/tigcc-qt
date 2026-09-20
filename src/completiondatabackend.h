
/*
 * @file    src/completiondatabackend.h
 * @brief   Canonical completion-data backend interface.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_COMPLETIONDATABACKEND_H
#define TIGCC_QT_COMPLETIONDATABACKEND_H

#include <QList>
#include <QString>

#include "completionentry.h"



class CompletionDataBackend
{
public:
	CompletionDataBackend();

	bool
	load(
		const QString &filePath,
		QString *errorMessage = nullptr
	);

	bool
	isLoaded() const;

	const QList<CompletionEntry> &
	entries() const;

	QList<CompletionEntry>
	findCompletions(
		const QString &prefix
	) const;

private:
	QList<CompletionEntry> m_entries;
	bool m_loaded;
};

#endif // TIGCC_QT_COMPLETIONDATABACKEND_H
