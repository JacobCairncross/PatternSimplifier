#ifndef SOLIDITYHANDLER_H
#define SOLIDITYHANDLER_H

#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

class SolidityHandler
{
public:
    SolidityHandler();
    void sol_to_solast(QString filePath);
    QJsonObject get_ast();
};
#endif // SOLIDITYHANDLER_H
