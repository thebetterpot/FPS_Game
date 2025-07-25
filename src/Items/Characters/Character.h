#ifndef CHARACTER_H
#define CHARACTER_H

#include <QObject>
#include <QGraphicsItem>
#include <QPixmap>

// 九宫格精灵图参数（144x144，3行3列）
const int FRAME_SIZE = 48;       // 单帧尺寸（48x48）
const int TOTAL_FRAMES = 3;      // 每行帧数
const int WALK_FRAME_COUNT = 3;  // 行走动画帧数

class Character : public QObject, public QGraphicsItem {
    Q_OBJECT
public:
    enum State {
        Idle,       // 静止
        WalkLeft,   // 向左走
        WalkRight,  // 向右走
        Crouch      // 下蹲
    };

    explicit Character(int playerId, const QString& spritePath, QGraphicsItem* parent = nullptr);

    // 动画更新
    void updateAnimation(int deltaTime);

    // 输入控制
    void setLeftPressed(bool pressed) { m_leftPressed = pressed; }
    void setRightPressed(bool pressed) { m_rightPressed = pressed; }
    void setCrouchPressed(bool pressed) { m_crouchPressed = pressed; }

    // 位置控制
    void setPosition(qreal x, qreal y) { setPos(x, y); }
    QPointF getPosition() const { return pos(); }

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    int m_playerId;               // 玩家ID
    QPixmap m_spriteSheet;        // 精灵图
    bool m_spriteLoaded;          // 精灵图加载状态

    // 动画状态
    State m_currentState;         // 当前状态
    int m_currentFrame;           // 当前帧索引
    int m_frameTimer;             // 帧计时器
    const int m_frameDelay = 120; // 帧间隔（毫秒）

    // 输入状态
    bool m_leftPressed;
    bool m_rightPressed;
    bool m_crouchPressed;

    // 移动参数
    const qreal m_moveSpeed = 4.0; // 移动速度

    // 更新状态（根据输入判断）
    void updateState();
    // 更新位置
    void updatePosition();
};

#endif // CHARACTER_H
