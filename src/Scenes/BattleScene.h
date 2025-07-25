#ifndef BATTLESCENE_H
#define BATTLESCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include "../Items/Characters/Character.h"
#include "../Items/Characters/Link.h"

class BattleScene : public QGraphicsScene {
    Q_OBJECT
public:
    static const int SCENE_WIDTH = 1280;  // 场景宽度
    static const int SCENE_HEIGHT = 720;  // 场景高度

    explicit BattleScene(QObject *parent = nullptr);
    ~BattleScene() override;

    void startLoop(); // 启动游戏循环

private slots:
    void update(int deltaTime); // 每帧更新

private:
    Character* m_player2 = nullptr;  // 玩家2
    Link* m_player1 = nullptr;       // 玩家1（Link角色）
    QTimer* m_gameTimer = nullptr;   // 游戏定时器
};

#endif // BATTLESCENE_H
