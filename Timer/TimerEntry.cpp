//
// Created by Jiang_Boyuan on 25-5-27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_TimerEntry.h" resolved

#include "TimerEntry.h"
#include "ui_TimerEntry.h"
#include <QScrollBar>
#include <QTextBlock>
#include <QJsonObject> // { }
#include <QJsonArray> // [ ]
#include <QJsonDocument> // 解析Json
#include <QJsonValue> // int float double bool null { } [ ]
#include <QJsonParseError>

namespace Timer {
TimerEntry::TimerEntry(QWidget *parent) :
    QWidget(parent), ui(new Ui::TimerEntry){
    ui->setupUi(this);

    connect(ui->desc,&QPlainTextEdit::textChanged,this,&TimerEntry::_on_desc_changed);
    connect(ui->ddl,&QLineEdit::textChanged,this,&TimerEntry::_on_ddl_changed);
    connect(ui->del_btn,&QPushButton::clicked,this,&TimerEntry::del_entry);

    ui->desc->installEventFilter(this);
    ui->ddl->installEventFilter(this);

    update_time();
}

TimerEntry::TimerEntry(const QJsonObject &entry_data, QWidget *parent) : TimerEntry(parent){
    // 解析描述
    if (!entry_data["Desc"].isString()) {
        qDebug() << "entry desc decode error";
        return;
    }
    ui->desc->setPlainText(entry_data["Desc"].toString());

    // 解析DDL
    if (!entry_data["DDL"].isString()) {
        qDebug() << "entry desc decode error";
        return;
    }
    ui->ddl->setText(entry_data["DDL"].toString());
}

TimerEntry::~TimerEntry() {
    delete ui;
}

bool TimerEntry::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::FocusOut) {
        // qDebug() << "TimerEntry Child widget lost focus!";
        need_save();
    }
    return QWidget::eventFilter(obj, event);
}

QJsonObject TimerEntry::get_json() {
    QJsonObject entry_data;
    entry_data["Desc"] = ui->desc->toPlainText();
    entry_data["DDL"] = ui->ddl->text();
    return entry_data;
}

void TimerEntry::resizeEvent(QResizeEvent* event) {
    // 判断宽度是否变化
    if (event->oldSize().width() != event->size().width()) {
        resize_desc();
    }
}

void TimerEntry::update_time() {
    // qDebug() << "TimerEntry update_time";
    if(!ddl.isValid())return;

    QDateTime cur = QDateTime::currentDateTime();
    qint64 days = cur.daysTo(ddl);

    if (days==0) {
        ui->rest->setText("今天");
    } else if (days==1) {
        ui->rest->setText("明天");
    } else {
        ui->rest->setText(QString::number(days)+"天");
    }
}

QString TimerEntry::get_desc() {
    return ui->desc->toPlainText();
}
QDateTime TimerEntry::get_ddl() {
    return ddl;
}

void TimerEntry::_on_ddl_changed() {
    // qDebug() << "TimerEntry _on_ddl_changed";

    // 解析时间
    ddl = QDateTime::fromString(ui->ddl->text(), "MM-dd HH:mm");
    if(!ddl.isValid())ui->rest->setText("无效的时间");

    QDateTime currentDateTime = QDateTime::currentDateTime();
    int currentYear = currentDateTime.date().year();
    QDate dateThisYear(currentYear, ddl.date().month(), ddl.date().day());
    QDate dateNextYear(currentYear + 1, ddl.date().month(), ddl.date().day());

    // 比较候选日期与当前日期
    QDate currentDate = currentDateTime.date();
    if (dateThisYear >= currentDate) {
        // 今年的日期尚未过去，使用今年
        ddl = QDateTime(dateThisYear, QTime(ddl.time().hour(), 0)); // 假设分钟为0
    } else {
        // 今年的日期已过去，使用明年
        ddl = QDateTime(dateNextYear, QTime(ddl.time().hour(), 0));
    }
    if(!ddl.isValid())ui->rest->setText("无效的时间");

    update_time();

    edited();
}

void TimerEntry::_on_desc_changed() {
    // qDebug() << "TimerEntry _on_desc_changed";

    resize_desc();
    edited();
}


void TimerEntry::resize_desc() {
    // qDebug() << "TimerEntry resize_desc";

    QTextDocument* doc = ui->desc->document();
    // 计算所有文本块的总高度
    int totalHeight = 0;
    QTextBlock block = doc->firstBlock();
    while (block.isValid()) {
        QRectF blockRect = doc->documentLayout()->blockBoundingRect(block);
        totalHeight += blockRect.height();
        block = block.next();
    }

    // 添加文档边距和控件边距
    int docMargin = doc->documentMargin() * 2;
    int widgetMargin = ui->desc->contentsMargins().top() + ui->desc->contentsMargins().bottom();
    int newHeight = totalHeight + docMargin + widgetMargin;

    // 设置最小高度（至少显示一行）
    if (doc->lineCount() == 0) {
        newHeight = ui->desc->fontMetrics().lineSpacing() + docMargin + widgetMargin;
    }

    // 应用高度并更新布局
    ui->desc->setFixedHeight(newHeight);
    ui->desc->updateGeometry();
}

void TimerEntry::del_entry() {
    qDebug() << "TimerEntry::del_entry";
    setParent(NULL);
    need_save();
    deleteLater();
}

} // Timer
