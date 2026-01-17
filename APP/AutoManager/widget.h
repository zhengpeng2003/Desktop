#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QSettings>
#include "globalconfig.h"
#include <qdir.h>
#include <QDebug>
#include <myitem.h>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void InitInfo();
    void AddItem();
    ~Widget();

private slots:

    void on_close_btn_clicked();
    void SetAutoManager(bool ischecked);

private:
    QSettings *_Set=nullptr;
    QVector<AppInfo> _AppInfos;
    QList<MyItem*> m_items;
    Screen _Screen;
    Ui::Widget *ui;
};
#endif // WIDGET_H
