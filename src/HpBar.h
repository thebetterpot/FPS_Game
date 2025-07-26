#ifndef HPBAR_H
#define HPBAR_H

#include <QGraphicsItem>
#include <QObject>
#include "Items/Characters/Character.h"

// 血条类：显示单个玩家的血量
class HpBar : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    // 参数：关联的角色、血条位置偏移（用于区分两个玩家）
    HpBar(Character* target, int yOffset, QObject* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // 更新血条显示（根据角色当前血量）
    void updateHpBar();

private:
    Character* m_target;      // 关联的角色
    int m_width = 200;        // 血条宽度
    int m_height = 20;        // 血条高度
    int m_yOffset;            // Y轴偏移（用于区分两个血条）
};

#endif // HPBAR_H
