#ifndef TIGCC_QT_PROJECTMANAGER_H
#define TIGCC_QT_PROJECTMANAGER_H

#include <QString>

#include "project.h"




class ProjectManager
{
public:
    bool
    saveProject(
        const Project &project,
        const QString &filePath,
        QString *errorMessage = nullptr
    ) const;

    bool
    loadProject(
        const QString &filePath,
        Project *project,
        QString *errorMessage = nullptr
    ) const;
};

#endif // TIGCC_QT_PROJECTMANAGER_H
