#ifndef CHARACTER_H
#define CHARACTER_H

#include <QGraphicsItem>
#include <QKeyEvent>
#include <QPixmap>

class Character : public QGraphicsItem {
public:
    Character(int playerId, QGraphicsItem *parent = nullptr);
    ~Character() override;

    // 图形项必需实现的函数
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // 按键事件处理
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    // 外部设置按键状态
    void setLeftPressed(bool pressed);
    void setRightPressed(bool pressed);
    void setCrouchPressed(bool pressed);
    void setJumpPressed(bool pressed);

    // 获取移动速度
    QPointF velocity() const;

private:
    int m_playerId; // 玩家ID（1或2）
    // 按键状态
    bool m_leftPressed;
    bool m_rightPressed;
    bool m_crouchPressed;
    bool m_jumpPressed;
    QPixmap m_spriteSheet; // 角色素材
};

#endif // CHARACTER_H
