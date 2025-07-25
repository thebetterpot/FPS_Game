#ifndef MAP_H
#define MAP_H

#include <QGraphicsItem>
#include <QPixmap>

class Map : public QGraphicsItem {
public:
    explicit Map(QGraphicsItem* parent = nullptr);

    // 加载背景图
    bool loadBackground(const QString& resourcePath);

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QPixmap m_background; // 背景图
};

#endif // MAP_H
