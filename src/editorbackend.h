#ifndef TIGCC_QT_EDITORBACKEND_H
#define TIGCC_QT_EDITORBACKEND_H

#include <QObject>
#include <QString>




class QWidget;

class EditorBackend : public QObject
{
    Q_OBJECT

public:
    explicit EditorBackend(
        QObject *parent = nullptr
    );

    ~EditorBackend() override = default;

    virtual QWidget *
    widget() = 0;

    virtual bool
    openFile(
        const QString &filePath,
        QString *errorMessage = nullptr
    ) = 0;

    virtual bool
    saveCurrentFile(
        QString *errorMessage = nullptr
    ) = 0;

    virtual QString
    currentFilePath() const = 0;

    virtual bool
    isModified() const = 0;

signals:
    void
    currentFileChanged(
        const QString &filePath
    );

    void
    modificationChanged(
        bool modified
    );

    void
    editorError(
        const QString &message
    );
};

#endif // TIGCC_QT_EDITORBACKEND_H
