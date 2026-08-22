#ifndef TIGCC_QT_EDITORWIDGET_H
#define TIGCC_QT_EDITORWIDGET_H

#include <QPlainTextEdit>

class EditorWidget : public QPlainTextEdit
{
public:
    explicit EditorWidget(QWidget *parent = nullptr);
};

#endif // TIGCC_QT_EDITORWIDGET_H
