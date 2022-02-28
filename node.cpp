#include "node.h"

Node::Node(Node* parent, QString key, QVariant value, Type type)
{
    parent = parent;
    key = key;
    value = value;
    type = type;
}

QString Node::getKey(){
    return key;
}
void Node::setKey(QString key){
    key = key;
}
QVariant Node::getValue(){
    return value;
}
void Node::setValue(QVariant value){
    value = value;
}
Node::Type Node::getType(){
    return type;
}
void Node::setType(Type type){
    type = type;
}
