#include "MyGame.h"
#include <QDebug>

MyGame::MyGame(QWidget* parent) : QMainWindow(parent) {
    // 设置窗口属性
    setWindowTitle("九宫格精灵图动画演示");
    setFixedSize(BattleScene::SCENE_WIDTH, BattleScene::SCENE_HEIGHT);
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowTitleHint);

    // 创建并显示场景
    m_scene = new BattleScene();
    m_view = new QGraphicsView(m_scene, this);
    m_view->setSceneRect(0, 0, BattleScene::SCENE_WIDTH, BattleScene::SCENE_HEIGHT);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHint(QPainter::SmoothPixmapTransform); // 平滑渲染
    m_view->setFixedSize(BattleScene::SCENE_WIDTH, BattleScene::SCENE_HEIGHT);

    setCentralWidget(m_view);

    // 启动游戏
    m_scene->startLoop();
    qDebug() << "[MyGame] 游戏启动成功";
}

MyGame::~MyGame() {
    delete m_view;
    delete m_scene;
}
