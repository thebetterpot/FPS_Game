#include "BattleScene.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QKeyEvent>
#include <QDebug>
#include "../Items/Characters/Character.h"
#include "../Items/Characters/Link.h"
#include "../Items/Characters/HpBar.h"

BattleScene::BattleScene(QObject *parent)
    : QGraphicsScene(parent), m_player1(nullptr), m_player2(nullptr), m_timer(nullptr)
{
    setBackgroundBrush(Qt::black);
    setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    // 最底层白色背景
    QGraphicsRectItem* whiteBg = new QGraphicsRectItem(sceneRect());
    whiteBg->setBrush(Qt::white);
    whiteBg->setZValue(-2);
    addItem(whiteBg);

    // 地图背景（宽度90%场景，高度匹配场景）
    QGraphicsPixmapItem* mapTu = new QGraphicsPixmapItem(QPixmap(":/map_tu.png"));
    if (mapTu->pixmap().isNull()) {
        qWarning() << "[BattleScene] 背景素材map_tu加载失败！";
    } else {
        int targetWidth = static_cast<int>(SCENE_WIDTH * 0.9);
        int targetHeight = SCENE_HEIGHT;

        QPixmap scaledBg = mapTu->pixmap().scaled(
            targetWidth,
            targetHeight,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
            );
        mapTu->setPixmap(scaledBg);

        qreal xPos = (SCENE_WIDTH - targetWidth) / 2;
        qreal yPos = 0;
        mapTu->setPos(xPos, yPos);
        mapTu->setZValue(-1);
        addItem(mapTu);
    }

    // 土地碰撞区域（x:62~1210, y:614~708）
    QGraphicsRectItem* landArea = new QGraphicsRectItem(62, 614, 1210-62, 708-614);
    landArea->setBrush(QBrush(Qt::transparent));
    landArea->setPen(QPen(Qt::NoPen));
    landArea->setZValue(0);
    landArea->setData(0, "land"); // 标记为土地
    addItem(landArea);

    // 创建玩家1并确认血量
    m_player1 = new Link(1, ":/character1.png", this);
    m_player1->setPos(200, 566);
    m_player1->setHp(100);
    addItem(m_player1);
    qDebug() << "[BattleScene] 玩家1创建，血量：" << m_player1->getHp();

    // 创建玩家2并确认血量
    m_player2 = new Character(2, ":/character2.png", this);
    m_player2->setPos(1000, 566);
    m_player2->setHp(100);
    addItem(m_player2);
    qDebug() << "[BattleScene] 玩家2创建，血量：" << m_player2->getHp();

    // 创建并添加血条
    m_hpBar1 = new HpBar(m_player1, 1, 0, this);
    m_hpBar2 = new HpBar(m_player2, 2, 50, this); // 增大偏移至50像素
    addItem(m_hpBar1);
    addItem(m_hpBar2);

    // 调试：确认血条已添加到场景
    qDebug() << "[BattleScene] 血条数量：" << items().count(); // 应大于基础元素数量

    // 立即更新血条
    m_hpBar1->updateHpBar();
    m_hpBar2->updateHpBar();

    // 初始化定时器（约60FPS）
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &BattleScene::updateScene);
    m_timer->start(16);
}

BattleScene::~BattleScene() {
    delete m_player1;
    delete m_player2;
    // m_timer由父对象清理
}

void BattleScene::updateScene() {
    const int deltaTime = 16;
    if (m_player1) m_player1->updatePhysics(deltaTime);
    if (m_player2) m_player2->updatePhysics(deltaTime);
    if (m_player1) m_player1->updateAnimation(deltaTime);
    if (m_player2) m_player2->updateAnimation(deltaTime);

    if (m_hpBar1) m_hpBar1->updateHpBar();
    if (m_hpBar2) m_hpBar2->updateHpBar();
}

// 新增：键盘事件处理，检测攻击按键
void BattleScene::keyPressEvent(QKeyEvent *event) {
    if (!m_player1 || !m_player2) return;

    // 玩家1控制（WASD移动，J攻击）
    switch (event->key()) {
    case Qt::Key_W:
        m_player1->setJumpPressed(true);
        break;
    case Qt::Key_A:
        m_player1->setLeftPressed(true);
        break;
    case Qt::Key_S:
        m_player1->setCrouchPressed(true);
        break;
    case Qt::Key_D:
        m_player1->setRightPressed(true);
        break;
    case Qt::Key_J:  // 玩家1攻击键
        m_player1->setAttackPressed(true);
        break;
    }

    // 玩家2控制（方向键移动，1攻击）
    switch (event->key()) {
    case Qt::Key_Up:
        m_player2->setJumpPressed(true);
        break;
    case Qt::Key_Left:
        m_player2->setLeftPressed(true);
        break;
    case Qt::Key_Down:
        m_player2->setCrouchPressed(true);
        break;
    case Qt::Key_Right:
        m_player2->setRightPressed(true);
        break;
    case Qt::Key_1:  // 玩家2攻击键
        m_player2->setAttackPressed(true);
        break;
    }
}

// 新增：处理按键释放
void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    if (!m_player1 || !m_player2) return;

    // 玩家1控制
    switch (event->key()) {
    case Qt::Key_W:
        m_player1->setJumpPressed(false);
        break;
    case Qt::Key_A:
        m_player1->setLeftPressed(false);
        break;
    case Qt::Key_S:
        m_player1->setCrouchPressed(false);
        break;
    case Qt::Key_D:
        m_player1->setRightPressed(false);
        break;
    case Qt::Key_J:
        m_player1->setAttackPressed(false);
        break;
    }

    // 玩家2控制
    switch (event->key()) {
    case Qt::Key_Up:
        m_player2->setJumpPressed(false);
        break;
    case Qt::Key_Left:
        m_player2->setLeftPressed(false);
        break;
    case Qt::Key_Down:
        m_player2->setCrouchPressed(false);
        break;
    case Qt::Key_Right:
        m_player2->setRightPressed(false);
        break;
    case Qt::Key_1:
        m_player2->setAttackPressed(false);
        break;
    }
}
