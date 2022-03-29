#include "patterntreeitem.h"

PatternTreeItem::PatternTreeItem(){
    ruleObject = new pattern_rule(pattern_rule::RuleType::root);
}
PatternTreeItem::PatternTreeItem(pattern_rule* rule): ruleObject(rule){};
PatternTreeItem::PatternTreeItem(pattern_rule* rule, PatternTreeItem* parent): ruleObject(rule), mParent(parent){};


void PatternTreeItem::appendChild(PatternTreeItem * item){
    mChilds.append(item);
}

PatternTreeItem *PatternTreeItem::child(int index){
    return mChilds.at(index);
}

PatternTreeItem *PatternTreeItem::parent(){
    return mParent;
}

int PatternTreeItem::childCount() const{
    return mChilds.size();
}

void PatternTreeItem::setKey(const QString& key){
    mKey = key;
}

void PatternTreeItem::setRule(pattern_rule* rule){
    ruleObject = rule;
}

QString PatternTreeItem::key() const{
    return mKey;
}

pattern_rule* PatternTreeItem::rule() const{
    return ruleObject;
}

QJsonObject* PatternTreeItem::toJson(){
    QJsonObject* obj = new QJsonObject();
    obj->insert("ruleObject", QJsonValue(*ruleObject->toJson()));
    QJsonArray* children = new QJsonArray();
    for(auto child : mChilds){
        children->append(*child->toJson());
    }
    obj->insert("children", QJsonValue(*children));

    return obj;
//    QByteArray json;
//    QByteArray indentAmount = indent ? QByteArray(4 * depth, ' '): "";

//    json += "\n"+indentAmount+"{\n";
//    json += indentAmount+"\"ruleObject\": \n";
//    json += ruleObject->toJson(depth+1, indent);
//    json += ",\n";
//    json += indentAmount+"\"children\": ";
//    if(mChilds.size() != 0){
//        json += "[";
//        bool firstElement = true;
//        for(auto child : mChilds){
//            if(!firstElement){
//                json += ",";
//            }
//            json += child->toJson(depth+1, indent);
//            firstElement = false;
//        }
//        json += "\n"+indentAmount+"]";
//    }
//    else{
//        json += "[]";
//    }
//    json += "\n"+indentAmount+"}";
//    return json;
}

//PatternTreeItem* PatternTreeItem::getChildlessCopy(PatternTreeItem* parent) const{}
