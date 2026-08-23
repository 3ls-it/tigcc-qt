/*
 * @file    src/project.h
 * @brief   Header file for project.cpp
 *
 * This file is part of TIGCC-Qt.
 *
 * Copyright (c) 2026 J Adams <jfa63@duck.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TIGCC_QT_PROJECT_H
#define TIGCC_QT_PROJECT_H

#include <QString>
#include <QStringList>



class Project
{
public:
    Project();
    Project(
        const QString &name,
        const QString &directory
    );

    QString
    name() const;

    void
    setName(const QString &name);

    QString
    directory() const;

    void
    setDirectory(const QString &directory);

    QStringList
    sourceFiles() const;

    QStringList
    headerFiles() const;

    void
    addSourceFile(const QString &path);

    void
    removeSourceFile(const QString &path);

    void
    addHeaderFile(const QString &path);

    void
    removeHeaderFile(const QString &path);

private:
    QString m_name;
    QString m_directory;
    QStringList m_sourceFiles;
    QStringList m_headerFiles;
};

#endif // TIGCC_QT_PROJECT_H
