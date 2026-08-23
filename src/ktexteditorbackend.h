/*
 * @file    src/keditorbackend.h
 * @brief   Header for keditorbackend.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_KTEXTEDITORBACKEND_H
#define TIGCC_QT_KTEXTEDITORBACKEND_H

#include "editorbackend.h"



namespace KTextEditor
{
    class Document;
    class View;
}

class KTextEditorBackend : public EditorBackend
{
    Q_OBJECT

public:
    explicit KTextEditorBackend(
        QWidget *parent = nullptr
    );

    QWidget *
    widget() override;

    bool
    openFile(
        const QString &filePath,
        QString *errorMessage = nullptr
    ) override;

    bool
    saveCurrentFile(
        QString *errorMessage = nullptr
    ) override;

    QString
    currentFilePath() const override;

    bool
    isModified() const override;

private slots:
    void
    documentModifiedChanged();

private:
    KTextEditor::Document *m_document;
    KTextEditor::View *m_view;
    QWidget *m_editorWidget;
};

#endif // TIGCC_QT_KTEXTEDITORBACKEND_H
