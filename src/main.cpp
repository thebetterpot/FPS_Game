#include "MyGame.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MyGame game;
    game.show();
    return a.exec();
}
