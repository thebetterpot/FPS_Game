#include "Link.h"

// 初始化Link角色（继承自Character，使用相同素材）
Link::Link(int playerId, QGraphicsItem *parent)
    : Character(playerId, parent) {
    // 可在此添加Link角色的专属逻辑
}
