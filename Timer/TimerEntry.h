//
// Created by Jiang_Boyuan on 25-5-27.
//

#ifndef TIMERENTRY_H
#define TIMERENTRY_H

#include <QWidget>
#include <QDateTime>
#include <QJsonObject>

namespace Timer {
QT_BEGIN_NAMESPACE
namespace Ui { class TimerEntry; }
QT_END_NAMESPACE

class TimerEntry : public QWidget {
Q_OBJECT

public:
    explicit TimerEntry(QWidget *parent = nullptr);
    explicit TimerEntry(const QJsonObject &entry_data, QWidget *parent = nullptr);
    ~TimerEntry() override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    QJsonObject get_json();
    QString get_desc();
    QDateTime get_ddl();
    QDateTime ddl;
protected:
    void resizeEvent(QResizeEvent* event) override;
public slots:
    void _on_desc_changed();
    void resize_desc();
    void _on_ddl_changed();
    void del_entry();
    void update_time();
signals:
    void edited();
    void need_save();
private:
    Ui::TimerEntry *ui;
};
} // Timer

#endif //TIMERENTRY_H
