#include "Map.h"
#include <QPainter>
#include <QDebug>

Map::Map(QGraphicsItem* parent) : QGraphicsItem(parent) {}

bool Map::loadBackground(const QString& resourcePath) {
    if (m_background.load(resourcePath)) {
        qDebug() << "[Map] 背景图加载成功：" << resourcePath
                 << "尺寸：" << m_background.size();
        return true;
    } else {
        qWarning() << "[Map] 背景图加载失败：" << resourcePath;
        return false;
    }
}

QRectF Map::boundingRect() const {
    // 背景尺寸固定为1280x720
    return QRectF(0, 0, 1280, 720);
}

void Map::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_background.isNull()) {
        // 缩放背景图至1280x720
        painter->drawPixmap(boundingRect().toRect(),
                            m_background.scaled(1280, 720,
                                                Qt::IgnoreAspectRatio,
                                                Qt::SmoothTransformation));
    } else {
        // 背景加载失败时显示灰色底和提示文字
        painter->fillRect(boundingRect(), Qt::lightGray);
        painter->drawText(boundingRect(), Qt::AlignCenter, "背景图加载失败\n请检查map_tu.png");
    }
}
