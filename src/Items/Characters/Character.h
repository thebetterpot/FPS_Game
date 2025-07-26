#ifndef CHARACTER_H
#define CHARACTER_H

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>
#include <QPainter>

class Character : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    explicit Character(int playerId, const QString& spritePath, QObject *parent = nullptr);
    ~Character() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPointF velocity() const;
    void updatePhysics(int deltaTime);
    void updateAnimation(int deltaTime);
    void setLeftPressed(bool pressed);
    void setRightPressed(bool pressed);
    void setJumpPressed(bool pressed);
    void setCrouchPressed(bool pressed);
    void setHp(int hp);        // 设置当前血量
    int getHp() const;         // 获取当前血量
    int getMaxHp() const;      // 获取最大血量

    // 新增：攻击相关方法
    void attack();                  // 发起攻击
    void takeDamage(int damage);    // 受到伤害
    bool isAttacking() const;       // 是否正在攻击
    QRectF attackHitbox() const;    // 获取攻击判定区域

    // 新增：设置攻击状态（用于输入控制）
    void setAttackPressed(bool pressed);

    // 新增：判断是否站在冰块顶部
    bool isOnIce() const;

private:
    int m_playerId;
    QPixmap m_sprite;
    int m_currentFrame = 0;
    int m_currentRow = 1;
    int m_frameDelay = 100;
    int m_frameTimer = 0;
    mutable bool m_isMoving = false;
    bool m_leftPressed = false;
    bool m_rightPressed = false;
    bool m_jumpPressed = false;
    bool m_crouchPressed = false;
    bool m_inAir; // 是否在空中
    qreal m_gravity; // 重力加速度
    qreal m_ascendSpeed; // 向上跳跃速度（负值表示向上）
    const int m_maxAscendTime; // 最长跳跃时间（400毫秒）
    int m_ascendTime; // 当前跳跃持续时间
    int m_lastFacingRow = 1;
    int m_currentHp = 100;        // 当前血量（初始满血）
    const int m_maxHp = 100;      // 最大血量（固定100）

    // 新增：攻击相关成员
    bool m_isAttacking = false;     // 是否正在攻击
    bool m_attackPressed = false;   // 攻击键是否按下
    int m_attackCooldown = 0;       // 攻击冷却计时器（毫秒）
    const int m_attackCooldownMax = 1000; // 攻击冷却时间（1秒）
    int m_attackDuration = 0;       // 攻击动画持续时间
    const int m_attackDurationMax = 300;  // 攻击动画持续时间（300毫秒）
    // 新增：受伤冷却相关
    int m_damageCooldown = 0;       // 受伤冷却计时器（毫秒）
    const int m_damageCooldownMax = 500; // 受伤冷却时间（500毫秒，即半秒）

    // 冰块区域常量
    static const qreal ICE_LEFT;
    static const qreal ICE_RIGHT;
    static const qreal ICE_TOP;
    static const qreal ICE_BOTTOM;

    // 新增：冰块速度加成相关
    int m_iceSpeedBoostTimer = 0;
    const int m_iceSpeedBoostDuration = 500;
    bool m_iceSpeedBoostActive = false;
    bool m_iceSpeedBoostWasActive = false; // 新增：标记是否刚离开冰块
};

#endif // CHARACTER_H
