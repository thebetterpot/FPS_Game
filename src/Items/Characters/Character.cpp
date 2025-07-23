#include "Character.h"
#include <QPainter>
#include <QDebug>

// 构造函数：初始化角色属性和动画参数
Character::Character(int playerId, const QString& spritePath, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_playerId(playerId),
    m_leftPressed(false), m_rightPressed(false),
    m_crouchPressed(false), m_jumpPressed(false),
    m_spriteSheet(spritePath),
    m_currentFrame(0),
    m_currentRow(0),
    m_frameTimer(0),
    m_frameDelay(100),  // 每100ms切换一帧（控制动画速度）
    m_isMoving(false)
{
    // 素材加载校验
    if (m_spriteSheet.isNull()) {
        qWarning() << "角色" << playerId << "素材加载失败！路径：" << spritePath;
    } else if (m_spriteSheet.size() != QSize(144, 144)) {
        qWarning() << "角色" << playerId << "素材尺寸错误（应为144x144，实际为"
                   << m_spriteSheet.width() << "x" << m_spriteSheet.height() << "）";
    }
}

Character::~Character() {}

// 角色碰撞盒（与单帧尺寸一致）
QRectF Character::boundingRect() const {
    return QRectF(0, 0, 48, 48);  // 144÷3=48，单帧尺寸
}

// 绘制当前动画帧
void Character::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_spriteSheet.isNull()) {
        // 素材加载失败时绘制占位方块
        painter->fillRect(boundingRect(), Qt::red);
        painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(m_playerId));
        return;
    }

    // 从精灵图中截取当前帧（九宫格坐标计算）
    QRect frameRect(
        m_currentFrame * 48,  // X坐标：当前帧索引×单帧宽度
        m_currentRow * 48,    // Y坐标：当前行索引×单帧高度
        48, 48                // 单帧尺寸
        );
    painter->drawPixmap(boundingRect().toRect(), m_spriteSheet, frameRect);
}

// 按键按下事件处理
void Character::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:
        m_leftPressed = true;
        m_isMoving = true;
        break;
    case Qt::Key_Right:
        m_rightPressed = true;
        m_isMoving = true;
        break;
    case Qt::Key_Down:
        m_crouchPressed = true;
        break;
    case Qt::Key_Up:
        m_jumpPressed = true;
        break;
    default:
        QGraphicsItem::keyPressEvent(event);
    }
}

// 按键释放事件处理
void Character::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:
        m_leftPressed = false;
        m_isMoving = m_rightPressed;  // 只有右移键按下时才保持移动状态
        break;
    case Qt::Key_Right:
        m_rightPressed = false;
        m_isMoving = m_leftPressed;   // 只有左移键按下时才保持移动状态
        break;
    case Qt::Key_Down:
        m_crouchPressed = false;
        break;
    case Qt::Key_Up:
        m_jumpPressed = false;
        break;
    default:
        QGraphicsItem::keyReleaseEvent(event);
    }
}

// 外部控制接口：设置按键状态
void Character::setLeftPressed(bool pressed)  {
    m_leftPressed = pressed;
    m_isMoving = pressed || m_rightPressed;
}
void Character::setRightPressed(bool pressed) {
    m_rightPressed = pressed;
    m_isMoving = pressed || m_leftPressed;
}
void Character::setCrouchPressed(bool pressed) { m_crouchPressed = pressed; }
void Character::setJumpPressed(bool pressed)  { m_jumpPressed = pressed; }

// 计算移动速度（下蹲时无法移动）
QPointF Character::velocity() const {
    qreal xVelocity = 0;
    if (!m_crouchPressed) {  // 下蹲状态禁用移动
        if (m_leftPressed)  xVelocity = -5;
        if (m_rightPressed) xVelocity = 5;
    }
    return QPointF(xVelocity, 0);
}

// 动画帧更新逻辑（核心）
void Character::updateAnimation(int deltaTime) {
    // 1. 确定当前动画行（优先级：下蹲 > 移动）
    if (m_crouchPressed) {
        m_currentRow = 2;  // 第三行：下蹲动画
        m_currentFrame = 0;  // 下蹲状态固定第一帧
    } else {
        // 移动方向对应的动画行（修复方向相反问题）
        if (m_leftPressed) {
            m_currentRow = 0;  // 第一行：左走动画
        } else if (m_rightPressed) {
            m_currentRow = 1;  // 第二行：右走动画
        }
    }

    // 2. 移动时更新帧（静止时复位）
    if (m_isMoving && !m_crouchPressed) {
        m_frameTimer += deltaTime;  // 累计帧间隔时间
        if (m_frameTimer >= m_frameDelay) {
            m_currentFrame = (m_currentFrame + 1) % 3;  // 循环切换3帧（0→1→2→0）
            m_frameTimer = 0;  // 重置计时器
        }
    } else if (!m_crouchPressed) {
        m_currentFrame = 0;  // 静止时回到第一帧
    }

    update();  // 触发重绘
}
