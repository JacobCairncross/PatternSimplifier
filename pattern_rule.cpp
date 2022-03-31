#include "pattern_rule.h"

pattern_rule::pattern_rule(RuleType _ruleType){
  ruleType = _ruleType;
}
pattern_rule::pattern_rule(RuleType _ruleType, QJsonValue _value){
    ruleType = _ruleType;
    value = _value;
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

QJsonValue pattern_rule::get_value(){
    return value;
}

QString pattern_rule::value_to_string(){
    switch(ruleType){
    case root:
        return "";
    case pass:
        return "";
    case key:
        return keyValue;
    case literalValue:
    {
        if(valueType == QJsonValue::String){
            return stringValue;
        }
        else if(valueType == QJsonValue::Double){
            return QVariant(floatValue).toString();
        }
        else if(valueType == QJsonValue::Bool){
            return QVariant(boolValue).toString();
        }
    }
        break;
    case index:
        return QVariant(indexValue).toString();
    case size:
        return QVariant(sizeValue).toString();
    }
}

QJsonValue pattern_rule::value_as_json(){
    switch(ruleType){
    case root:
        return QJsonValue("");
    case pass:
        return QJsonValue("");
    case key:
        return QJsonValue(keyValue);
    case literalValue:
    {
        if(valueType == QJsonValue::String){
            return QJsonValue(stringValue);
        }
        else if(valueType == QJsonValue::Double){
            return QJsonValue(floatValue);
        }
        else if(valueType == QJsonValue::Bool){
            return QJsonValue(boolValue);
        }
    }
        break;
    case index:
        return QJsonValue(indexValue);
    case size:
        return QJsonValue(sizeValue);
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
void pattern_rule::set_value(QJsonValue _value){
    value = _value;
}

QJsonObject* pattern_rule::toJson(){
    QJsonObject* obj = new QJsonObject();
    obj->insert("rule", rule_name());
    obj->insert("value", value);
    return obj;

}












