#include "Character.h"
#include <QPainter>
#include <QDebug>

Character::Character(int playerId, QGraphicsItem *parent)
    : QGraphicsItem(parent), m_playerId(playerId),
    m_leftPressed(false), m_rightPressed(false),
    m_crouchPressed(false), m_jumpPressed(false),
    m_spriteSheet(":/character.png") // 加载自定义素材
{
    // 检查素材加载状态
    if (m_spriteSheet.isNull()) {
        qWarning() << "警告：角色素材加载失败！请检查资源路径是否正确";
    }
}

Character::~Character() {}

// 定义角色碰撞箱/绘制区域
QRectF Character::boundingRect() const {
    return QRectF(0, 0, 48, 48); // 角色尺寸（48x48像素）
}

// 绘制角色
void Character::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_spriteSheet.isNull()) {
        // 绘制自定义素材
        painter->drawPixmap(boundingRect().toRect(), m_spriteSheet);
    } else {
        // 素材加载失败时绘制红色方块作为占位
        painter->fillRect(boundingRect(), Qt::red);
    }
}

// 处理按键按下事件
void Character::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:  m_leftPressed = true;  break;
    case Qt::Key_Right: m_rightPressed = true; break;
    case Qt::Key_Down:  m_crouchPressed = true; break;
    case Qt::Key_Up:    m_jumpPressed = true;   break;
    default: QGraphicsItem::keyPressEvent(event);
    }
}

// 处理按键释放事件
void Character::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:  m_leftPressed = false;  break;
    case Qt::Key_Right: m_rightPressed = false; break;
    case Qt::Key_Down:  m_crouchPressed = false; break;
    case Qt::Key_Up:    m_jumpPressed = false;   break;
    default: QGraphicsItem::keyReleaseEvent(event);
    }
}

// 外部设置按键状态的函数
void Character::setLeftPressed(bool pressed)  { m_leftPressed = pressed; }
void Character::setRightPressed(bool pressed) { m_rightPressed = pressed; }
void Character::setCrouchPressed(bool pressed) { m_crouchPressed = pressed; }
void Character::setJumpPressed(bool pressed)  { m_jumpPressed = pressed; }

// 计算移动速度
QPointF Character::velocity() const {
    qreal xVelocity = 0;
    if (m_leftPressed)  xVelocity = -5;  // 左移速度
    if (m_rightPressed) xVelocity = 5;   // 右移速度
    return QPointF(xVelocity, 0);        // 暂时只处理水平移动
}
