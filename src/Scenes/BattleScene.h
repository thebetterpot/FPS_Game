#ifndef BATTLESCENE_H
#define BATTLESCENE_H

#include <QGraphicsScene>
#include <QTimer>

#include "../Items/Weapon.h"
#include "../Items/Projectile.h"

// 提前声明
class Character;
class Link;
class HpBar;

class BattleScene : public QGraphicsScene {
    Q_OBJECT

public:
    // 场景尺寸常量
    static const int SCENE_WIDTH = 1280;
    static const int SCENE_HEIGHT = 720;

    explicit BattleScene(QObject *parent = nullptr);
    ~BattleScene() override;

    void spawnWeapon(WeaponType type, QPointF position);
    void spawnWeapon(WeaponType type, const QString& imagePath, QPointF position);

protected:
    // 新增：重写键盘事件处理函数
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void handleWeaponAttack(WeaponType type, QPointF position, qreal direction);
    void updateProjectiles();
    // 场景更新定时器
    void updateScene();

private:
    Link* m_player1;      // 玩家1
    Character* m_player2; // 玩家2
    QTimer* m_timer;      // 更新定时器
    HpBar* m_hpBar1;  // 新增：玩家1血条
    HpBar* m_hpBar2;  // 新增：玩家2血条
    QList<Projectile*> m_projectiles;
    QTimer m_projectileTimer;
};

#endif // BATTLESCENE_H
