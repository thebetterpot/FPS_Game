#ifndef LINK_H
#define LINK_H

#include "Character.h"

class Link : public Character {
public:
    Link(int playerId, QGraphicsItem *parent = nullptr);
};

#endif // LINK_H
