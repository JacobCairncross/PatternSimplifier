#include "pattern_rule.h"

pattern_rule::pattern_rule(RuleType _ruleType){
  ruleType = _ruleType;
}
pattern_rule::pattern_rule(RuleType _ruleType, QString _key){
  ruleType = _ruleType;
  keyValue = _key;
}
pattern_rule::pattern_rule(RuleType _ruleType, QJsonValue _valueType, QString _stringVal){
  ruleType = _ruleType;
  valueType = _valueType;
  stringValue = _stringVal;
}


pattern_rule::RuleType pattern_rule::rule_type(){
  return ruleType;
}

QByteArray pattern_rule::rule_name(){
    switch(ruleType){
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

QString pattern_rule::key_value(){
  return keyValue;
}
//Value of literal can be string, signed int, unsigned int, float, bool
QJsonValue pattern_rule::value_type(){
  return valueType;
}
//Need to know value_type before calling these value functions so user knows which to call
QString pattern_rule::string_value(){
  return stringValue;
}
int pattern_rule::int_value(){
  return intValue;
}
unsigned int pattern_rule::uint_value(){
  return uintValue;
}
float pattern_rule::float_value(){
  return floatValue;
}
bool pattern_rule::bool_value(){
  return boolValue;
}
int pattern_rule::size_value(){
  return sizeValue;
}
int pattern_rule::index_value(){
  return indexValue;
}
QByteArray pattern_rule::value_to_string(){
    switch(ruleType){
    case pass:
        return "";
    case key:
        return keyValue.toUtf8();
    case literalValue:
    {
        if(valueType == QJsonValue::String){
            return stringValue.toUtf8();
        }
        else if(valueType == QJsonValue::Double){
            return QVariant(floatValue).toString().toUtf8();
        }
        else if(valueType == QJsonValue::Bool){
            return QVariant(boolValue).toString().toUtf8();
        }
    }
        break;
    case index:
        return QVariant(indexValue).toString().toUtf8();
    case size:
        return QVariant(sizeValue).toString().toUtf8();
    }
}

void pattern_rule::set_key_value(QString value){
  keyValue = value;
}
void pattern_rule::set_value_type(QJsonValue value){
  valueType = value;
}
void pattern_rule::set_string_value(QString value){
  stringValue = value;
}
void pattern_rule::set_int_value(int value){
  intValue = value;
}
void pattern_rule::set_uint_value(unsigned int value){
  uintValue = value;
}
void pattern_rule::set_float_value(float value){
  floatValue = value;
}
void pattern_rule::set_bool_value(bool value){
  boolValue = value;
}
void pattern_rule::set_size_value(int value){
  sizeValue = value;
}
void pattern_rule::set_index_value(int value){
  indexValue = value;
}

QByteArray pattern_rule::toJson(int depth, bool indent){
    QByteArray json;
    QByteArray indentAmount = indent ? QByteArray(4 * depth, ' '): "";

    //Double indentAmounts are purely for aesthetic purposes
    json += indentAmount+"{\n";
    json += indentAmount+"\"rule\": ";
    json += '"';
    json += rule_name();
    json += "\",\n";
    json += indentAmount+"\"value\": ";
    json += '"';
    json += value_to_string();
    json += "\"\n";
    json += indentAmount+"}";
    return json;
}












