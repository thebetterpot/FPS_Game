#ifndef MYGAME_H
#define MYGAME_H

#include <QMainWindow>
#include "Scenes/BattleScene.h"

class MyGame : public QMainWindow {
    Q_OBJECT
public:
    MyGame(QWidget *parent = nullptr);
};

#endif // MYGAME_H
