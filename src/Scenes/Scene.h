//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_SCENE_H
#define QT_PROGRAMMING_2024_SCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include <QtGlobal>

class Scene : public QGraphicsScene {
    Q_OBJECT

public:
    explicit Scene(QObject *parent = nullptr);
    void startLoop();

protected:
    virtual void processInput() = 0;
    virtual void processMovement() = 0;
    virtual void processPicking() = 0;

protected slots:
    void onUpdate();

protected: // 关键修改：deltaTime 改为 protected
    qint64 deltaTime{};

private:
    QTimer *timer;
    qint64 lastTime = -1;
};

#endif //QT_PROGRAMMING_2024_SCENE_H
