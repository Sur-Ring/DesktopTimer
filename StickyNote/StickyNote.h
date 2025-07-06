//
// Created by Jiang_Boyuan on 25-3-31.
//

#ifndef StickyNote_H
#define StickyNote_H

#include <QWidget>

#include <windows.h>
#include "Monitor.h"
#include "Elevator.h"

#include <QMouseEvent>
#include <QSettings>

namespace StickyNote {
QT_BEGIN_NAMESPACE
namespace Ui { class StickyNote; }
QT_END_NAMESPACE

class StickyNote : public QWidget {
Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void paintEvent(QPaintEvent*event) override;
private:
    bool inited = false;
    int boundaryWidth = 4; // 可拖动距离
    QPoint m_dragPosition;  // 用于窗口移动的临时变量
public:
    explicit StickyNote(QWidget *parent = nullptr);
    ~StickyNote() override;
    void set_window_style();
    QSettings* config;
private:
    Ui::StickyNote *ui;
    HWND hWnd;
    Monitor monitor;
    Elevator elevator;
private slots:
    void move_to_top();
    void checkType(Monitor::Type);
};
} // StickyNote

#endif //StickyNote_H
