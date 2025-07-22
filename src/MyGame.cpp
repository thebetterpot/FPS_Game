#include "MyGame.h"
#include <QDebug>

MyGame::MyGame(QWidget *parent) : QMainWindow(parent) {
    // 配置窗口
    setWindowTitle("双人对战游戏");
    setFixedSize(1280, 720); // 固定窗口大小

    // 初始化战斗场景
    battleScene = new BattleScene(this);
    if (!battleScene) {
        qCritical() << "无法创建战斗场景!";
        return;
    }

    // 配置视图
    view = new QGraphicsView(battleScene, this);
    view->setSceneRect(0, 0, 1280, 720); // 场景可视区域
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 隐藏滚动条
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setRenderHint(QPainter::SmoothPixmapTransform); // 平滑渲染

    // 设置为中心部件
    setCentralWidget(view);
    qDebug() << "主窗口初始化完成";
}

MyGame::~MyGame() {
    delete battleScene;
    delete view;
}
