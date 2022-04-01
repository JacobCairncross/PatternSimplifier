#include "pattern_rule.h"

pattern_rule::pattern_rule(RuleType _ruleType){
  ruleType = _ruleType;
}
pattern_rule::pattern_rule(RuleType _ruleType, QJsonValue _value){
    ruleType = _ruleType;
    value = _value;
}


pattern_rule::RuleType pattern_rule::rule_type(){
  return ruleType;
}

QString pattern_rule::rule_name(){
    switch(ruleType){
    case root:
        return "ROOT";
    case pass:
        return "PASS";
    case key:
        return "KEY";
    case literalValue:
        return "LITERAL_VALUE";
    case index:
        return "INDEX";
    case size:
        return "SIZE";
    case contains:
        return "CONTAINS";
    }
}


QJsonValue pattern_rule::get_value(){
    return value;
}


void pattern_rule::set_value(QJsonValue _value){
    value = _value;
}

void pattern_rule::set_rule(RuleType rule){
    ruleType = rule;
}


QJsonObject* pattern_rule::toJson(){
    QJsonObject* obj = new QJsonObject();
    obj->insert("rule", rule_name());
    obj->insert("value", value);
    return obj;

}












