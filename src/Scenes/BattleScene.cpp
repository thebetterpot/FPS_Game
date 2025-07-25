#include "BattleScene.h"
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <QStringLiteral>
#include <QElapsedTimer>

BattleScene::BattleScene(QObject *parent)
    : QGraphicsScene(parent)
{
    setBackgroundBrush(Qt::black);
    setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    // 加载背景（保持原逻辑）
    QGraphicsPixmapItem* mapTu = new QGraphicsPixmapItem(QPixmap(":/map_tu.png"));
    if (mapTu->pixmap().isNull()) {
        qWarning() << "[BattleScene] 背景素材map_tu加载失败！";
        QGraphicsRectItem* bgRect = new QGraphicsRectItem(sceneRect());
        bgRect->setBrush(QBrush(QColor(40, 40, 40)));
        bgRect->setZValue(-1);
        addItem(bgRect);
    } else {
        QPixmap scaledBg = mapTu->pixmap().scaledToWidth(SCENE_WIDTH, Qt::SmoothTransformation);
        mapTu->setPixmap(scaledBg);
        mapTu->setPos(0, SCENE_HEIGHT - scaledBg.height());
        mapTu->setZValue(-1);
        addItem(mapTu);
    }

    // 核心修复：角色父对象设为nullptr（避免QGraphicsScene*转QGraphicsItem*的类型错误）
    // 角色通过addItem添加到场景，由场景管理生命周期
    m_player1 = new Link(1, QStringLiteral(":/character1.png"), nullptr);
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
