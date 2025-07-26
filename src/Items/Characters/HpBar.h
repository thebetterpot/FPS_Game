#ifndef HPBAR_H
#define HPBAR_H

#include <QGraphicsItem>
#include <QObject>
#include "Character.h"

class HpBar : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    HpBar(Character* target, int playerId, int yOffset, QObject* parent = nullptr);

    // 新增：获取玩家ID（用于调试）
    int getPlayerId() const { return m_playerId; }

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void updateHpBar();

private:
    Character* m_target;      // 关联角色
    int m_playerId;           // 玩家ID（1/2）
    int m_width = 200;        // 血条宽度
    int m_height = 30;        // 增大高度，提高可见性
    int m_yOffset;            // Y轴偏移
};

#endif // HPBAR_H
