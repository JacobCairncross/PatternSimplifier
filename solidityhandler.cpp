#include "solidityhandler.h"

SolidityHandler::SolidityHandler(){}

void SolidityHandler::sol_to_solast(QString filePath){
    QString command = "solc --ast-compact-json " + filePath + " > ./tmp.solast";
    system(qPrintable(command));
}

QByteArray SolidityHandler::get_ast(){
    QString fileName = "./tmp.solast";
    QFile file(fileName);
    file.open(QIODevice::ReadOnly | QFile::Text);

    QTextStream in(&file);
    QString text = in.readAll();
    text.remove(0,55);

    return text.toUtf8();
}
