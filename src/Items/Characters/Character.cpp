#include "Character.h"
#include <QPainter>
#include <QDebug>
#include <QObject>

// 核心修复：完全移除QGraphicsItem*到QObject*的转换，两者父对象独立管理
Character::Character(int playerId, const QString& spritePath, QGraphicsItem *parent)
    : QObject(nullptr),  // QObject父对象独立设置为nullptr（不依赖QGraphicsItem）
    QGraphicsItem(parent),  // QGraphicsItem使用自身的parent
    m_playerId(playerId),
    m_spriteSheet(spritePath)
{
    // 彻底删除所有qobject_cast相关代码（因QGraphicsItem不是QObject派生类，转换无效）

    if (m_spriteSheet.isNull()) {
        qWarning() << "[Character] 玩家" << m_playerId
                   << "精灵图加载失败: " << spritePath;
    } else if (m_spriteSheet.size() != QSize(144, 144)) {
        qWarning() << "[Character] 玩家" << m_playerId
                   << "精灵图尺寸错误（应为144x144）";
    }

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

// 其余函数实现保持不变...

Character::~Character() {}

QRectF Character::boundingRect() const {
    return QRectF(0, 0, 48, 48); // 单帧尺寸（144÷3）
}

void Character::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_spriteSheet.isNull()) {
        // 加载失败时显示占位符
        painter->fillRect(boundingRect(), Qt::red);
        painter->setPen(Qt::white);
        painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(m_playerId));
        return;
    }

    // 绘制当前帧（九宫格截取）
    QRect frameRect(
        m_currentFrame * 48,  // X坐标：帧索引×单帧宽度
        m_currentRow * 48,    // Y坐标：行索引×单帧高度
        48, 48
        );
    painter->drawPixmap(boundingRect(), m_spriteSheet, frameRect);
}

// 键盘按下事件
void Character::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:  m_leftPressed = true;  m_isMoving = true; break;
    case Qt::Key_Right: m_rightPressed = true; m_isMoving = true; break;
    case Qt::Key_Down:  // 玩家1下蹲键
    case Qt::Key_S:     // 玩家2下蹲键
        m_crouchPressed = true;
        break;
    case Qt::Key_Up:    // 玩家1跳跃键
    case Qt::Key_W:     // 玩家2跳跃键
        m_jumpPressed = true;
        break;
    default: QGraphicsItem::keyPressEvent(event);
    }
}

// 键盘释放事件
void Character::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:  m_leftPressed = false; m_isMoving = m_rightPressed; break;
    case Qt::Key_Right: m_rightPressed = false; m_isMoving = m_leftPressed; break;
    case Qt::Key_Down:
    case Qt::Key_S:     m_crouchPressed = false; break;
    case Qt::Key_Up:
    case Qt::Key_W:     m_jumpPressed = false; break;
    default: QGraphicsItem::keyReleaseEvent(event);
    }
}

// 外部状态控制接口
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

// 设置地面Y坐标
void Character::setGroundY(qreal groundY) {
    m_groundY = groundY;
}

// 计算移动速度
QPointF Character::velocity() const {
    qreal xVel = 0;
    if (!m_crouchPressed) { // 下蹲时禁止水平移动
        xVel = m_leftPressed ? -5 : (m_rightPressed ? 5 : 0);
    }

    qreal yVel = 0;
    if (m_inAir) {
        yVel = m_gravity; // 空中受重力影响
    } else if (m_jumpPressed && !m_crouchPressed) {
        yVel = -15; // 跳跃初速度
    }

    return QPointF(xVel, yVel);
}

// 核心：动画更新（第三行全帧下蹲动画）
void Character::updateAnimation(int deltaTime) {
    if (m_crouchPressed) {
        m_currentRow = 2; // 第三行：下蹲动画行

        // 循环第三行全部3帧（0→1→2→0）
        m_frameTimer += deltaTime;
        if (m_frameTimer >= m_frameDelay) {
            m_currentFrame = (m_currentFrame + 1) % 3;
            m_frameTimer = 0;
        }

        m_isMoving = false; // 下蹲时禁止移动
    } else {
        // 移动方向行（左移0行，右移1行）
        m_currentRow = m_leftPressed ? 0 : (m_rightPressed ? 1 : m_currentRow);

        // 移动时切换帧，静止时复位
        if (m_isMoving) {
            m_frameTimer += deltaTime;
            if (m_frameTimer >= m_frameDelay) {
                m_currentFrame = (m_currentFrame + 1) % 3;
                m_frameTimer = 0;
            }
        } else {
            m_currentFrame = 0; // 静止时第一帧
        }
    }

    update(); // 触发重绘
}

// 物理更新（重力与地面碰撞）
void Character::updatePhysics(int deltaTime) {
    Q_UNUSED(deltaTime);

    if (m_inAir) {
        m_gravity += 0.5; // 重力加速度
        if (m_gravity > 20) m_gravity = 20; // 限制最大下落速度
    } else if (m_jumpPressed && !m_crouchPressed) {
        m_inAir = true; // 从地面起跳
        m_gravity = 0;
    }

    // 地面碰撞检测
    if (y() + 48 > m_groundY && m_inAir) {
        setY(m_groundY - 48); // 固定在地面
        m_inAir = false;
        m_gravity = 0;
    }
}
