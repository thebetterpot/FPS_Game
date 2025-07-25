#include "Character.h"
#include <QPainter>
#include <QDebug>

Character::Character(int playerId, const QString& spritePath, QGraphicsItem* parent)
    : QObject(nullptr), QGraphicsItem(parent), m_playerId(playerId),
    m_spriteLoaded(false), m_currentState(Idle), m_currentFrame(0), m_frameTimer(0),
    m_leftPressed(false), m_rightPressed(false), m_crouchPressed(false) {

    // 加载九宫格精灵图（144x144）
    if (m_spriteSheet.load(spritePath)) {
        // 验证精灵图尺寸
        if (m_spriteSheet.width() == FRAME_SIZE * TOTAL_FRAMES &&
            m_spriteSheet.height() == FRAME_SIZE * TOTAL_FRAMES) {
            m_spriteLoaded = true;
            qDebug() << "[Character] 玩家" << m_playerId << "精灵图加载成功（144x144九宫格）";
        } else {
            qWarning() << "[Character] 精灵图尺寸错误！应为144x144，实际为"
                       << m_spriteSheet.size();
        }
    } else {
        qWarning() << "[Character] 精灵图加载失败！路径：" << spritePath;
    }

    // 初始缩放2倍（48x48 → 96x96，便于观察）
    setScale(2.0);
}

QRectF Character::boundingRect() const {
    return QRectF(0, 0, FRAME_SIZE, FRAME_SIZE);
}

void Character::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_spriteLoaded) {
        // 加载失败时绘制占位方块
        painter->fillRect(boundingRect(), m_playerId == 1 ? Qt::blue : Qt::red);
        return;
    }

    // 计算当前帧在精灵图中的位置（核心逻辑）
    int row = 0;
    int col = m_currentFrame;

    switch (m_currentState) {
    case WalkLeft:  row = 0; break; // 第一行：朝左动作
    case WalkRight: row = 1; break; // 第二行：朝右动作
    case Crouch:    row = 2; col = 1; break; // 第三行中间：下蹲
    case Idle:      row = m_currentState == WalkLeft ? 0 : 1; col = 0; break; // 静止帧
    }

    // 提取当前帧
    QRect frameRect(col * FRAME_SIZE, row * FRAME_SIZE, FRAME_SIZE, FRAME_SIZE);
    painter->drawPixmap(boundingRect(), m_spriteSheet, frameRect);
}

void Character::updateState() {
    // 下蹲优先级最高
    if (m_crouchPressed) {
        m_currentState = Crouch;
        return;
    }

    // 移动状态判断
    if (m_leftPressed && !m_rightPressed) {
        m_currentState = WalkLeft;
    } else if (m_rightPressed && !m_leftPressed) {
        m_currentState = WalkRight;
    } else {
        m_currentState = Idle; // 静止
    }
}

void Character::updatePosition() {
    if (m_currentState == Crouch) return; // 下蹲时不能移动

    qreal x = pos().x();
    if (m_currentState == WalkLeft) {
        x -= m_moveSpeed;
    } else if (m_currentState == WalkRight) {
        x += m_moveSpeed;
    }

    // 边界限制（1280x720场景）
    x = qBound(0.0, x, 1280.0 - FRAME_SIZE * scale());
    setPos(x, pos().y());
}

void Character::updateAnimation(int deltaTime) {
    updateState();
    updatePosition();

    // 只有移动状态需要切换帧
    if (m_currentState != WalkLeft && m_currentState != WalkRight) {
        m_currentFrame = 0;
        return;
    }

    // 帧计时器累加
    m_frameTimer += deltaTime;
    if (m_frameTimer >= m_frameDelay) {
        // 循环切换行走帧（0→1→2→0）
        m_currentFrame = (m_currentFrame + 1) % WALK_FRAME_COUNT;
        m_frameTimer = 0;
    }

    // 触发重绘
    update();
}
