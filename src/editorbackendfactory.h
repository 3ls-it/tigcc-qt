/*
 * @file    src/editorbackendfactory.h
 * @brief   Creates editor backend instances.
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_EDITORBACKENDFACTORY_H
#define TIGCC_QT_EDITORBACKENDFACTORY_H

#include <QObject>



class EditorBackend;
class QWidget;

enum class EditorBackendType
{
	QScintilla,
	KTextEditor,
	Vim
};

EditorBackend *
createEditorBackend(
	EditorBackendType type,
	QWidget *parent = nullptr
);

#endif // TIGCC_QT_EDITORBACKENDFACTORY_H
