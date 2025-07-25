#include "BattleScene.h"
#include <QKeyEvent>
#include <QDebug>

BattleScene::BattleScene(QObject* parent) : QGraphicsScene(parent) {
    // 设置场景尺寸
    setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);

    // 添加背景地图（最底层）
    Battlefield* battlefield = new Battlefield();
    battlefield->setZValue(-1);
    addItem(battlefield);

    // 创建玩家1（左侧）
    m_player1 = new Character(1, ":/assets/character1.png");
    m_player1->setPosition(200, 300); // 初始位置
    m_player1->setZValue(1); // 角色在地图上方
    addItem(m_player1);

    // 创建玩家2（右侧）
    m_player2 = new Character(2, ":/assets/character2.png");
    m_player2->setPosition(1000, 300); // 初始位置
    m_player2->setZValue(1); // 角色在地图上方
    addItem(m_player2);

    qDebug() << "[BattleScene] 场景初始化完成，尺寸：" << SCENE_WIDTH << "x" << SCENE_HEIGHT;
}

void BattleScene::startLoop() {
    m_gameTimer = new QTimer(this);
    // 约60FPS（16ms/帧）
    connect(m_gameTimer, &QTimer::timeout, this, [this]() {
        m_deltaTime = m_gameTimer->interval();
        gameLoop();
    });
    m_gameTimer->start(16);
}

void BattleScene::gameLoop() {
    // 更新角色动画
    if (m_player1) m_player1->updateAnimation(m_deltaTime);
    if (m_player2) m_player2->updateAnimation(m_deltaTime);
}

void BattleScene::keyPressEvent(QKeyEvent* event) {
    // 玩家1控制：A(左)、D(右)、S(下蹲)
    if (event->key() == Qt::Key_A) m_player1->setLeftPressed(true);
    else if (event->key() == Qt::Key_D) m_player1->setRightPressed(true);
    else if (event->key() == Qt::Key_S) m_player1->setCrouchPressed(true);

    // 玩家2控制：左方向键、右方向键、下方向键
    else if (event->key() == Qt::Key_Left) m_player2->setLeftPressed(true);
    else if (event->key() == Qt::Key_Right) m_player2->setRightPressed(true);
    else if (event->key() == Qt::Key_Down) m_player2->setCrouchPressed(true);

    QGraphicsScene::keyPressEvent(event);
}

void BattleScene::keyReleaseEvent(QKeyEvent* event) {
    // 玩家1控制释放
    if (event->key() == Qt::Key_A) m_player1->setLeftPressed(false);
    else if (event->key() == Qt::Key_D) m_player1->setRightPressed(false);
    else if (event->key() == Qt::Key_S) m_player1->setCrouchPressed(false);

    // 玩家2控制释放
    else if (event->key() == Qt::Key_Left) m_player2->setLeftPressed(false);
    else if (event->key() == Qt::Key_Right) m_player2->setRightPressed(false);
    else if (event->key() == Qt::Key_Down) m_player2->setCrouchPressed(false);

    QGraphicsScene::keyReleaseEvent(event);
}
