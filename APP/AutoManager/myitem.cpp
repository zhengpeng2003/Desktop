#include "myitem.h"
#include "ui_myitem.h"
MyItem::MyItem(const AppInfo &info, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyItem)   // ⭐⭐⭐ 核心
{
    ui->setupUi(this);    // ⭐⭐⭐ 先 setup
    _Appinfo=info;
    ui->label->setText(info.Name);
    connect(ui->switchbutton,&SwitchButtonWidget::toggled,this,&MyItem::S_MyItem_btn);
}
void MyItem::setChecked(bool checked)
{
    ui->switchbutton->setChecked(checked);
}

bool MyItem::isChecked() const
{
    return ui->switchbutton->isChecked();
}

MyItem::~MyItem()
{
    delete ui;
}

quint8 MyItem::GetAppid()
{
    return _Appinfo.Id;
}

