#include "Character.h"
#include <QPainter>
#include <QDebug>
#include <QObject>

Character::Character(int playerId, const QString& spritePath, QGraphicsItem *parent)
    : QObject(nullptr),
    QGraphicsItem(parent),
    m_playerId(playerId),
    m_spriteSheet(spritePath)
{
    // 验证精灵图有效性
    if (m_spriteSheet.isNull()) {
        qWarning() << "[Character] 玩家" << m_playerId
                   << "精灵图加载失败: " << spritePath;
    } else if (m_spriteSheet.size() != QSize(144, 144)) {
        qWarning() << "[Character] 玩家" << m_playerId
                   << "精灵图尺寸错误（应为144x144，实际为"
                   << m_spriteSheet.width() << "x" << m_spriteSheet.height() << ")";
    }

    // 启用焦点（虽然按键由场景处理，但保留焦点属性以备扩展）
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

Character::~Character() {}

QRectF Character::boundingRect() const {
    return QRectF(0, 0, 48, 48); // 单帧尺寸（144÷3）
}

void Character::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_spriteSheet.isNull()) {
        // 精灵图加载失败时显示占位符
        painter->fillRect(boundingRect(), Qt::red);
        painter->setPen(Qt::white);
        painter->drawText(boundingRect(), Qt::AlignCenter, QString::number(m_playerId));
        return;
    }

    // 从九宫格精灵图中截取当前帧
    QRect frameRect(
        m_currentFrame * 48,  // X坐标：当前帧索引×单帧宽度
        m_currentRow * 48,    // Y坐标：当前行索引×单帧高度
        48, 48                // 单帧尺寸
        );
    painter->drawPixmap(boundingRect(), m_spriteSheet, frameRect);
}

// 禁用角色自身的按键按下事件（由场景统一处理）
void Character::keyPressEvent(QKeyEvent *event) {
    // 不处理任何按键，全部由BattleScene分发
    QGraphicsItem::keyPressEvent(event);
}

// 禁用角色自身的按键释放事件
void Character::keyReleaseEvent(QKeyEvent *event) {
    // 不处理任何按键，全部由BattleScene分发
    QGraphicsItem::keyReleaseEvent(event);
}

// 状态控制接口（供场景调用，设置移动状态）
void Character::setLeftPressed(bool pressed)  {
    m_leftPressed = pressed;
    m_isMoving = pressed || m_rightPressed;
}
void Character::setRightPressed(bool pressed) {
    m_rightPressed = pressed;
    m_isMoving = pressed || m_leftPressed;
}
void Character::setCrouchPressed(bool pressed) {
    // 若角色正在跳跃（空中），直接忽略下蹲按键
    if (m_inAir) {
        return; // 跳跃中不响应下蹲
    }

    // 仅在地面时处理下蹲状态（原有逻辑）
    if (pressed && !m_crouchPressed) {
        m_lastFacingRow = m_currentRow; // 记录下蹲前朝向
    } else if (!pressed && m_crouchPressed) {
        m_currentRow = m_lastFacingRow; // 恢复原朝向
    }
    m_crouchPressed = pressed;
}

void Character::setJumpPressed(bool pressed)  { m_jumpPressed = pressed; }

// 设置地面Y坐标（用于重力计算）
void Character::setGroundY(qreal groundY) {
    m_groundY = groundY;
}

// 获取当前速度（供场景更新位置）
QPointF Character::velocity() const {
    qreal xVel = 0;
    if (!m_crouchPressed) {
        xVel = m_leftPressed ? -5 : (m_rightPressed ? 5 : 0);
    }

    qreal yVel = 0;
    if (m_inAir) {
        // 上升阶段：匀速无重力
        if (m_ascendTime < m_maxAscendTime) {
            yVel = m_ascendSpeed; // 固定上升速度
        }
        // 下降阶段：受重力影响
        else {
            yVel = m_gravity; // 重力控制的下落速度
        }
    }
    // 移除此处对m_ascendTime的修改，移至updatePhysics()

    return QPointF(xVel, yVel);
}

// 动画更新逻辑（第三行全帧下蹲动画）
void Character::updateAnimation(int deltaTime) {
    if (m_crouchPressed) {
        m_currentRow = 2; // 下蹲时固定为第2行（不影响保存的m_lastFacingRow）

        // 下蹲动画帧更新（保持不变）
        m_frameTimer += deltaTime;
        if (m_frameTimer >= m_frameDelay) {
            m_currentFrame = (m_currentFrame + 1) % 3;
            m_frameTimer = 0;
        }

        m_isMoving = false;
    } else {
        // 非下蹲状态：根据移动方向更新朝向（正常逻辑）
        if (m_leftPressed) {
            m_currentRow = 0; // 朝左
        } else if (m_rightPressed) {
            m_currentRow = 1; // 朝右
        }
        // 静止时保持当前朝向（即最后保存的朝向）

        // 移动动画帧更新（保持不变）
        if (m_isMoving) {
            m_frameTimer += deltaTime;
            if (m_frameTimer >= m_frameDelay) {
                m_currentFrame = (m_currentFrame + 1) % 3;
                m_frameTimer = 0;
            }
        } else {
            m_currentFrame = 0;
        }
    }

    update();
}

// 物理更新（重力与地面碰撞）
void Character::updatePhysics(int deltaTime) {
    if (m_inAir) {
        // 更新上升时间（累加每帧时长）
        m_ascendTime += deltaTime;

        // 下降阶段才应用重力（上升阶段无重力）
        if (m_ascendTime >= m_maxAscendTime) {
            m_gravity += 0.5; // 重力加速度（可调整）
            if (m_gravity > 20) m_gravity = 20; // 最大下落速度
        }
    } else if (m_jumpPressed && !m_crouchPressed) {
        // 起跳时重置状态（在非const函数中修改成员变量）
        m_inAir = true;
        m_gravity = 0;
        m_ascendTime = 0; // 开始计时上升阶段
    }

    // 地面碰撞检测（保持不变）
    if (y() + 48 > m_groundY && m_inAir) {
        setY(m_groundY - 48);
        m_inAir = false;
        m_gravity = 0;
        m_ascendTime = 0; // 落地后重置上升计时器
    }
}
