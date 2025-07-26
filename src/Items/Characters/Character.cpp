#include "Character.h"
#include <QGraphicsScene>
#include <QList>
#include <QDebug>
#include "../../Scenes/BattleScene.h"

// 初始化冰块区域常量
const qreal Character::ICE_LEFT = 190;
const qreal Character::ICE_RIGHT = 644;
const qreal Character::ICE_TOP = 340;
const qreal Character::ICE_BOTTOM = 422;

Character::Character(int playerId, const QString& spritePath, QObject *parent)
    : QObject(parent), m_playerId(playerId),
    m_ascendSpeed(-12.0), // 向上跳跃速度为12（Y轴向上为负方向）
    m_maxAscendTime(400), // 最长跳跃时间400毫秒
    m_ascendTime(0),
    m_inAir(false),
    m_gravity(0),
    m_currentHp(100), m_maxHp(100),
    m_attackCooldownMax(1000), m_attackDurationMax(300),
    m_damageCooldownMax(500)
{
    // 加载角色精灵图
    m_sprite = QPixmap(spritePath);
    if (m_sprite.isNull()) {
        qWarning() << "[Character] 精灵图加载失败:" << spritePath;
    }

    // 初始化状态
    m_currentFrame = 0;
    m_currentRow = 1; // 默认朝右
    m_frameDelay = 100;
    m_frameTimer = 0;
    m_isMoving = false;
    m_leftPressed = false;
    m_rightPressed = false;
    m_jumpPressed = false;
    m_crouchPressed = false;
    m_inAir = false;
    m_gravity = 0;
    m_ascendTime = 0;
    m_lastFacingRow = 1;
    m_isAttacking = false;
    m_attackPressed = false;
    m_attackCooldown = 0;
    m_attackDuration = 0;
    m_damageCooldown = 0;
}

Character::~Character() {}

QRectF Character::boundingRect() const {
    // 角色碰撞盒（宽40，高48）
    return QRectF(0, 0, 40, 48);
}

void Character::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 精灵图为空时绘制红色占位矩形
    if (m_sprite.isNull()) {
        painter->fillRect(boundingRect(), Qt::red);
        return;
    }

    // 计算精灵图分帧区域
    int frameWidth = m_sprite.width() / 3;
    int frameHeight = m_sprite.height() / 3;

    // 绘制当前帧（攻击状态可在此处切换攻击帧）
    painter->drawPixmap(
        boundingRect(),
        m_sprite,
        QRect(m_currentFrame * frameWidth, m_currentRow * frameHeight, frameWidth, frameHeight)
        );

    // 调试：绘制攻击判定区域（绿色半透明）
    if (m_isAttacking) {
        painter->setPen(QPen(Qt::green, 1));
        painter->setBrush(QBrush(QColor(0, 255, 0, 50)));
        painter->drawRect(attackHitbox().translated(-pos())); // 转换为本地坐标
    }
}

QPointF Character::velocity() const {
    qreal xVel = 0;
    qreal baseSpeed = m_iceSpeedBoostActive ? 10 : 5;

    if (!m_crouchPressed && !m_isAttacking) {
        xVel = m_leftPressed ? -baseSpeed : (m_rightPressed ? baseSpeed : 0);
        m_isMoving = (xVel != 0);
    } else {
        m_isMoving = false;
    }

    qreal yVel = 0;
    if (m_inAir) {
        // 跳跃阶段：前400毫秒上升（速度12），之后进入下落（受重力）
        if (m_ascendTime < m_maxAscendTime) {
            yVel = m_ascendSpeed; // 上升速度保持12
        } else {
            yVel = m_gravity; // 下落阶段（重力累加）
        }
    }
    return QPointF(xVel, yVel);
}

void Character::updatePhysics(int deltaTime) {
    QPointF oldPos = pos();
    qreal charWidth = boundingRect().width();
    qreal charHeight = boundingRect().height();
    qreal charTop = oldPos.y(); // 角色顶端Y坐标
    qreal charBottom = oldPos.y() + charHeight; // 角色底端Y坐标

    // 关键修复：在使用前声明charRect变量
    QRectF charRect = boundingRect().translated(oldPos); // 角色碰撞矩形（基于旧位置）

    QRectF iceRect(ICE_LEFT, ICE_TOP, ICE_RIGHT - ICE_LEFT, ICE_BOTTOM - ICE_TOP);
    bool currentlyOnIce = isOnIce();

    // 加速buff逻辑（保持不变）
    if (currentlyOnIce) {
        m_iceSpeedBoostActive = true;
        m_iceSpeedBoostTimer = 0;
        if (!m_iceSpeedBoostWasActive) {
            qDebug() << "[冰块] 激活加速buff";
            m_iceSpeedBoostWasActive = true;
        }
    } else if (m_iceSpeedBoostActive) {
        m_iceSpeedBoostTimer += deltaTime;
        if (m_iceSpeedBoostTimer >= m_iceSpeedBoostDuration) {
            m_iceSpeedBoostActive = false;
            m_iceSpeedBoostTimer = 0;
            m_iceSpeedBoostWasActive = false;
        }
    }

    // 重力与跳跃逻辑（保持不变）
    if (m_inAir) {
        m_ascendTime += deltaTime;
        if (m_ascendTime >= m_maxAscendTime) {
            m_gravity += 0.2;
            if (m_gravity > 10) m_gravity = 10;
        }
    } else if (m_jumpPressed && !m_crouchPressed && !m_isAttacking) {
        m_inAir = true;
        m_gravity = 0;
        m_ascendTime = 0;
        qDebug() << "[跳跃] 开始跳跃（速度12，最长400ms）";
    }

    // 冰块碰撞检测（现在charRect已声明，可以正常使用）
    if (charTop <= iceRect.bottom() && charTop + velocity().y() > iceRect.bottom() &&
        oldPos.x() + charWidth > iceRect.left() && oldPos.x() < iceRect.right()) {
        m_ascendTime = m_maxAscendTime;
        m_gravity = 0.5;
        qDebug() << "[冰块碰撞] 顶端顶到冰块，结束跳跃，进入下落";
    }
    else if (charBottom <= iceRect.top() && charBottom + velocity().y() > iceRect.top() &&
             oldPos.x() + charWidth > iceRect.left() && oldPos.x() < iceRect.right()) {
        setY(iceRect.top() - charHeight);
        m_inAir = false;
        m_gravity = 0;
        m_ascendTime = 0;
    }
    // 左右碰撞检测（使用已声明的charRect）
    else if (charRect.intersects(iceRect)) {
        if (oldPos.x() + charWidth <= iceRect.left() && oldPos.x() + charWidth + velocity().x() > iceRect.left()) {
            setX(iceRect.left() - charWidth);
        } else if (oldPos.x() >= iceRect.right() && oldPos.x() + velocity().x() < iceRect.right()) {
            setX(iceRect.right());
        }
    }

    // 应用移动
    moveBy(velocity().x(), velocity().y());


    // 1. 地面范围检测（完全超出时下坠）
    if (!m_inAir) {
        qreal charWidth = boundingRect().width();
        qreal groundMinX = 62;
        qreal groundMaxX = 1210;

        bool fullyLeftOfGround = (x() + charWidth) < groundMinX;
        bool fullyRightOfGround = x() > groundMaxX;

        if (fullyLeftOfGround || fullyRightOfGround) {
            m_inAir = true;
            m_gravity = 0.5;
            m_ascendTime = m_maxAscendTime;
            qDebug() << "[Character] 玩家" << m_playerId << "完全超出地面，触发下坠";
        }
    }

    // 2. 重力与跳跃逻辑
    if (m_inAir) {
        m_ascendTime += deltaTime;
        if (m_ascendTime >= m_maxAscendTime) {
            m_gravity += 0.2;
            if (m_gravity > 10) m_gravity = 10; // 限制最大下落速度
        }
    } else if (m_jumpPressed && !m_crouchPressed && !m_isAttacking) {
        m_inAir = true;
        m_gravity = 0;
        m_ascendTime = 0;
    }

    // 3. 应用移动
    moveBy(velocity().x(), velocity().y());

    // 4. 场景边界限制
    charWidth = boundingRect().width();
    charHeight = boundingRect().height();

    // 左右边界
    if (x() < 0) {
        setX(0);
    } else if (x() + charWidth > BattleScene::SCENE_WIDTH) {
        setX(BattleScene::SCENE_WIDTH - charWidth);
    }

    // 上边界
    if (y() < 0) {
        setY(0);
        if (m_ascendTime < m_maxAscendTime) {
            m_ascendTime = m_maxAscendTime; // 强制进入下降阶段
        }
    }

    // 下边界（坠崖死亡）
    if (y() + charHeight > BattleScene::SCENE_HEIGHT) {
        setHp(0);
        qDebug() << "[Character] 玩家" << m_playerId << "坠崖死亡！";
    }

    // 5. 受伤冷却计时器更新
    if (m_damageCooldown > 0) {
        m_damageCooldown -= deltaTime;
    }

    // 6. 攻击逻辑处理
    if (m_attackCooldown > 0) {
        m_attackCooldown -= deltaTime;
    }

    if (m_isAttacking) {
        m_attackDuration += deltaTime;
        if (m_attackDuration >= m_attackDurationMax) {
            m_isAttacking = false;
            m_attackDuration = 0;
        }
    } else if (m_attackPressed && m_attackCooldown <= 0 && !m_inAir) {
        attack(); // 地面状态才能攻击
    }

    // 7. 攻击碰撞检测
    if (m_isAttacking) {
        QList<QGraphicsItem*> collidingItems = scene()->collidingItems(this);
        foreach (QGraphicsItem* item, collidingItems) {
            Character* otherPlayer = dynamic_cast<Character*>(item);
            if (otherPlayer && otherPlayer->m_playerId != m_playerId) {
                if (attackHitbox().intersects(otherPlayer->boundingRect().translated(otherPlayer->pos()))) {
                    otherPlayer->takeDamage(5); // 每次攻击扣5血
                    m_attackPressed = false; // 防止一次攻击多次判定
                }
            }
        }
    }

    // 8. 土地碰撞检测
    QList<QGraphicsItem*> collidingItems = scene()->collidingItems(this);
    foreach (QGraphicsItem* item, collidingItems) {
        if (item->data(0) == "land") {
            QRectF landRect = item->boundingRect().translated(item->pos());
            QRectF charRect = boundingRect().translated(pos());

            // 底部碰撞土地（落地）
            if (oldPos.y() + charRect.height() <= landRect.top() && charRect.bottom() > landRect.top()) {
                setY(landRect.top() - charRect.height());
                m_inAir = false;
                m_gravity = 0;
                m_ascendTime = 0;
            }
            // 顶部碰撞土地
            else if (oldPos.y() >= landRect.bottom() && charRect.top() < landRect.bottom()) {
                setY(oldPos.y());
            }
            // 左侧碰撞土地
            else if (oldPos.x() + charRect.width() <= landRect.left() && charRect.right() > landRect.left()) {
                setX(oldPos.x());
            }
            // 右侧碰撞土地
            else if (oldPos.x() >= landRect.right() && charRect.left() < landRect.right()) {
                setX(oldPos.x());
            }
        }
    }
}

bool Character::isOnIce() const {
    qreal charBottom = y() + boundingRect().height();
    qreal charLeft = x();
    qreal charRight = x() + boundingRect().width();

    bool yCondition = (charBottom >= ICE_TOP - 1) && (charBottom <= ICE_TOP + 1);
    bool xCondition = (charLeft >= ICE_LEFT) && (charRight <= ICE_RIGHT);

    return yCondition && xCondition;
}

void Character::updateAnimation(int deltaTime) {
    if (m_crouchPressed) {
        m_currentRow = 2; // 下蹲行
        m_frameTimer += deltaTime;
        if (m_frameTimer >= m_frameDelay) {
            m_currentFrame = (m_currentFrame + 1) % 3;
            m_frameTimer = 0;
        }
        m_isMoving = false;
    } else if (m_isAttacking) {
        // 攻击动画（可在此处设置攻击帧）
        m_currentFrame = 1; // 固定攻击帧
    } else {
        // 更新朝向
        if (m_leftPressed) {
            m_currentRow = 0; // 朝左
        } else if (m_rightPressed) {
            m_currentRow = 1; // 朝右
        }

        // 更新移动动画帧
        if (m_isMoving) {
            m_frameTimer += deltaTime;
            if (m_frameTimer >= m_frameDelay) {
                m_currentFrame = (m_currentFrame + 1) % 3;
                m_frameTimer = 0;
            }
        } else {
            m_currentFrame = 0; // 静止帧
        }
    }
    update(); // 触发重绘
}

// 攻击相关方法
void Character::attack() {
    m_isAttacking = true;
    m_attackDuration = 0;
    m_attackCooldown = m_attackCooldownMax;
    qDebug() << "[攻击] 玩家" << m_playerId << "使用拳头攻击";
}

void Character::takeDamage(int damage) {
    if (m_damageCooldown > 0) {
        qDebug() << "[伤害] 玩家" << m_playerId << "处于受伤冷却中，伤害无效";
        return;
    }

    setHp(getHp() - damage);
    qDebug() << "[伤害] 玩家" << m_playerId << "受到" << damage << "点伤害，剩余血量：" << getHp();
    m_damageCooldown = m_damageCooldownMax; // 半秒内不能再次受伤
}

bool Character::isAttacking() const {
    return m_isAttacking;
}

QRectF Character::attackHitbox() const {
    qreal width = 30;  // 攻击范围宽度
    qreal height = 30; // 攻击范围高度
    QRectF hitbox;

    // 根据朝向确定攻击方向
    if (m_currentRow == 1) { // 朝右
        hitbox = QRectF(boundingRect().width(), boundingRect().height()/4, width, height);
    } else { // 朝左
        hitbox = QRectF(-width, boundingRect().height()/4, width, height);
    }

    return hitbox.translated(pos());
}

// 控制状态设置
void Character::setLeftPressed(bool pressed) {
    if (!m_crouchPressed && !m_isAttacking) {
        m_leftPressed = pressed;
    }
}

void Character::setRightPressed(bool pressed) {
    if (!m_crouchPressed && !m_isAttacking) {
        m_rightPressed = pressed;
    }
}

void Character::setJumpPressed(bool pressed) {
    m_jumpPressed = pressed;
}

void Character::setCrouchPressed(bool pressed) {
    if (m_inAir || m_isAttacking) return;

    if (pressed && !m_crouchPressed) {
        m_lastFacingRow = m_currentRow;
    } else if (!pressed && m_crouchPressed) {
        m_currentRow = m_lastFacingRow;
    }
    m_crouchPressed = pressed;
}

void Character::setAttackPressed(bool pressed) {
    m_attackPressed = pressed;
}

// 血量操作
void Character::setHp(int hp) {
    if (hp < 0) hp = 0;
    if (hp > m_maxHp) hp = m_maxHp;
    m_currentHp = hp;
}

int Character::getHp() const {
    return m_currentHp;
}

int Character::getMaxHp() const {
    return m_maxHp;
}
