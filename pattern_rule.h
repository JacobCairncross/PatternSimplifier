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
  RuleType rule_type();
  QString rule_name();
  QJsonValue get_value();


  void set_rule(RuleType rule);
  void set_value(QJsonValue _value);
  QJsonObject* toJson();

private:
  RuleType ruleType;
  QJsonValue value;
};

#endif // PATTERN_RULE_H
