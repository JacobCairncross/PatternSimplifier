#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTreeWidget>
#include "node.h"
#include "dependencies/QJsonModel/qjsonmodel.h"
#include "patterntreeitem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_as_triggered();

    void on_actionCompile_triggered();

    void on_selectNodeButton_clicked();

    void save_byte_to_file(QByteArray byteArr, QString fileName);

private:
    Ui::MainWindow *ui;
    QString currentFile = "";
    QJsonModel* ASTModel;
    QJsonTreeItem* selectedNodesRoot = new QJsonTreeItem();
    PatternTreeItem* patternNodesRoot = new PatternTreeItem();

};
#endif // MAINWINDOW_H
