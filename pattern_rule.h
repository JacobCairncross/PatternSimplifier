#ifndef PATTERN_RULE_H
#define PATTERN_RULE_H

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

class pattern_rule
{
public:
  enum RuleType{root, pass, key, literalValue, index, size, contains};
  pattern_rule(RuleType _ruleType);
  pattern_rule(RuleType _ruleType, QJsonValue _value);
  pattern_rule(RuleType _ruleType, QString _key);
  pattern_rule(RuleType _ruleType, QJsonValue _valueType, QString _stringVal);
  //TODO: Add more constructors
  RuleType rule_type();
  QString rule_name();
  QString key_value();
  //Value of literal can be string, signed int, unsigned int, float, bool
  QJsonValue value_type();
  //Need to know value_type before calling these value functions so user knows which to call
  QString string_value();
  //Unsure if we need int and uint value, nlohmann may not care as much as it seems
  int int_value();
  unsigned int uint_value();
  float float_value(); //Maybe change this to double?
  bool bool_value();
  int size_value();
  int index_value();
  QJsonValue get_value();
  QString value_to_string();
  QJsonValue value_as_json();

  //Setters, maybe return bool to indicate success or fail?
  void set_key_value(QString value);
  void set_value_type(QJsonValue value);
  void set_string_value(QString value);
  void set_int_value(int value);
  void set_uint_value(unsigned int value);
  //TODO: change this to double for more precision
  void set_float_value(float value);
  void set_bool_value(bool value);
  void set_size_value(int value);
  void set_index_value(int value);
  void set_value(QJsonValue _value);
  QJsonObject* toJson();

private:
  RuleType ruleType;
  QJsonValue valueType;
  QString keyValue;
  QString stringValue;
  int intValue;
  unsigned int uintValue;
  float floatValue;
  bool boolValue;
  int sizeValue;
  int indexValue;
  QJsonValue value;
};

#endif // PATTERN_RULE_H
