#include <QApplication>
#include "Scenes/BattleScene.h"
#include "StartDialog.h"
#include "Items/Weapon.h" // 引用items文件夹下的Weapon类
#include <QGraphicsView>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 显示启动弹窗
    StartDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        // 创建游戏场景
        BattleScene scene;
        scene.setSceneRect(0, 0, BattleScene::SCENE_WIDTH, BattleScene::SCENE_HEIGHT);

        // 生成武器（根据实际图片路径修改）
        scene.spawnWeapon(RIFLE, "", QPointF(300, 300));       // 路径已在Weapon构造函数中硬编码
        scene.spawnWeapon(SNIPER, "", QPointF(500, 300));
        scene.spawnWeapon(KNIFE, "", QPointF(700, 300));
        scene.spawnWeapon(BALL, "", QPointF(900, 300));

        // 创建视图并显示
        QGraphicsView view(&scene);
        view.setFixedSize(BattleScene::SCENE_WIDTH + 2, BattleScene::SCENE_HEIGHT + 2);
        view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view.show();

        return a.exec();
    }

    return 0;
}
