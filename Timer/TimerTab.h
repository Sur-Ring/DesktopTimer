//
// Created by Jiang_Boyuan on 25-7-5.
//

#ifndef TIMERTAB_H
#define TIMERTAB_H

#include <QWidget>
#include <QJsonObject>

namespace Timer {
QT_BEGIN_NAMESPACE
namespace Ui { class TimerTab; }
QT_END_NAMESPACE

class TimerTab : public QWidget {
Q_OBJECT

public:
    explicit TimerTab(QWidget *parent = nullptr);
    explicit TimerTab(const QJsonObject &tab_data, QWidget *parent = nullptr);
    bool eventFilter(QObject *obj, QEvent *event) override;
    ~TimerTab() override;
    QJsonObject get_json();
    void create_entry(const QJsonObject &entry_data);
    bool is_fold;
    public slots:
        void update_next();
        void switch_fold();
        void set_fold();
        void update_time();
        void add_entry();
        void reorder_entries();
        void del_tab();
        void _on_name_changed();
    signals:
        void _on_update_time();
        void edited();
        void need_save();
        void move_down(TimerTab* tab);
        void move_up(TimerTab* tab);

private:
    Ui::TimerTab *ui;
};
} // Timer

#endif //TIMERTAB_H
