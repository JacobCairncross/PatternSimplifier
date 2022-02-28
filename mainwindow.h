#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTreeWidget>
#include "node.h"
#include "dependencies/QJsonModel/qjsonmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void AddRoot(QString name, QJsonValue value);
    void AddChild(QTreeWidgetItem* parent, QString name, QJsonValue value);

private slots:
    void on_actionNew_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_as_triggered();

    void on_actionCompile_triggered();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QString currentFile = "";
    QJsonModel* ASTModel;

};
#endif // MAINWINDOW_H
