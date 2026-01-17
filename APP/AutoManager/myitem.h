#ifndef MYITEM_H
#define MYITEM_H

#include <QWidget>
#include "globalconfig.h"
#include "switchbuttonwidget.h"
namespace Ui {
class MyItem;
}

class MyItem : public QWidget
{
    Q_OBJECT

public:
    explicit MyItem(const AppInfo &info, QWidget *parent = nullptr);
    void setChecked(bool checked);
    bool isChecked() const;
    ~MyItem();
    quint8 GetAppid();

signals:
    void S_MyItem_btn(bool is_check);
private:
    AppInfo _Appinfo;
    Ui::MyItem *ui;

};

#endif // MYITEM_H
