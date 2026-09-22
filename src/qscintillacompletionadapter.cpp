/*
 * @file    src/qscintillacompletionadaptor.cpp
 * @brief   
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Qsci/qsciapis.h>
#include <Qsci/qscilexer.h>

#include <QString>

#include "qscintillacompletionadapter.h"
#include "completiondatabackend.h"



QScintillaCompletionAdapter::QScintillaCompletionAdapter(
	const CompletionDataBackend *data
)
	: m_data(data)
{
}


QString
QScintillaCompletionAdapter::formatEntry(
	const CompletionEntry &entry
) const
{
	if (!entry.signature.isEmpty()) {
		return entry.signature;
	}

	return entry.name;
}


bool
QScintillaCompletionAdapter::install(
	QsciLexer *lexer,
	QString *errorMessage
)
{
	if (lexer == nullptr) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"No QScintilla lexer was provided."
				);
		}

		return false;
	}

	if (m_data == nullptr ||
		!m_data->isLoaded()) {
		if (errorMessage != nullptr) {
			*errorMessage =
				QStringLiteral(
					"Completion data is not loaded."
				);
		}

		return false;
	}

	auto *apis =
		new QsciAPIs(
			lexer
		);

	for (const CompletionEntry &entry :
			m_data->entries()) {
		apis->add(
			formatEntry(
				entry
			)
		);
	}

	apis->prepare();

	lexer->setAPIs(
		apis
	);

	return true;
} // End install
