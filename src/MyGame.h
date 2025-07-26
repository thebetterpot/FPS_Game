#ifndef MYGAME_H
#define MYGAME_H

#include <QMainWindow>

class BattleScene;
class QGraphicsView;

class MyGame : public QMainWindow {
    Q_OBJECT

public:
    explicit MyGame(QWidget *parent = nullptr);
    ~MyGame() override; // 显式声明析构函数

private:
    BattleScene* m_battleScene; // 战斗场景
    QGraphicsView* m_view;      // 视图
};

#endif // MYGAME_H
