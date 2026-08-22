#ifndef TIGCC_QT_BUILDOUTPUTWIDGET_H
#define TIGCC_QT_BUILDOUTPUTWIDGET_H

#include <QPlainTextEdit>

class BuildOutputWidget : public QPlainTextEdit
{
public:
    explicit BuildOutputWidget(QWidget *parent = nullptr);

    void appendOutput(const QString &text);
};

#endif // TIGCC_QT_BUILDOUTPUTWIDGET_H
