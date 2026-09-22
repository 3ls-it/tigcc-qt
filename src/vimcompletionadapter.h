/*
 * @file    src/vimcompletionadapter.h
 * @brief   Generates Vim completion data for one Vim session.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_VIMCOMPLETIONADAPTER_H
#define TIGCC_QT_VIMCOMPLETIONADAPTER_H

#include <QString>



class CompletionDataBackend;

class VimCompletionAdapter
{
public:
	static bool
	writeCompletionScript(
		const CompletionDataBackend *completionData,
		const QString &scriptPath,
		QString *errorMessage = nullptr
	);

/*
private:
	static QString
	vimString(
		const QString &value
	);

	static QChar
	vimKind(
		const QString &kind
	);
 */
};

#endif // TIGCC_QT_VIMCOMPLETIONADAPTER_H
