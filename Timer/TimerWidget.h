//
// Created by Jiang_Boyuan on 25-5-27.
//

#ifndef TIMERWIDGET_H
#define TIMERWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QJsonArray>
#include <QVBoxLayout>

#include "TimerTab.h"

namespace Timer {
QT_BEGIN_NAMESPACE
namespace Ui { class TimerWidget; }
QT_END_NAMESPACE

class TimerWidget : public QWidget {
Q_OBJECT

public:
    explicit TimerWidget(QWidget *parent = nullptr);
    ~TimerWidget() override;
    void load_data();
    void create_tab(const QJsonObject &tab_data);
signals:
    void saved();
    void changed();
public slots:
    void move_up(TimerTab* tab);
    void move_down(TimerTab* tab);
    void save_data();
    void add_tab();
    void edit_data();
private:
    Ui::TimerWidget *ui;
    QFont ft;
    QTimer m_pTimer;
};
} // Timer

#endif //TIMERWIDGET_H
