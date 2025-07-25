
#include "MyGame.h"
#include <QGraphicsView>

MyGame::MyGame(QWidget *parent)
    : QMainWindow(parent)
{
    // 创建战斗场景
    BattleScene* battleScene = new BattleScene(this);

    // 创建视图
    QGraphicsView* view = new QGraphicsView(battleScene);
    view->setFixedSize(BattleScene::SCENE_WIDTH, BattleScene::SCENE_HEIGHT);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setRenderHint(QPainter::SmoothPixmapTransform);

    // 设置为主窗口
    setCentralWidget(view);
    setFixedSize(BattleScene::SCENE_WIDTH, BattleScene::SCENE_HEIGHT);
    setWindowTitle("FPS Game 2025");

    // 启动游戏
    battleScene->startLoop();
}
