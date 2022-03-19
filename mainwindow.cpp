#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "solidityhandler.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionNew_triggered()
{
    currentFile.clear();
    ui->solidityCodeContainer->setText(QString());
}


void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(fileName);
    currentFile = fileName;
    if(!file.open(QIODevice::ReadOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot open file : " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();
    ui->solidityCodeContainer->setText(text);
    file.close();
}


void MainWindow::on_actionSave_as_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");
    QByteArray byteText = ui->solidityCodeContainer->toPlainText().toUtf8();
    save_byte_to_file(byteText, fileName);
//    QFile file(fileName);
//    if(!file.open(QFile::WriteOnly | QFile::Text)){
//        QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());
//        return;
//    }
//    currentFile = fileName;
//    setWindowTitle(fileName);
//    QTextStream out(&file);
//    QString text = ui->solidityCodeContainer->toPlainText();
//    out << text;
//    file.close();
}

void MainWindow::save_byte_to_file(QByteArray byteArr, QString fileName){
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(byteArr);
    file.close();
}


void MainWindow::on_actionCompile_triggered()
{
    //Original way of making tree
//    SolidityHandler* solidityHandler = new SolidityHandler();
//    solidityHandler->sol_to_solast(currentFile);

//    QJsonObject ast = solidityHandler->get_ast();
//    QStringList keys = ast.keys();
//    for(int i=0;i<keys.length();i++){
//        QString currentKey = keys.at(i);
//        AddRoot(currentKey, ast.value(keys.at(i)));
//    }

    //Using QJsonTree library
    ASTModel = new QJsonModel;
    ui->newSolASTContainer->setModel(ASTModel);
    SolidityHandler* solidityHandler = new SolidityHandler();
    solidityHandler->sol_to_solast(currentFile);
    QByteArray cleanedJson = solidityHandler->get_ast();
    ASTModel->loadJson(cleanedJson);
    selectedNodesRoot = new QJsonTreeItem();
    selectedNodesRoot->setType(QJsonValue::Object);
    ASTModel->setSelectedNodesRoot(selectedNodesRoot);


}


void MainWindow::on_selectNodeButton_clicked()
{
    //Using QJsonTreeModel
    QModelIndex index = ui->newSolASTContainer->currentIndex();
    QJsonTreeItem* data = ASTModel->treeData(index);
    QList<QJsonTreeItem*> pathToRoot = QList<QJsonTreeItem*>();
    QJsonTreeItem* currentNode = data;
    while(currentNode != nullptr){
        pathToRoot.append(currentNode);
        currentNode = currentNode->parent();
    }
    //Find common ancestor so we can append new path to it
    QJsonTreeItem* mostCommonAncestor = selectedNodesRoot;
    int pathToRootIndex = pathToRoot.length()-2;
    while(mostCommonAncestor->hasChild(pathToRoot[pathToRootIndex]->key())){
        mostCommonAncestor = mostCommonAncestor->child(pathToRoot[pathToRootIndex]->key());
        pathToRootIndex--;
    }


    QJsonTreeItem* lastNode = mostCommonAncestor;
    for(int i = pathToRootIndex; i >= 0; i--){ //TODO: Add a check here to ensure this wont cause an out of bounds exception
        QJsonTreeItem* newNode = pathToRoot[i]->getChildlessCopy(lastNode);
        assert(lastNode == newNode->parent());
        lastNode->appendChild(newNode);
        lastNode = newNode;
    }

    QJsonValue jsonVal = ASTModel->genJson(selectedNodesRoot);
    save_byte_to_file(ASTModel->jsonToByte(jsonVal), "savedPattern.json");
}

