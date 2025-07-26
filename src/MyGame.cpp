#include "MyGame.h"
#include "Scenes/BattleScene.h"
#include <QGraphicsView>

MyGame::MyGame(QWidget *parent) : QMainWindow(parent), m_battleScene(nullptr), m_view(nullptr) {
    setWindowTitle("游戏窗口");
    setFixedSize(BattleScene::SCENE_WIDTH, BattleScene::SCENE_HEIGHT);

    // 创建战斗场景
    m_battleScene = new BattleScene(this);

    // 创建视图并关联场景
    m_view = new QGraphicsView(m_battleScene, this);
    m_view->setFixedSize(BattleScene::SCENE_WIDTH, BattleScene::SCENE_HEIGHT);
    m_view->setSceneRect(0, 0, BattleScene::SCENE_WIDTH, BattleScene::SCENE_HEIGHT);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(m_view);
}

MyGame::~MyGame() {
    // 子对象由Qt父对象机制自动清理
}
