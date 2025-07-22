#ifndef BATTLESCENE_H
#define BATTLESCENE_H

#include "Scene.h"
#include "../Items/Characters/Character.h"
#include "../Items/Characters/Link.h"
#include <QKeyEvent>

class BattleScene : public Scene {
    Q_OBJECT

public:
    explicit BattleScene(QObject *parent = nullptr);

protected:
    void processInput() override;
    void processMovement() override;
    void processPicking() override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    Character *player1; // Link 角色
    Character *player2; // 通用角色
    bool p1Left = false, p1Right = false, p1Crouch = false, p1Jump = false;
    bool p2Left = false, p2Right = false, p2Crouch = false, p2Jump = false;

    void syncPlayerInputs();
};

#endif // BATTLESCENE_H
