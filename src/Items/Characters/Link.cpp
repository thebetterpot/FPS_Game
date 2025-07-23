#include "Link.h"

Link::Link(int playerId, QGraphicsItem *parent)
    : Character(playerId, ":/character1.png", parent) { // 补充精灵图路径
    // 可添加Link专属逻辑
}
