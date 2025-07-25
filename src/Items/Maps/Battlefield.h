#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include "Map.h"

// 战场地图（继承自Map，使用特定背景图）
class Battlefield : public Map {
public:
    explicit Battlefield(QGraphicsItem* parent = nullptr) : Map(parent) {
        // 加载战场背景图
        loadBackground(":/assets/map_tu.png");
    }
};

#endif // BATTLEFIELD_H
