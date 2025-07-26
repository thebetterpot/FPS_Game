#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

class StartDialog : public QDialog {
    Q_OBJECT // 必须添加，用于信号槽

public:
    explicit StartDialog(QWidget *parent = nullptr) : QDialog(parent) {
        // 设置窗口标题和固定大小
        setWindowTitle("游戏控制说明");
        setFixedSize(300, 200); // 宽300，高200

        // 创建布局（垂直排列控件）
        QVBoxLayout *layout = new QVBoxLayout(this);

        // 第一行：P1控制说明
        QLabel *p1Label = new QLabel("P1：WASD+J", this);
        QFont font = p1Label->font();
        font.setPointSize(12); // 字体大小
        p1Label->setFont(font);
        p1Label->setAlignment(Qt::AlignCenter); // 居中显示
        layout->addWidget(p1Label);

        // 第二行：P2控制说明
        QLabel *p2Label = new QLabel("P2：上下左右+1", this);
        p2Label->setFont(font);
        p2Label->setAlignment(Qt::AlignCenter);
        layout->addWidget(p2Label);

        // 第三行：开始游戏按钮
        QPushButton *startBtn = new QPushButton("开始游戏", this);
        startBtn->setFont(font);
        startBtn->setStyleSheet("background-color: green; color: white; border-radius: 5px;");
        layout->addWidget(startBtn);

        // 点击按钮关闭弹窗（返回Accepted状态）
        connect(startBtn, &QPushButton::clicked, this, &QDialog::accept);

        setLayout(layout);
    }
};

#endif // STARTDIALOG_H
