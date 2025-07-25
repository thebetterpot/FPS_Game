#ifndef BATTLESCENE_H
#define BATTLESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include "../Items/Characters/Character.h"
#include "../Items/Maps/Battlefield.h"

class BattleScene : public QGraphicsScene {
    Q_OBJECT
public:
    static const int SCENE_WIDTH = 1280;
    static const int SCENE_HEIGHT = 720;

    explicit BattleScene(QObject* parent = nullptr);
    void startLoop(); // 启动游戏循环

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void gameLoop(); // 游戏主循环

private:
    Character* m_player1 = nullptr;
    Character* m_player2 = nullptr;
    QTimer* m_gameTimer = nullptr;
    int m_deltaTime = 0; // 帧间隔时间（毫秒）
};

#endif // BATTLESCENE_H
