#include "buildoutputwidget.h"

BuildOutputWidget::BuildOutputWidget(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);

    setPlaceholderText(
        QStringLiteral("Compiler output")
    );
}

void
BuildOutputWidget::appendOutput(const QString &text)
{
    appendPlainText(text);
}
