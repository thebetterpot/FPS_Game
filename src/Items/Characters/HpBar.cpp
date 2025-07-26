#include "HpBar.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QDebug>

HpBar::HpBar(Character* target, int playerId, int yOffset, QObject* parent)
    : QObject(parent), m_target(target), m_playerId(playerId), m_yOffset(yOffset) {
    // 1. 强制顶层显示（Z值设为最高可能值）
    setZValue(1000);

    // 2. 固定在左上角可见区域（确保不超出场景）
    setPos(50, 50 + yOffset); // 远离边缘，避免被窗口边框遮挡

    // 调试：确认血条对象已创建
    qDebug() << "[HpBar] 玩家" << playerId << "血条已创建，位置：" << pos();
}

// 3. 确保碰撞盒完全包含血条和标签（避免绘制被裁剪）
QRectF HpBar::boundingRect() const {
    return QRectF(-60, 0, m_width + 60, m_height + 4); // 左侧预留60像素给标签
}

void HpBar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 4. 绘制 debug 背景块（临时确认绘制区域，可删除）
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor(0, 0, 255, 50))); // 半透明蓝色背景
    painter->drawRect(boundingRect());

    // 5. 绘制玩家标签（高对比度黄底黑字）
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(QBrush(Qt::yellow));
    painter->drawRect(-55, 2, 45, m_height - 4); // 标签背景块

    QFont font = painter->font();
    font.setPointSize(14);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(-50, m_height/2 + 6, QString("P%1").arg(m_playerId)); // 标签文字

    // 6. 绘制血条边框（亮黄色粗线）
    painter->setPen(QPen(Qt::yellow, 3));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(0, 0, m_width + 2, m_height + 2);

    // 7. 绘制血条背景（深灰色，确保与前景对比）
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor(80, 80, 80)));
    painter->drawRect(1, 1, m_width, m_height);

    // 8. 绘制当前血量（亮红色，高饱和度）
    if (m_target) {
        float hpRatio = static_cast<float>(m_target->getHp()) / m_target->getMaxHp();
        hpRatio = qBound(0.0f, hpRatio, 1.0f); // 强制限制在0~1范围
        int currentWidth = static_cast<int>(m_width * hpRatio);

        // 血量颜色随剩余量变化（健康→危险）
        QColor hpColor = hpRatio > 0.5 ? QColor(0, 255, 0) : // 绿色（健康）
                             hpRatio > 0.2 ? QColor(255, 255, 0) : // 黄色（警告）
                             QColor(255, 0, 0);   // 红色（危险）
        painter->setBrush(QBrush(hpColor));
        painter->drawRect(1, 1, currentWidth, m_height);

        // 调试：打印血量比例
        if (m_playerId == 1) { // 只打印一次避免刷屏
            qDebug() << "[HpBar] 血量比例：" << hpRatio;
        }
    } else {
        qWarning() << "[HpBar] 玩家" << m_playerId << "血条未关联角色！";
    }
}

void HpBar::updateHpBar() {
    update(); // 强制重绘
}
