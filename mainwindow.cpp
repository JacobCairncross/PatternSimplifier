#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "solidityhandler.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->solidityASTContainer->setColumnCount(2);
//Node* root = new Node(NULL, "root", QVariant(QJsonValue()), Node::object);
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
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());
        return;
    }
    currentFile = fileName;
    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = ui->solidityCodeContainer->toPlainText();
    out << text;
    file.close();
}


void MainWindow::on_actionCompile_triggered()
{
    //Original way of making tree
    SolidityHandler* solidityHandler = new SolidityHandler();
    solidityHandler->sol_to_solast(currentFile);

    QJsonObject ast = solidityHandler->get_ast();
    QStringList keys = ast.keys();
    for(int i=0;i<keys.length();i++){
        QString currentKey = keys.at(i);
        AddRoot(currentKey, ast.value(keys.at(i)));
    }

    //Using QJsonTree library
    ASTModel = new QJsonModel;
    ui->newSolASTContainer->setModel(ASTModel);
    ASTModel->load("tmp2.solast");
    selectedNodesRoot = new QJsonTreeItem();

}

void MainWindow::AddRoot(QString name, QJsonValue value){
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->solidityASTContainer);
    item->setText(0, name);
    if(value.isString()){
        item->setText(1,value.toString());
    }
    if(value.isObject()){
        QJsonObject object = value.toObject();
        QStringList keys = object.keys();
        for(int i = 0;i<keys.length();i++){
            QString currentKey = keys.at(i);
            AddChild(item, currentKey, object.value(keys.at(i)));
        }
    }
    if(value.isArray()){
        QJsonArray array = value.toArray();
        for(int i = 0; i<array.size(); i++){
            AddChild(item, QStringLiteral("%1").arg(i), array.at(i));
        }
    }

    ui->solidityASTContainer->addTopLevelItem(item);


}
void MainWindow::AddChild(QTreeWidgetItem* parent, QString name, QJsonValue value){
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, name);
    if(value.isString()){
        item->setText(1,value.toString());
    }
    else if(value.isDouble()){
        item->setText(1, QStringLiteral("%1").arg(value.toDouble()));
    }
    else if(value.isObject()){
        QJsonObject object = value.toObject();
        QStringList keys = object.keys();
        for(int i = 0;i<keys.length();i++){
            QString currentKey = keys.at(i);
            AddChild(item, currentKey, object.value(keys.at(i)));
        }
    }
    else if(value.isArray()){
        QJsonArray array = value.toArray();
        for(int i = 0; i<array.size(); i++){
            AddChild(item, QStringLiteral("%1").arg(i), array.at(i));
        }
    }
    else{
        item->setText(1, value.toString());
    }

    parent->addChild(item);
}


void MainWindow::on_pushButton_clicked()
{
    QTreeWidgetItem* currentNode = ui->solidityASTContainer->currentItem();
    QLinearGradient gradient(0,0,width(),0);
    gradient.setColorAt(0, QColor::fromRgbF(0, 1, 0, 1));

    QBrush brush(gradient);
    currentNode->setBackground(1, brush);

//    Node* newNode = new Node(MainWindow::root, currentNode->text(0), currentNode->text(1), Node::literal);
//    root->children.push_back(newNode);
//    ui->textEdit->setText("");
//    for(int i = 0; i<root->children.size();i++){
//        ui->textEdit->append(root->children.at(i)->getKey() + ", ");
//    }



}


void MainWindow::on_pushButton_2_clicked()
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

    for(int i = pathToRoot.length()-2; i >= 0; i++){ //TODO: Add a check here to ensure this wont cause an out of bounds exception
        QJsonTreeItem* newNode = pathToRoot[i]->getChildlessCopy(selectedNodesRoot);
        newNode->parent()->appendChild(newNode);
    }
    //BEGIN Test code
    QString pathText = "";
//    for(int i = 0; i< pathToRoot.length();i++){
//        pathText.append(pathToRoot[i]->key()).append(", ");
//    }
    QJsonTreeItem* currentNodeInSelectedTree = selectedNodesRoot;
    while(currentNodeInSelectedTree->childCount() > 0){
        pathText.append(currentNodeInSelectedTree->child(0)->key());
        currentNodeInSelectedTree = currentNodeInSelectedTree->child(0);
    }
    ui->textEdit->setText(pathText);
    //END Test code


}

