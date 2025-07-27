#include "Weapon.h"
#include <QPainter>
#include <QDebug>

Weapon::Weapon(WeaponType type, const QString& imagePath, QObject* parent)
    : QObject(parent), m_type(type), m_isPickedUp(false), m_isOnGround(true),
    m_attackCooldown(0)
{
    // 1. 将基础路径定义为QString（关键修复）
    QString basePath = "C:/Users/gyh49/Desktop/Mine/Learn_Work/2025_Summer/C_Python_Practice/FPS_Game/qt-programming-2025-master/images/weapons/";

    // 2. 根据武器类型拼接文件名（QString支持与const char*直接拼接）
    QString testPath;
    switch (type) {
    case RIFLE:    testPath = basePath + "bu.png"; break;
    case SNIPER:   testPath = basePath + "ju.png"; break;
    case KNIFE:    testPath = basePath + "dao.png"; break;
    case BALL:     testPath = basePath + "qiu.png"; break;
    }

    // 3. 加载图片
    m_sprite = QPixmap(testPath);

    // 3. 精确调整缩放比例（适配实际图片尺寸）
    switch (type) {
    case RIFLE:    m_scale = 0.2;  break; // 307*119 → ~61*24
    case SNIPER:   m_scale = 0.25; break; // 307*67 → ~77*17
    case KNIFE:    m_scale = 0.15; break; // 307*49 → ~46*7
    case BALL:     m_scale = 0.2;  break; // 100*100 → ~20*20
    }

    // 4. 其他属性初始化（保持不变）
    m_ammo = m_maxAmmo;
    setFlag(QGraphicsItem::ItemIsMovable);
    setZValue(8); // 确保在角色下方（角色Z值建议设为10）

    if (m_sprite.isNull()) {
        qDebug() << "图片加载失败！路径:" << testPath;
        m_sprite = QPixmap(50, 50);
        m_sprite.fill(type == RIFLE ? Qt::blue :
                          type == SNIPER ? Qt::green :
                          type == KNIFE ? Qt::yellow : Qt::red);
    }
}

QRectF Weapon::boundingRect() const {
    return QRectF(0, 0,
                  m_sprite.width() * m_scale,
                  m_sprite.height() * m_scale);
}

void Weapon::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 计算缩放后的目标矩形
    QRectF target(0, 0,
                  m_sprite.width() * m_scale,
                  m_sprite.height() * m_scale);

    // 绘制完整图片（源矩形为图片本身，目标矩形为缩放后的区域）
    painter->drawPixmap(target, m_sprite, m_sprite.rect());
}

WeaponType Weapon::getType() const {
    return m_type;
}

bool Weapon::isPickable() const {
    return m_isOnGround && !m_isPickedUp;
}

void Weapon::setPickedUp(bool picked) {
    m_isPickedUp = picked;
    m_isOnGround = !picked;
    if (picked) {
        setVisible(false); // 被拾取后隐藏
    }
}

int Weapon::getDamage() const {
    return m_damage;
}

int Weapon::getAmmo() const {
    return m_ammo;
}

int Weapon::getMaxAmmo() const {
    return m_maxAmmo;
}

qreal Weapon::getFireRate() const {
    return m_fireRate;
}

bool Weapon::canAttack() const {
    // 小刀不需要子弹,其他武器需要有子弹且冷却结束
    if (m_type == KNIFE) {
        return m_attackCooldown <= 0;
    }
    return m_ammo > 0 && m_attackCooldown <= 0;
}

void Weapon::useAmmo() {
    if (m_type != KNIFE && m_ammo > 0) {
        m_ammo--;
    }
    m_attackCooldown = m_fireRate; // 重置冷却
}

void Weapon::resetAttackCooldown() {
    m_attackCooldown = 0;
}

void Weapon::updateCooldown() {
    if (m_attackCooldown > 0) {
        m_attackCooldown -= 0.016; // 假设60FPS更新
        if (m_attackCooldown < 0) {
            m_attackCooldown = 0;
        }
    }
}
