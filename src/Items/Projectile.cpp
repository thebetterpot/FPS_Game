#include "Projectile.h"
#include <QPainter>

Projectile::Projectile(int damage, qreal speed, bool isBall, QObject* parent)
    : QObject(parent),
    m_damage(damage),
    m_speed(speed),
    m_isBall(isBall),
    m_direction(1),    // 默认向右
    m_gravity(0.1)     // 实心球重力（可调整）
{
    // 示例：加载默认精灵图（实际应根据武器类型加载对应图片）
    if (m_isBall) {
        m_sprite = QPixmap(":/images/projectiles/ball.png"); // 球的图片
    } else {
        m_sprite = QPixmap(":/images/projectiles/bullet.png"); // 子弹图片
    }
    if (m_sprite.isNull()) {
        m_sprite = QPixmap(10, 5); // 占位图
        m_sprite.fill(Qt::red);
    }
}

QRectF Projectile::boundingRect() const {
    return m_sprite.rect(); // 直接使用精灵图尺寸作为碰撞盒
}

void Projectile::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->drawPixmap(boundingRect().topLeft(), m_sprite); // 绘制精灵图
}

void Projectile::setDirection(qreal direction) {
    m_direction = direction;
    // 替换mirrored，使用QTransform实现水平翻转（兼容性更好）
    if (direction < 0) {
        QTransform transform;
        transform.scale(-1, 1); // 水平翻转（x轴反向）
        m_sprite = m_sprite.transformed(transform);
    }
}

void Projectile::updatePosition() {
    if (m_isBall) {
        // 抛物线运动：水平速度 + 垂直重力
        setX(x() + m_speed * m_direction);
        setY(y() + m_gravity);
        m_gravity += 0.05; // 模拟重力加速度
    } else {
        // 直线运动
        setX(x() + m_speed * m_direction);
    }
}

int Projectile::getDamage() const {
    return m_damage;
}

bool Projectile::isBall() const {
    return m_isBall;
}
