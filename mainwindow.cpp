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
    while(mostCommonAncestor->hasChild(pathToRoot[pathToRootIndex]->key())){
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
        lastNode->correspondingPatternItem()->appendChild(newItem);
        newNode->setCorrespondingPatternItem(newItem);
        lastNode = newNode;
        //See if you can get this to print to a file
    }



    QJsonValue jsonVal = ASTModel->genJson(selectedNodesRoot);
    save_byte_to_file(ASTModel->jsonToByte(jsonVal), "savedPattern.json");

    save_byte_to_file(QJsonDocument(*patternNodesRoot->toJson()).toJson(), "savedPatternItem.json");
}

pattern_rule* make_rule_from_AST_item(QJsonTreeItem* item){
    pattern_rule::RuleType ruleType;
    QJsonValue value;
    if(item->type() == QJsonValue::Type::Object){
        ruleType = pattern_rule::RuleType::key;
        value = item->key();
    }
    else if(item->type() == QJsonValue::Type::Array){
        ruleType = pattern_rule::RuleType::size;
        value = item->childCount();
    }
    else{
        ruleType = pattern_rule::RuleType::literalValue;
        value = QJsonValue::fromVariant(item->value());
    }

    return make_rule(ruleType, value);
}

pattern_rule* make_rule(pattern_rule::RuleType ruleType, QJsonValue value){
    pattern_rule* rule = new pattern_rule(ruleType);
    //Got to make a parameter to store values for the different rules. E.g a parameter to fill in key value, index value, string value, etc
    //maybe make separate functions for all scenarios. Contains value has to have a value of any type so maybe leave that for a bit and do the others first.
    //Implement contains but only for primitives for now
    bool errorOccurred = false;
    switch(ruleType){
    case pattern_rule::RuleType::key:
    {
        if(value.isString()){
            rule->set_key_value(value.toString());
        }
        else{
            errorOccurred = true;
        }
        break;
    }
    case pattern_rule::RuleType::index:
    {
        if(value.isDouble()){
            //Just truncates if not an actual int. Possibly add error later if not a strict int
            rule->set_index_value((int)value.toDouble());
        }
        else{
            errorOccurred = true;
        }
        break;
    }
    case pattern_rule::RuleType::size:
    {
        if(value.isDouble()){
            //Just truncates if not an actual int. Possibly add error later if not a strict int
            rule->set_size_value((int)value.toDouble());
        }
        else{
            errorOccurred = true;
        }
        break;
    }
    case pattern_rule::RuleType::literalValue:
    {
        make_literal_rule(rule, value);
    }
    }
    return rule;
}

void make_literal_rule(pattern_rule* rule, QJsonValue value){
    switch(value.type()){
    case QJsonValue::Type::String:
    {
        rule->set_value_type(QJsonValue::Type::String);
        rule->set_string_value(value.toString());
        break;
    }
    case QJsonValue::Type::Double:
    {
        //Value type will always say double so int will never be used. This may cause issues when using nlohmann. For now just treat everything as double
        rule->set_value_type(QJsonValue::Type::Double);
        rule->set_float_value(value.toDouble());
        break;
    }
    case QJsonValue::Type::Bool:
    {
        rule->set_value_type(QJsonValue::Type::Bool);
        rule->set_float_value(value.toBool());
        break;
    }
    }
}

void addPatternItemToTree(PatternTreeItem* item){

}
