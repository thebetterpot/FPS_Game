#ifndef CHARACTER_H
#define CHARACTER_H

#include <QGraphicsItem>
#include <QKeyEvent>
#include <QPixmap>
#include <QObject>

class Character : public QObject, public QGraphicsItem {
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem) // 声明实现QGraphicsItem接口

public:
    explicit Character(int playerId, const QString& spritePath, QGraphicsItem *parent = nullptr);
    ~Character() override;

    // 绘图接口
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // 输入事件处理
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    // 状态控制接口
    void setLeftPressed(bool pressed);
    void setRightPressed(bool pressed);
    void setCrouchPressed(bool pressed);
    void setJumpPressed(bool pressed);

    // 物理与动画
    QPointF velocity() const;
    void updateAnimation(int deltaTime);
    void updatePhysics(int deltaTime);
    void setGroundY(qreal groundY);

protected:
    int m_playerId; // 玩家编号

private:
    // 输入状态
    bool m_leftPressed = false;
    bool m_rightPressed = false;
    bool m_crouchPressed = false; // 下蹲状态
    bool m_jumpPressed = false;

    // 精灵图与动画
    QPixmap m_spriteSheet;        // 144x144九宫格精灵图
    int m_currentFrame = 0;       // 当前帧索引（0-2）
    int m_currentRow = 0;         // 当前行索引（0-2）
    int m_frameTimer = 0;         // 帧切换计时器
    const int m_frameDelay = 100; // 帧切换间隔（毫秒）
    bool m_isMoving = false;      // 移动状态标记

    // 物理属性
    bool m_inAir = false;         // 空中状态
    qreal m_gravity = 0;          // 重力加速度
    qreal m_groundY = 0;          // 地面Y坐标
};

#endif // CHARACTER_H
