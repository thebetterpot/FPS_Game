#ifndef MYGAME_H
#define MYGAME_H

#include <QMainWindow>
#include "Scenes/BattleScene.h"
#include <QGraphicsView>

class MyGame : public QMainWindow {
    Q_OBJECT

public:
    MyGame(QWidget *parent = nullptr);
    ~MyGame() override;

private:
    BattleScene *battleScene; // 战斗场景
    QGraphicsView *view;      // 图形视图
};

#endif // MYGAME_H
