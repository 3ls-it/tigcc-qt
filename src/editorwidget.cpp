#include "editorwidget.h"

EditorWidget::EditorWidget(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setPlaceholderText(
        QStringLiteral("Source editor")
    );
}
