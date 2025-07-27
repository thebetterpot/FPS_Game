#include "Character.h"
#include <QGraphicsScene>
#include <QList>
#include <QDebug>
#include "../Weapon.h"
#include "../Projectile.h"
#include "../../Scenes/BattleScene.h"

// 初始化冰块区域常量
const qreal Character::ICE_LEFT = 190;
const qreal Character::ICE_RIGHT = 644;
const qreal Character::ICE_TOP = 340;
const qreal Character::ICE_BOTTOM = 422;

// 新增：草地区域常量定义
const qreal Character::GRASS_LEFT = 432;    // 草地左边界
const qreal Character::GRASS_RIGHT = 1082;  // 草地右边界
const qreal Character::GRASS_TOP = 180;     // 草地顶边界
const qreal Character::GRASS_BOTTOM = 230;  // 草地底边界

Character::Character(int playerId, const QString& spritePath, QObject *parent)
    : QObject(parent), m_playerId(playerId),
    m_ascendSpeed(-12.0), // 向上跳跃速度为12（Y轴向上为负方向）
    m_maxAscendTime(440), // 最长跳跃时间440毫秒
    m_ascendTime(0),
    m_inAir(false),
    m_gravity(0),
    m_currentHp(100), m_maxHp(100),
    m_attackCooldownMax(1000), m_attackDurationMax(300),
    m_damageCooldownMax(500),m_jumpCooldown(0),
    m_currentWeapon(nullptr), m_facingDirection(1) // 默认向右
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
    qreal charTop = oldPos.y();
    qreal charBottom = oldPos.y() + charHeight;
    qreal charLeft = oldPos.x();
    qreal charRight = oldPos.x() + charWidth;

    QPointF currentPos = pos();
    QRectF charBox = boundingRect().translated(currentPos); // 角色碰撞盒（随位置动态更新）

    // 定义冰块碰撞盒（完整矩形区域）
    QRectF iceBox(ICE_LEFT, ICE_TOP, ICE_RIGHT - ICE_LEFT, ICE_BOTTOM - ICE_TOP);

    QRectF grassBox(GRASS_LEFT, GRASS_TOP, GRASS_RIGHT - GRASS_LEFT, GRASS_BOTTOM - GRASS_TOP);

    // 状态变量
    bool wasOnIce = isOnIce();
    bool currentlyOnIce = false;
    bool iceCollisionOccurred = false;
    bool grassCollisionOccurred = false;
    bool wasInAir = m_inAir;

    // 1. 冰块碰撞检测与防穿透处理（核心）
    if (charBox.intersects(iceBox)) {
        iceCollisionOccurred = true;
        QRectF overlap = charBox.intersected(iceBox); // 计算重叠区域

        // 1.1 上方碰撞（角色落在冰块上）
        if (overlap.height() <= overlap.width() && charBox.top() < iceBox.top()) {
            // 将角色推到冰块上方（底部对齐冰块顶部）
            setY(iceBox.top() - charHeight);
            m_inAir = false;
            m_gravity = 0;
            m_ascendTime = 0;
            currentlyOnIce = true;
            qDebug() << "上方碰撞冰块，修正位置";
        }
        // 1.2 下方碰撞（角色顶到冰块底部）
        else if (overlap.height() <= overlap.width() && charBox.bottom() > iceBox.bottom()) {
            // 将角色推到冰块下方（顶部对齐冰块底部）
            setY(iceBox.bottom());
            m_ascendTime = m_maxAscendTime; // 强制结束跳跃
            m_gravity = 1.0; // 开始下落
            qDebug() << "下方碰撞冰块，修正位置";
        }
        // 1.3 左侧碰撞（角色撞冰块左边缘）
        else if (overlap.width() < overlap.height() && charBox.left() < iceBox.left()) {
            // 将角色推到冰块左侧（右边缘对齐冰块左边缘）
            setX(iceBox.left() - charWidth);
            qDebug() << "左侧碰撞冰块，修正位置";
        }
        // 1.4 右侧碰撞（角色撞冰块右边缘）
        else if (overlap.width() < overlap.height() && charBox.right() > iceBox.right()) {
            // 将角色推到冰块右侧（左边缘对齐冰块右边缘）
            setX(iceBox.right());
            qDebug() << "右侧碰撞冰块，修正位置";
        }

        // 重新计算碰撞盒（位置已修正）
        charBox = boundingRect().translated(pos());
    }

    // 2. 新增：草地碰撞检测与防穿透处理
    if (charBox.intersects(grassBox)) {
        grassCollisionOccurred = true;
        QRectF overlap = charBox.intersected(grassBox); // 计算重叠区域

        // 2.1 上方碰撞（角色落在草地上方）
        if (overlap.height() <= overlap.width() && charBox.top() < grassBox.top()) {
            setY(grassBox.top() - charHeight); // 推到草地上方
            m_inAir = false;
            m_gravity = 0;
            m_ascendTime = 0;
            qDebug() << "上方碰撞草地，修正位置";
        }
        // 2.2 下方碰撞（角色顶到草地底部）
        else if (overlap.height() <= overlap.width() && charBox.bottom() > grassBox.bottom()) {
            setY(grassBox.bottom()); // 推到草地下方
            m_ascendTime = m_maxAscendTime;
            m_gravity = 1.0;
            qDebug() << "下方碰撞草地，修正位置";
        }
        // 2.3 左侧碰撞（角色撞草地左边缘）
        else if (overlap.width() < overlap.height() && charBox.left() < grassBox.left()) {
            setX(grassBox.left() - charWidth); // 推到草地左侧
            qDebug() << "左侧碰撞草地，修正位置";
        }
        // 2.4 右侧碰撞（角色撞草地右边缘）
        else if (overlap.width() < overlap.height() && charBox.right() > grassBox.right()) {
            setX(grassBox.right()); // 推到草地右侧
            qDebug() << "右侧碰撞草地，修正位置";
        }

        // 重新计算碰撞盒（位置已修正）
        charBox = boundingRect().translated(pos());
    }

    // 2. 站立状态判定（基于修正后的位置）
    if (!iceCollisionOccurred) {
        currentlyOnIce = isOnIce();
    }

    // 3. 超出冰块范围坠落逻辑
    if (currentlyOnIce) {
        qreal charLeft = charBox.left();
        qreal charRight = charBox.right();
        // 角色完全超出冰块左右边界时坠落
        if (charRight <= iceBox.left() || charLeft >= iceBox.right()) {
            m_inAir = true;
            m_ascendTime = m_maxAscendTime;
            m_gravity = 0.5;
            currentlyOnIce = false;
            qDebug() << "超出冰块范围，开始坠落";
        }
    }

    // 4. 冰块加速buff逻辑
    if (currentlyOnIce) {
        m_iceSpeedBoostActive = true;
        m_iceSpeedBoostTimer = 0;
    } else if (m_iceSpeedBoostActive) {
        m_iceSpeedBoostTimer += deltaTime;
        if (m_iceSpeedBoostTimer >= m_iceSpeedBoostDuration) {
            m_iceSpeedBoostActive = false;
        }
    }

    if (m_inAir) {
        m_ascendTime += deltaTime;
        if (m_ascendTime >= m_maxAscendTime && m_gravity < 10) {
            m_gravity += 0.2;
        }
    }
    // 新增：落地时启动跳跃冷却
    else if (!m_inAir && wasInAir) { // wasInAir需要提前定义为上一帧的m_inAir状态
        m_jumpCooldown = m_jumpCooldownDuration; // 重置冷却为0.1秒
    }

    // 更新跳跃冷却时间
    if (m_jumpCooldown > 0) {
        m_jumpCooldown -= deltaTime;
        if (m_jumpCooldown < 0) m_jumpCooldown = 0;
    }

    // 修正跳跃触发条件：添加冷却检查
    if (!m_inAir && m_jumpPressed && !m_crouchPressed && !m_isAttacking && m_jumpCooldown <= 0) {
        m_inAir = true;
        m_ascendTime = 0;
        m_gravity = 0;
        qDebug() << "跳跃触发（冷却已结束）";
    }

    // 应用移动
    moveBy(velocity().x(), velocity().y());

    // 记录移动后的位置（关键：用移动后的位置判断是否超出）
    qreal newLeft = x();
    qreal newRight = x() + charWidth;

    QRectF newCharBox = boundingRect().translated(pos()); // 移动后的角色碰撞盒

    // 移动后再次检查：如果当前在冰块上但已超出范围，立即坠落
    if (currentlyOnIce) {
        if (newRight <= iceBox.left() || newLeft >= iceBox.right()) {
            m_inAir = true;
            m_ascendTime = m_maxAscendTime;
            m_gravity = 0.5;
            currentlyOnIce = false;
            m_iceSpeedBoostActive = false; // 立即取消加速
            qDebug() << "左右移动离开冰块范围，触发坠落";
        }
    }

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

    if (m_crouchPressed) {
        detectAndPickupWeapons();
    }

     updateAnimation(deltaTime);
}

// 检测并拾取武器
void Character::detectAndPickupWeapons() {
    if (!scene()) return;

    QRectF playerBounds = boundingRect().translated(pos());

    // 检查场景中所有武器
    foreach (QGraphicsItem* item, scene()->items()) {
        Weapon* weapon = dynamic_cast<Weapon*>(item);
        if (weapon && weapon->isPickable()) {
            QRectF weaponBounds = weapon->boundingRect().translated(weapon->pos());

            // 碰撞且按下蹲下键
            if (playerBounds.intersects(weaponBounds)) {
                pickUpWeapon(weapon);
                break;
            }
        }
    }
}

void Character::pickUpWeapon(Weapon* weapon) {
    if (!weapon || !weapon->isPickable()) return;

    // 添加到库存
    m_inventory.append(weapon);
    weapon->setPickedUp(true);

    // 如果没有当前武器,立即装备
    if (!m_currentWeapon) {
        m_currentWeapon = weapon;
        qDebug() << "装备了新武器";
    }
}

void Character::attackWithWeapon() {
    if (!m_currentWeapon || !m_currentWeapon->canAttack()) return;

    // 发射攻击信号
    QPointF attackPos = pos();
    // 根据面向方向调整攻击位置
    attackPos.setX(attackPos.x() + (m_facingDirection > 0 ? boundingRect().width() : 0));
    attackPos.setY(attackPos.y() + boundingRect().height() / 2);

    emit m_currentWeapon->attack(m_currentWeapon->getType(), attackPos, m_facingDirection);

    // 使用弹药并重置冷却
    m_currentWeapon->useAmmo();

    // 小刀不需要子弹,其他武器如果子弹用完则卸下
    if (m_currentWeapon->getAmmo() == 0 && m_currentWeapon->getType() != KNIFE) {
        m_inventory.removeOne(m_currentWeapon);
        m_currentWeapon = m_inventory.isEmpty() ? nullptr : m_inventory.first();
    }
}

Weapon* Character::getCurrentWeapon() const {
    return m_currentWeapon;
}

bool Character::isOnIce() const {
    QRectF charBox = boundingRect().translated(pos());
    QRectF iceBox(ICE_LEFT, ICE_TOP, ICE_RIGHT - ICE_LEFT, ICE_BOTTOM - ICE_TOP);

    // 角色底端与冰块顶端对齐，且部分在冰块上方
    bool yOnIce = qFuzzyCompare(charBox.bottom(), iceBox.top()) ||
                  (charBox.bottom() > iceBox.top() && charBox.bottom() < iceBox.top() + 2);
    bool xInRange = charBox.intersects(iceBox); // 角色与冰块有交集

    return yOnIce && xInRange;
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

    if (m_leftPressed) {
        m_facingDirection = -1;
    } else if (m_rightPressed) {
        m_facingDirection = 1;
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
