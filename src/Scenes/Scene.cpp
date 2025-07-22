#include "Scene.h"
#include <QDateTime>

Scene::Scene(QObject *parent) : QGraphicsScene(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Scene::onUpdate);
}

void Scene::startLoop() {
    lastTime = QDateTime::currentMSecsSinceEpoch();
    timer->start(16); // 60FPS 间隔
}

void Scene::onUpdate() {
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    processInput();   // 子类实现输入
    processMovement();// 子类实现移动
    processPicking(); // 子类实现拾取

    this->update();   // 触发渲染
}
