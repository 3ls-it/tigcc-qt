/*
 * @file    src/editorbackendfactory.cpp
 * @brief   Implements editor backend creation.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "editorbackendfactory.h"

#include "editorbackend.h"
#include "ktexteditorbackend.h"
#include "qscintillabackend.h"



EditorBackend *
createEditorBackend(
	EditorBackendType type,
	QWidget *parent
)
{
	switch (type) {
		case EditorBackendType::QScintilla:
			return new QScintillaBackend(
				parent
			);

#ifdef ENABLE_KTEXTEDITOR
		case EditorBackendType::KTextEditor:
			return new KTextEditorBackend(
				parent
			);
#endif
        }
	return nullptr;
}
