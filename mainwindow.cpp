#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "solidityhandler.h"
#include "pattern_rule.h"
#include "patterntreeitem.h"

void make_literal_rule(pattern_rule* rule, QJsonValue value);
pattern_rule* make_rule(pattern_rule::RuleType ruleType, QJsonValue value);
pattern_rule* make_rule_from_AST_item(QJsonTreeItem* item);


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

}

void MainWindow::save_byte_to_file(QByteArray byteArr, QString fileName){
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(byteArr);
    file.close();
}


void MainWindow::on_actionCompile_triggered()
{
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
    selectedNodesRoot->setCorrespondingPatternItem(patternNodesRoot);


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
    while(pathToRootIndex >= 0 && mostCommonAncestor->hasChild(pathToRoot[pathToRootIndex]->key())){
        mostCommonAncestor = mostCommonAncestor->child(pathToRoot[pathToRootIndex]->key());
        pathToRootIndex--;
    }

    //Also need to find corresponding node in pattern item tree. Maybe add a bit in pattern tree item that maps to a selected node
    //Decide a rule for each item in pattern item. Maybe make it just 'has key' or 'index'?. Something to just pass through to the final value.
    //At some point add ability to specify at least final value in path selected but hopefully abiity to edit the rules higher up too.

    QJsonTreeItem* lastNode = mostCommonAncestor;
    for(int i = pathToRootIndex; i >= 0; i--){ //TODO: Add a check here to ensure this wont cause an out of bounds exception
        QJsonTreeItem* newNode = pathToRoot[i]->getChildlessCopy(lastNode);
        assert(lastNode == newNode->parent());
        lastNode->appendChild(newNode);
        PatternTreeItem* newItem = new PatternTreeItem(make_rule_from_AST_item(pathToRoot[i]), lastNode->correspondingPatternItem());
        //Check if item is literal, if so add a new child rule to match exactly that literal
        if(pathToRoot[i]->type() != QJsonValue::Type::Object && pathToRoot[i]->type() != QJsonValue::Type::Array){
            PatternTreeItem* literalRule = new PatternTreeItem(new pattern_rule(pattern_rule::RuleType::literalValue, QJsonValue::fromVariant(pathToRoot[i]->value())), newItem);
            newItem->appendChild(literalRule);
        }
        lastNode->correspondingPatternItem()->appendChild(newItem);
        newNode->setCorrespondingPatternItem(newItem);
        lastNode = newNode;
    }

    QByteArray jsonByteArray = QJsonDocument(*patternNodesRoot->toJson()).toJson();
    save_byte_to_file(jsonByteArray, "savedPatternItem.json");
    QAbstractItemModel* model = ui->patternContainer->model();
    ASTModel = new QJsonModel;
    ui->patternContainer->setModel(ASTModel);
    ASTModel->loadJson(jsonByteArray);
    delete model;
}

pattern_rule* make_rule_from_AST_item(QJsonTreeItem* item){
    pattern_rule::RuleType ruleType;
    QJsonValue value;
    QJsonTreeItem* parent = item->parent();
    if(parent->type() == QJsonValue::Type::Object){
        ruleType = pattern_rule::RuleType::key;
        value = item->key();
    }
    else if(parent->type() == QJsonValue::Type::Array){
        ruleType = pattern_rule::RuleType::index;
        //Default value is final index in array although this should always be changed to the actual index of the item
        value = parent->childCount();
        //Find what index item is in parent array
        for(int i = 0; i< parent->childCount();i++){
            if(parent->child(i) == item){
                value = i;
            }
        }
    }
    return new pattern_rule(ruleType, value);
}




void MainWindow::on_changeValueButton_clicked()
{
    QModelIndex index = ui->newSolASTContainer->currentIndex();
    QJsonTreeItem* data = ASTModel->treeData(index);
    QList<QJsonTreeItem*> pathToRoot = QList<QJsonTreeItem*>();
    QJsonTreeItem* currentNode = data;

    //Data isnt the actual node in selected nodes so we need to traverse the selected nodes to find it
    while(currentNode != nullptr){
        pathToRoot.append(currentNode);
        currentNode = currentNode->parent();
    }
    QJsonTreeItem* selectedNode = selectedNodesRoot;
    int pathToRootIndex = pathToRoot.length()-2;
    while(pathToRootIndex >= 0 && selectedNode->hasChild(pathToRoot[pathToRootIndex]->key())){
        selectedNode = selectedNode->child(pathToRoot[pathToRootIndex]->key());
        pathToRootIndex--;
    }
    PatternTreeItem* item = selectedNode->correspondingPatternItem();
    if(index.column() == 1 && selectedNode->value().isValid()){
        //If they select the second column AND the child is a literal value (if its value is valid then its a leaf literal) rule then select the child node
        item = item->child(0);
    }
    if(ui->ruleBox->currentText() == "Rule Type"){
        change_rule_type(item);
    }
    else if(ui->ruleBox->currentText() == "Rule Value"){
        change_rule_value(item);
    }


    QByteArray jsonByteArray = QJsonDocument(*patternNodesRoot->toJson()).toJson();
    save_byte_to_file(jsonByteArray, "savedPatternItem.json");
    QAbstractItemModel* model = ui->patternContainer->model();
    ASTModel = new QJsonModel;
    ui->patternContainer->setModel(ASTModel);
    ASTModel->loadJson(jsonByteArray);
    delete model;

}

void MainWindow::change_rule_type(PatternTreeItem* item){
    if(ui->valueBox->text().toLower() == "pass"){
        item->rule()->set_rule(pattern_rule::RuleType::pass);
    }
    else if(ui->valueBox->text().toLower() == "key"){
        item->rule()->set_rule(pattern_rule::RuleType::key);
    }
    else if(ui->valueBox->text().toLower() == "index"){
        item->rule()->set_rule(pattern_rule::RuleType::index);
    }
    else if(ui->valueBox->text().toLower() == "size"){
        item->rule()->set_rule(pattern_rule::RuleType::size);
    }
    else if(ui->valueBox->text().toLower() == "contains"){
        item->rule()->set_rule(pattern_rule::RuleType::contains);
    }
    else if(ui->valueBox->text().toLower() == "literal_value"){
        item->child(0)->rule()->set_rule(pattern_rule::RuleType::literalValue);
    }
}

void MainWindow::change_rule_value(PatternTreeItem* item){
    if(ui->typeBox->currentText() == "int"){
        bool isInt = false;
        int intValue = ui->valueBox->text().toInt(&isInt);
        if(isInt){
            item->rule()->set_value(QJsonValue(intValue));
        }
        //Handle error here
    }
    else if(ui->typeBox->currentText() == "float"){
        bool isFloat = false;
        float floatValue = ui->valueBox->text().toFloat(&isFloat);
        if(isFloat){
            item->rule()->set_value(QJsonValue(floatValue));
        }
        //Handle error here
    }
    else if(ui->typeBox->currentText() == "bool"){
        if(ui->valueBox->text().toLower() == "true"){
            item->rule()->set_value(QJsonValue(true));
        }
        else if(ui->valueBox->text().toLower() == "false"){
            item->rule()->set_value(QJsonValue(false));
        }
        //Handle error here
    }
    else{
        item->rule()->set_value(QJsonValue(ui->valueBox->text()));
    }
}
