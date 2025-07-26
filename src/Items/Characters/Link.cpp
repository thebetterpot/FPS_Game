#include "Link.h"

// 调用父类构造函数
Link::Link(int playerId, const QString& spritePath, QObject* parent)
    : Character(playerId, spritePath, parent) {
    // 可添加Link特有的初始化逻辑
}

Link::~Link() {
    // 可添加Link特有的清理逻辑
}
