#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>

class Projectile : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Projectile(int damage, qreal speed, bool isBall = false, QObject* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void setDirection(qreal direction); // 1为右, -1为左
    void updatePosition();
    int getDamage() const;
    bool isBall() const;

private:
    int m_damage;
    qreal m_speed;
    qreal m_direction; // 方向
    qreal m_gravity;   // 仅用于实心球
    bool m_isBall;     // 是否为实心球(抛物线运动)
    QPixmap m_sprite;
};

#endif // PROJECTILE_H
