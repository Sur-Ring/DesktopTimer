//
// Created by Jiang_Boyuan on 25-7-5.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TimerTab.h" resolved

#include "TimerTab.h"
#include "ui_TimerTab.h"

#include "TimerEntry.h"

#include <QJsonObject> // { }
#include <QJsonArray> // [ ]
#include <QJsonDocument> // 解析Json
#include <QJsonValue> // int float double bool null { } [ ]
#include <QJsonParseError>
#include "TimerWidget.h"

namespace Timer {

TimerTab::TimerTab(QWidget *parent) :
    QWidget(parent), ui(new Ui::TimerTab){
    ui->setupUi(this);
    connect(ui->tab_name,&QLineEdit::textChanged,this,&TimerTab::_on_name_changed);
    connect(ui->add_btn, &QPushButton::clicked, this, &TimerTab::add_entry);
    connect(ui->fold_btn, &QPushButton::clicked, this, &TimerTab::switch_fold);
    connect(ui->up_btn, &QPushButton::clicked, this, [this](){move_up(this);});
    connect(ui->down_btn, &QPushButton::clicked, this, [this](){move_down(this);});
    connect(ui->del_btn, &QPushButton::clicked, this, &TimerTab::del_tab);
    ui->tab_name->installEventFilter(this);
}

TimerTab::TimerTab(const QJsonObject& tab_data, QWidget *parent) : TimerTab(parent){
    // 解析标题
    if (!tab_data["TabName"].isString()) {
        qDebug() << "tab name decode error";
        return;
    }
    ui->tab_name->setText(tab_data["TabName"].toString());

    // 解析条目
    if (!tab_data["Entries"].isArray()) {
        qDebug() << "tab entries decode error";
        return;
    }
    QJsonArray entry_array = tab_data["Entries"].toArray();
    for (int i = 0; i < entry_array.size(); i++) {
        if (!entry_array[i].isObject()) {
            qDebug() << "entry decode error";
            return;
        }
        QJsonObject entry_data = entry_array[i].toObject();
        create_entry(entry_data);
    }

    if (!tab_data["Fold"].isBool()) {
        qDebug() << "tab name decode error";
        is_fold=false;
    }else {
        is_fold = tab_data["Fold"].toBool();
    }
    set_fold();

    reorder_entries();
}

TimerTab::~TimerTab() {
    delete ui;
}

bool TimerTab::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::FocusOut) {
        // qDebug() << "Child widget lost focus!";
        need_save();
    }
    return QWidget::eventFilter(obj, event);
}

void TimerTab::switch_fold() {
    is_fold = !is_fold;
    set_fold();
}
void TimerTab::set_fold() {
    if(is_fold) {
        ui->entries->hide();
        ui->fold_btn->setIcon(QIcon::fromTheme("zoom-in"));
    }else {
        ui->entries->show();
        ui->fold_btn->setIcon(QIcon::fromTheme("zoom-out"));
    }
}

void TimerTab::_on_name_changed() {
    qDebug() << "on_name_changed";
    edited();
}

void TimerTab::update_time() {
    // qDebug() << "TimerTab update_time";
    _on_update_time();

    if (ui->entry_layout->isEmpty()) {
        ui->next_desc->setText("");
        ui->next_ddl->setText("");
        ui->next_rest->setText("");
        return;
    }
    TimerEntry* entry = dynamic_cast<TimerEntry*>(ui->entry_layout->itemAt(0)->widget());
    QDateTime ddl = entry->get_ddl();
    if(!ddl.isValid()){
        ui->next_ddl->setText("");
        ui->next_rest->setText("");
        return;
    }

    ui->next_ddl->setText(ddl.toString("MM-dd HH:mm"));

    QDateTime cur = QDateTime::currentDateTime();
    qint64 days = cur.daysTo(ddl);

    if (days==0) {
        ui->next_rest->setText("今天");
    } else if (days==1) {
        ui->next_rest->setText("明天");
    } else {
        ui->next_rest->setText(QString::number(days)+"天");
    }
}

void TimerTab::reorder_entries() {
    // qDebug() << "TimerTab::reorder_entries";
    if (ui->entry_layout->isEmpty()) {
        // qDebug() << "reorder_entries is empty";
        return;
    }

    // 选择排序
    for (int i=0; i<ui->entry_layout->count(); i++) {
        int min_idx = i;
        TimerEntry* min_entry = dynamic_cast<TimerEntry*>(ui->entry_layout->itemAt(i)->widget());
        if(!min_entry->ddl.isValid()) continue;

        for (int j=i+1; j<ui->entry_layout->count(); j++) {
            TimerEntry* check_entry = dynamic_cast<TimerEntry*>(ui->entry_layout->itemAt(j)->widget());
            if(!check_entry->ddl.isValid()) continue;

            if (check_entry->ddl<min_entry->ddl) {
                min_idx = j;
                min_entry = check_entry;
            }
        }

        if (i!=min_idx) {
            QLayoutItem *min_item = ui->entry_layout->takeAt(min_idx);
            ui->entry_layout->insertItem(i, min_item);
        }
    }

    update_next();
}

void TimerTab::update_next() {
    if (ui->entry_layout->isEmpty()) {
        ui->next_desc->setText("");
        ui->next_ddl->setText("");
        ui->next_rest->setText("");
        return;
    }

    TimerEntry* entry = dynamic_cast<TimerEntry*>(ui->entry_layout->itemAt(0)->widget());
    ui->next_desc->setText(entry->get_desc());
    update_time();
}

void TimerTab::add_entry() {
    qDebug() << "TimerTab::add_entry";

    QJsonObject entry_data;
    entry_data.insert("Desc", "新条目");
    entry_data.insert("DDL", "00-00 00:00");
    create_entry(entry_data);

    need_save();
}

void TimerTab::create_entry(const QJsonObject &entry_data) {
    // qDebug() << "TimerTab::create_entry";
    TimerEntry* timer_entry = new TimerEntry(entry_data);
    ui->entry_layout->addWidget(timer_entry);
    connect(this, &TimerTab::_on_update_time, timer_entry, TimerEntry::update_time);
    connect(timer_entry, &TimerEntry::edited, this, &TimerTab::edited);
    connect(timer_entry, &TimerEntry::need_save, this, &TimerTab::need_save);
    connect(timer_entry, &TimerEntry::need_save, this, &TimerTab::reorder_entries);
}

QJsonObject TimerTab::get_json() {
    QJsonObject tab_data;
    tab_data["TabName"] = ui->tab_name->text();
    QJsonArray entry_array;
    for (int i = 0; i < ui->entry_layout->count(); i++){
        TimerEntry* entry = dynamic_cast<TimerEntry*>(ui->entry_layout->itemAt(i)->widget());
        entry_array.append(entry->get_json());
    }
    tab_data["Entries"] = entry_array;
    tab_data["Fold"] = is_fold;

    return tab_data;
}

void TimerTab::del_tab() {
    qDebug() << "TimerTab::del_tab";
    setParent(NULL);
    need_save();
    deleteLater();
}

} // Timer
