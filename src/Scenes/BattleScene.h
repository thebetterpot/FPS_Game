#ifndef BATTLESCENE_H
#define BATTLESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include "../Items/Characters/Character.h"
#include "../Items/Characters/Link.h"

class BattleScene : public QGraphicsScene {
    Q_OBJECT
public:
    static const int SCENE_WIDTH = 1280;
    static const int SCENE_HEIGHT = 720;

    explicit BattleScene(QObject *parent = nullptr);
    ~BattleScene() override;

    void startLoop();

    // 新增：重写场景按键事件
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void update(int deltaTime);

private:
    Character* m_player2 = nullptr;
    Link* m_player1 = nullptr;       // character1
    QTimer* m_gameTimer = nullptr;
};

#endif // BATTLESCENE_H
