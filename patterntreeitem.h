#ifndef PATTERNTREEITEM_H
#define PATTERNTREEITEM_H

#include "pattern_rule.h"
#include <QString>

class PatternTreeItem
{
public:
    PatternTreeItem();
    PatternTreeItem(pattern_rule* rule);
    PatternTreeItem(pattern_rule* rule, PatternTreeItem* parent);
    void appendChild(PatternTreeItem * item);
    PatternTreeItem *child(int index);
    PatternTreeItem *parent();
    int childCount() const;
    void setKey(const QString& key);
    void setRule(pattern_rule* rule);
    QString key() const;
    pattern_rule* rule() const;
    //PatternTreeItem* getChildlessCopy(PatternTreeItem* parent) const;
    QByteArray toJson(int depth = 0, bool indent = false);

    //static PatternTreeItem* load(const QJsonValue& value, PatternTreeItem * parent = 0);

protected:

private:
    QString mKey; //I believe unneccessary but leave in here until I can be sure
    pattern_rule* ruleObject;
    QList<PatternTreeItem*> mChilds;
    PatternTreeItem * mParent;
};

#endif // PATTERNTREEITEM_H
