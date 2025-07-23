#include "BattleScene.h"
#include <QDebug>

BattleScene::BattleScene(QObject *parent) : Scene(parent) {
    setBackgroundBrush(Qt::lightGray);

    // 初始化玩家角色，使用新的素材
    player1 = new Character(1, ":/character1.png");  // 玩家1使用character1.png
    player2 = new Character(2, ":/character2.png");  // 玩家2使用character2.png

    // 设置初始位置
    player1->setPos(200, 300);
    player2->setPos(1000, 300);

    // 添加到场景
    addItem(player1);
    addItem(player2);

    // 启动游戏循环
    startLoop();

    qDebug() << "BattleScene初始化完成，使用character1和character2素材";
}

void BattleScene::processInput() {
    syncPlayerInputs();
}

void BattleScene::processMovement() {
    qreal speedFactor = deltaTime / 16.0;

    // 更新角色动画（传入帧间隔时间）
    player1->updateAnimation(deltaTime);
    player2->updateAnimation(deltaTime);

    // 更新玩家1位置
    player1->setX(player1->x() + player1->velocity().x() * speedFactor);
    // 更新玩家2位置
    player2->setX(player2->x() + player2->velocity().x() * speedFactor);

    // 边界限制
    const int sceneW = 1280, sceneH = 720, charSize = 48;

    // 玩家1边界检查
    if (player1->x() < 0)
        player1->setX(0);
    if (player1->x() > sceneW - charSize)
        player1->setX(sceneW - charSize);
    if (player1->y() < 0)
        player1->setY(0);
    if (player1->y() > sceneH - charSize)
        player1->setY(sceneH - charSize);

    // 玩家2边界检查
    if (player2->x() < 0)
        player2->setX(0);
    if (player2->x() > sceneW - charSize)
        player2->setX(sceneW - charSize);
    if (player2->y() < 0)
        player2->setY(0);
    if (player2->y() > sceneH - charSize)
        player2->setY(sceneH - charSize);
}

void BattleScene::processPicking() {
    // 暂未实现
}

void BattleScene::syncPlayerInputs() {
    // 同步玩家1输入
    player1->setLeftPressed(p1Left);
    player1->setRightPressed(p1Right);
    player1->setCrouchPressed(p1Crouch);
    player1->setJumpPressed(p1Jump);

    // 同步玩家2输入
    player2->setLeftPressed(p2Left);
    player2->setRightPressed(p2Right);
    player2->setCrouchPressed(p2Crouch);
    player2->setJumpPressed(p2Jump);
}

void BattleScene::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    // 玩家1控制（WASD）
    case Qt::Key_A: p1Left = true; break;
    case Qt::Key_D: p1Right = true; break;
    case Qt::Key_S: p1Crouch = true; break;
    case Qt::Key_W: p1Jump = true; break;

    // 玩家2控制（方向键）
    case Qt::Key_Left: p2Left = true; break;
    case Qt::Key_Right: p2Right = true; break;
    case Qt::Key_Down: p2Crouch = true; break;
    case Qt::Key_Up: p2Jump = true; break;

    default:
        Scene::keyPressEvent(event);
    }
}

void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    // 玩家1控制（WASD）
    case Qt::Key_A: p1Left = false; break;
    case Qt::Key_D: p1Right = false; break;
    case Qt::Key_S: p1Crouch = false; break;
    case Qt::Key_W: p1Jump = false; break;

    // 玩家2控制（方向键）
    case Qt::Key_Left: p2Left = false; break;
    case Qt::Key_Right: p2Right = false; break;
    case Qt::Key_Down: p2Crouch = false; break;
    case Qt::Key_Up: p2Jump = false; break;

    default:
        Scene::keyReleaseEvent(event);
    }
}
