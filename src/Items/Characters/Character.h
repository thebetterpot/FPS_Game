#ifndef CHARACTER_H
#define CHARACTER_H

#include <QGraphicsItem>
#include <QKeyEvent>
#include <QPixmap>

class Character : public QGraphicsItem {
public:
    // 构造函数：接收玩家ID和精灵图路径
    Character(int playerId, const QString& spritePath, QGraphicsItem *parent = nullptr);
    ~Character() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void setLeftPressed(bool pressed);
    void setRightPressed(bool pressed);
    void setCrouchPressed(bool pressed);
    void setJumpPressed(bool pressed);

    QPointF velocity() const;

    // 更新动画帧
    void updateAnimation(int deltaTime);

private:
    int m_playerId;
    bool m_leftPressed;
    bool m_rightPressed;
    bool m_crouchPressed;
    bool m_jumpPressed;
    QPixmap m_spriteSheet;

    // 动画帧管理
    int m_currentFrame;       // 当前帧索引（0-2）
    int m_currentRow;         // 当前行索引（0：右走；1：左走；2：下蹲）
    int m_frameTimer;         // 帧切换计时器
    const int m_frameDelay;   // 帧切换延迟（毫秒）
    bool m_isMoving;          // 是否正在移动
};

#endif // CHARACTER_H
