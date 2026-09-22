/*
 * @file    src/qscintillacompletionadapter.h
 * @brief   <brief description of this file>
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_QSCINTILLACOMPLETIONADAPTER_H
#define TIGCC_QT_QSCINTILLACOMPLETIONADAPTER_H

#include <QString>

#include "completionentry.h"



class CompletionDataBackend;
class QsciLexer;

class QScintillaCompletionAdapter
{
public:
	explicit QScintillaCompletionAdapter(
		const CompletionDataBackend *data
	);

	bool
	install(
		QsciLexer *lexer,
		QString *errorMessage = nullptr
	);

private:
	QString
	formatEntry(
		const CompletionEntry &entry
	) const;

	const CompletionDataBackend *m_data;
};

#endif // TIGCC_QT_QSCINTILLACOMPLETIONADAPTER_H
