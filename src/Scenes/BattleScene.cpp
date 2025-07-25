#include "BattleScene.h"
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QStringLiteral>
#include <QElapsedTimer>
#include <QKeyEvent>
#include <QGraphicsRectItem>

BattleScene::BattleScene(QObject *parent)
    : QGraphicsScene(parent)
{
    setBackgroundBrush(Qt::black);
    setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    // 最底层白色背景（保持不变）
    QGraphicsRectItem* whiteBg = new QGraphicsRectItem(sceneRect());
    whiteBg->setBrush(Qt::white);
    whiteBg->setZValue(-2);
    addItem(whiteBg);

    // 加载地图背景（map_tu）并按需求缩放
    QGraphicsPixmapItem* mapTu = new QGraphicsPixmapItem(QPixmap(":/map_tu.png"));
    if (mapTu->pixmap().isNull()) {
        qWarning() << "[BattleScene] 背景素材map_tu加载失败！";
    } else {
        // 核心修改1：纵向高度不变，横向拉伸至场景宽度的90%
        int targetWidth = static_cast<int>(SCENE_WIDTH * 0.9); // 目标宽度：场景90%
        int originalHeight = mapTu->pixmap().height();        // 保持原图高度（720）

        // 拉伸图像：宽度=targetWidth，高度=originalHeight（忽略宽高比）
        QPixmap scaledBg = mapTu->pixmap().scaled(
            targetWidth,
            originalHeight,
            Qt::IgnoreAspectRatio,  // 允许拉伸（不保持原比例）
            Qt::SmoothTransformation
            );
        mapTu->setPixmap(scaledBg);

        // 核心修改2：置于场景中央（水平和垂直都居中）
        qreal xPos = (SCENE_WIDTH - scaledBg.width()) / 2;   // 水平居中
        qreal yPos = (SCENE_HEIGHT - scaledBg.height()) / 2; // 垂直居中（因高度=720，若场景高度=720则y=0）
        mapTu->setPos(xPos, yPos);

        mapTu->setZValue(-1);  // 保持在白色背景之上
        addItem(mapTu);
    }

    // 创建角色（确保m_player1是character1）
    m_player1 = new Link(1, QStringLiteral(":/character1.png"), nullptr);  // character1
    m_player2 = new Character(2, QStringLiteral(":/character2.png"), nullptr);

    // 设置角色初始位置和地面基准线
    const qreal groundY = SCENE_HEIGHT - 100;
    m_player1->setPos(200, groundY - 48);
    m_player2->setPos(1000, groundY - 48);
    m_player1->setGroundY(groundY);
    m_player2->setGroundY(groundY);

    // 设置显示层级
    m_player1->setZValue(2);
    m_player2->setZValue(2);

    // 添加角色到场景（关键：通过场景管理角色，而非构造函数传递父对象）
    addItem(m_player1);
    addItem(m_player2);

    // 初始化游戏循环
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, [this]() {
        static QElapsedTimer timer;
        if (!timer.isValid()) timer.start();
        update(timer.restart());
    });

    qDebug() << "[BattleScene] 初始化完成";
}


BattleScene::~BattleScene() {
    delete m_gameTimer;
}

void BattleScene::startLoop() {
    m_gameTimer->start(16); // 约60FPS
    qDebug() << "[BattleScene] 游戏循环启动";
}

void BattleScene::update(int deltaTime) {
    // 更新动画和物理
    m_player1->updateAnimation(deltaTime);
    m_player2->updateAnimation(deltaTime);
    m_player1->updatePhysics(deltaTime);
    m_player2->updatePhysics(deltaTime);

    // 更新位置
    m_player1->setPos(m_player1->pos() + m_player1->velocity());
    m_player2->setPos(m_player2->pos() + m_player2->velocity());

    // 边界检测
    auto clampPos = [this](QGraphicsItem* item) {
        QPointF pos = item->pos();
        pos.setX(qBound(0.0, pos.x(), (qreal)SCENE_WIDTH - 48));
        item->setPos(pos);
    };
    clampPos(m_player1);
    clampPos(m_player2);
}

// 新增：场景按键按下事件（直接控制角色状态）
void BattleScene::keyPressEvent(QKeyEvent *event) {
    // 控制character1（m_player1）：WASD
    if (m_player1) {
        switch (event->key()) {
        case Qt::Key_W: m_player1->setJumpPressed(true); break;
        case Qt::Key_S: m_player1->setCrouchPressed(true); break;
        case Qt::Key_A: m_player1->setLeftPressed(true); break;
        case Qt::Key_D: m_player1->setRightPressed(true); break;
        default: break;
        }
    }

    // 控制player2：方向键
    if (m_player2) {
        switch (event->key()) {
        case Qt::Key_Up: m_player2->setJumpPressed(true); break;
        case Qt::Key_Down: m_player2->setCrouchPressed(true); break;
        case Qt::Key_Left: m_player2->setLeftPressed(true); break;
        case Qt::Key_Right: m_player2->setRightPressed(true); break;
        default: break;
        }
    }

    QGraphicsScene::keyPressEvent(event);  // 传递事件
}

// 新增：场景按键释放事件
void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    // 控制character1（m_player1）：WASD释放
    if (m_player1) {
        switch (event->key()) {
        case Qt::Key_W: m_player1->setJumpPressed(false); break;
        case Qt::Key_S: m_player1->setCrouchPressed(false); break;
        case Qt::Key_A: m_player1->setLeftPressed(false); break;
        case Qt::Key_D: m_player1->setRightPressed(false); break;
        default: break;
        }
    }

    // 控制player2：方向键释放
    if (m_player2) {
        switch (event->key()) {
        case Qt::Key_Up: m_player2->setJumpPressed(false); break;
        case Qt::Key_Down: m_player2->setCrouchPressed(false); break;
        case Qt::Key_Left: m_player2->setLeftPressed(false); break;
        case Qt::Key_Right: m_player2->setRightPressed(false); break;
        default: break;
        }
    }

    QGraphicsScene::keyReleaseEvent(event);  // 传递事件
}
