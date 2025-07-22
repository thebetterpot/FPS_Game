#include <QApplication>
#include "MyGame.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MyGame gameWindow;
    gameWindow.show();
    return app.exec();
}
