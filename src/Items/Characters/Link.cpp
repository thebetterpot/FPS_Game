#include "Link.h"

// 严格匹配父类构造函数参数，正确传递参数给基类
Link::Link(int playerId, const QString& spritePath, QGraphicsItem *parent)
    : Character(playerId, spritePath, parent)  // 直接传递parent给Character构造函数
{
    // Link专属初始化逻辑（如特殊属性、武器等）
    // 示例：设置不同的移动速度（如果基类支持）
    // m_moveSpeed = 6; // 需在Character中声明相关成员
}
