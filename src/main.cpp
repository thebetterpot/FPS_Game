#include <QApplication>
#include "Scenes/BattleScene.h" // 关键修正：BattleScene在Scenes文件夹下
#include "StartDialog.h" // StartDialog在src文件夹下，直接引用
#include <QGraphicsView>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 显示启动弹窗
    StartDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        // 创建游戏场景（路径正确后可正常实例化）
        BattleScene scene;
        scene.setSceneRect(0, 0, BattleScene::SCENE_WIDTH, BattleScene::SCENE_HEIGHT);

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
