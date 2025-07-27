#ifndef WEAPON_H
#define WEAPON_H

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>
#include <QTimer>

// 武器类型枚举
enum WeaponType {
    RIFLE,      // 步枪
    SNIPER,     // 狙击枪
    KNIFE,      // 小刀
    BALL        // 实心球
};

class Weapon : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Weapon(WeaponType type, const QString& imagePath, QObject* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    WeaponType getType() const;
    bool isPickable() const;
    void setPickedUp(bool picked);

    // 武器属性
    int getDamage() const;
    int getAmmo() const;
    int getMaxAmmo() const;
    qreal getFireRate() const; // 攻击间隔(秒)
    bool canAttack() const;
    void useAmmo();
    void resetAttackCooldown();

signals:
    void attack(WeaponType type, QPointF position, qreal direction); // 方向: 1右, -1左

public slots:
    void updateCooldown();

private:
    WeaponType m_type;
    QPixmap m_sprite;
    bool m_isPickedUp;
    bool m_isOnGround;

    // 武器属性
    int m_damage;
    int m_ammo;
    int m_maxAmmo;
    qreal m_fireRate; // 攻击间隔(秒)
    qreal m_attackCooldown; // 当前冷却时间(秒)

    // 缩放比例(根据武器类型调整)
    qreal m_scale;
};

#endif // WEAPON_H
