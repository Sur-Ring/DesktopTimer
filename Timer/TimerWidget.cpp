//
// Created by Jiang_Boyuan on 25-5-27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TimerWidget.h" resolved

#include "TimerWidget.h"

#include <iostream>

#include "ui_TimerWidget.h"
#include <QSettings>
#include <QFile>
#include "TimerEntry.h"
#include "TimerTab.h"

#include <QJsonObject> // { }
#include <QJsonArray> // [ ]
#include <QJsonDocument> // 解析Json
#include <QJsonValue> // int float double bool null { } [ ]
#include <QJsonParseError>

QString data_file_path = "./Data/Data.txt";
// QString data_file_path = "D:/Codes/QT/DesktopTimer/Data/Data.txt";
// QString data_file_path = "D:/Codes/QT/DesktopTimer/Data/Data2.txt";
namespace Timer {
TimerWidget::TimerWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::TimerWidget) {
    ui->setupUi(this);
    qDebug() << "Timer init";

    // 加载配置
    QSettings* config = new QSettings("./Data/cfg.ini", QSettings::IniFormat);
    int font_size = config->value("/SN/FontSize", 14).toUInt();
    config->setValue("/SN/FontSize", font_size);
    ft.setPointSize(font_size);

    // 加载数据
    load_data();

    // 新增按钮
    connect(ui->add_btn, &QPushButton::clicked, this, &TimerWidget::add_tab);
    ui->add_btn->hide();

    // 启动计时器
    m_pTimer.start(5000);
}

TimerWidget::~TimerWidget() {
    save_data();
    delete ui;
}

void TimerWidget::load_data() {
    qDebug() << "TimerWidget load_data";

    // 读取数据文件
    QFile file(data_file_path);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "can't open data file!";
        return;
    }
    QTextStream stream(&file);
    QString str = stream.readAll();
    // qDebug() << "str: " << str;
    file.close();

    // 解析json
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError && !doc.isNull()) {
        qDebug() << "Json error！" << jsonError.error;
        return;
    }
    if (!doc.isArray()) {
        qDebug() << "data decode error";
        return;
    }
    QJsonArray tab_array = doc.array();
    for (int i = 0; i < tab_array.size(); i++) {
        QJsonValue tab = tab_array.at(i);
        if (!tab.isObject()) {
            qDebug() << "tab decode error";
            return;
        }
        QJsonObject tab_data = tab.toObject();
        create_tab(tab_data);
    }
}

void TimerWidget::save_data() {
    qDebug() << "TimerWidget save_data";

    QJsonArray tab_array;
    for (int i = 0; i < ui->tab_layout->count(); i++){
        TimerTab* tab = dynamic_cast<TimerTab*>(ui->tab_layout->itemAt(i)->widget());
        tab_array.append(tab->get_json());
    }

    QJsonDocument doc;
    doc.setArray(tab_array);

    QFile file(data_file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "can't open error!";
        return;
    }
    QTextStream stream(&file);
    stream << doc.toJson();
    file.close();

    saved();
}

void TimerWidget::add_tab() {
    qDebug() << "TimerWidget add_tab";

    QJsonObject tab_data;
    tab_data.insert("TabName", "新项目");
    tab_data.insert("Entries", QJsonArray());
    tab_data.insert("Fold", false);
    create_tab(tab_data);
}

void TimerWidget::create_tab(const QJsonObject &tab_data) {
    // qDebug() << "TimerWidget::create_tab";
    TimerTab* timer_tab = new TimerTab(tab_data);
    ui->tab_layout->addWidget(timer_tab);
    connect(&m_pTimer, &QTimer::timeout, timer_tab, &TimerTab::update_time);
    connect(timer_tab, &TimerTab::edited, this, &TimerWidget::edit_data);
    connect(timer_tab, &TimerTab::need_save, this, &TimerWidget::save_data);
    connect(timer_tab, &TimerTab::move_down, this, &TimerWidget::move_down);
    connect(timer_tab, &TimerTab::move_up, this, &TimerWidget::move_up);
}

void TimerWidget::edit_data() {
    // qDebug() << "TimerWidget edit_data";
    changed();
}

void TimerWidget::move_up(TimerTab* tab) {
    qDebug() << "TimerWidget::move_up";
    int index = ui->tab_layout->indexOf(tab); // 获取控件在布局中的索引
    if (index == 0) return;
    QLayoutItem *item = ui->tab_layout->takeAt(index);
    ui->tab_layout->insertItem(index-1, item);
    save_data();
}
void TimerWidget::move_down(TimerTab* tab) {
    qDebug() << "TimerWidget::move_down";
    int index = ui->tab_layout->indexOf(tab); // 获取控件在布局中的索引
    if (index ==  ui->tab_layout->count()-1) return;
    QLayoutItem *item = ui->tab_layout->takeAt(index);
    ui->tab_layout->insertItem(index+1, item);
    save_data();
}

} // Timer
