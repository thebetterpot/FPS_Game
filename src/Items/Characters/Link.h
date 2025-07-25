#ifndef LINK_H
#define LINK_H

#include "Character.h"

class Link : public Character {
    Q_OBJECT
public:
    // 明确声明与父类一致的构造函数参数
    explicit Link(int playerId, const QString& spritePath, QGraphicsItem *parent = nullptr);
};

#endif // LINK_H
