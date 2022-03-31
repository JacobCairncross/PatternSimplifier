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
        //Check if item is literal, if so add a new child rule to match exactly that literal
        if(pathToRoot[i]->type() != QJsonValue::Type::Object && pathToRoot[i]->type() != QJsonValue::Type::Array){
            PatternTreeItem* literalRule = new PatternTreeItem(make_rule(pattern_rule::RuleType::literalValue, QJsonValue::fromVariant(pathToRoot[i]->value())), newItem);
            newItem->appendChild(literalRule);
        }
        lastNode->correspondingPatternItem()->appendChild(newItem);
        newNode->setCorrespondingPatternItem(newItem);
        lastNode = newNode;
    }

    save_byte_to_file(QJsonDocument(*patternNodesRoot->toJson()).toJson(), "savedPatternItem.json");
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
//    return make_rule(ruleType, value);
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

void MainWindow::on_changeValueButton_clicked()
{
//    QModelIndex index = ui->newSolASTContainer->currentIndex();
//    QJsonTreeItem* data = ASTModel->treeData(index);
//    if(ui->typeBox->currentText() == "int"){
//        bool isInt = false;
//        int intValue = ui->valueBox->text().toInt(&isInt);
//        if(isInt){
//            data->correspondingPatternItem()->rule()->set
//        }
//    }
}

