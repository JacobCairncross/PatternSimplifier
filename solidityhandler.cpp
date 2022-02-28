#include "solidityhandler.h"

SolidityHandler::SolidityHandler(){}

void SolidityHandler::sol_to_solast(QString filePath){
    QString command = "solc --ast-compact-json " + filePath + " > ./tmp.solast";
    system(qPrintable(command));
}

QJsonObject SolidityHandler::get_ast(){
    QString fileName = "./tmp.solast";
    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QFile::Text);

    QTextStream in(&file);
    QString text = in.readAll();
    text.remove(0,55);
    QByteArray jsonData = text.toUtf8();
    if(jsonData.isEmpty()) qDebug() << "AST is empty";

    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
    QJsonObject object = jsonDocument.object();
    return object;
}
