#ifndef NODE_H
#define NODE_H

#include <QJsonValue>
#include <QString>
#include <QVariant>

class Node
{
public:
    enum Type {literal, array, object};

    Node(Node* parent, QString key, QVariant value, Type type);
    Node* parent;
    std::vector<Node*> children;
    QString getKey();
    void setKey(QString key);
    QVariant getValue();
    void setValue(QVariant value);
    Type getType();
    void setType(Type type);

private:
    QString key;
    QVariant value;
    Type type;
};

#endif // NODE_H
