#ifndef TIGCC_QT_MAINWINDOW_H
#define TIGCC_QT_MAINWINDOW_H

#include <QMainWindow>

class BuildOutputWidget;
class EditorWidget;
class ProjectTreeWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    ProjectTreeWidget *projectTree;
    EditorWidget *editor;
    BuildOutputWidget *buildOutput;
};

#endif // TIGCC_QT_MAINWINDOW_H
