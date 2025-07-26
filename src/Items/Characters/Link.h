#ifndef LINK_H
#define LINK_H

#include "Character.h"

class Link : public Character {
    Q_OBJECT

public:
    // 与Character构造函数匹配的构造函数
    Link(int playerId, const QString& spritePath, QObject* parent = nullptr);
    ~Link() override;
};

#endif // LINK_H
