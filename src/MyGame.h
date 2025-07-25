#ifndef MYGAME_H
#define MYGAME_H

#include <QMainWindow>
#include <QGraphicsView>
#include "Scenes/BattleScene.h"

class MyGame : public QMainWindow {
    Q_OBJECT

public:
    MyGame(QWidget* parent = nullptr);
    ~MyGame() override;

private:
    BattleScene* m_scene = nullptr;
    QGraphicsView* m_view = nullptr;
};

#endif // MYGAME_H
