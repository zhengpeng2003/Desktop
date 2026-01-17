#include "widget.h"
#include "ui_widget.h"
//tets
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)// _Set("../../../Desktop/Appinfo/Appinfo.ini",QSettings::IniFormat)
{

    QString appDir = QCoreApplication::applicationDirPath();

    // 创建 QDir 对象并进入子目录
    QDir dir(appDir);
    dir.cdUp();
    QString iniPath = dir.absoluteFilePath("AppInfo/Appinfo.ini");
    _Set = new QSettings(iniPath, QSettings::IniFormat);
    qDebug()<<iniPath;
    InitInfo();
    ui->setupUi(this);
    this->resize(_Screen.Screen_Width,_Screen.Screen_Height);
    ui->close_btn->setStyleSheet(
        "QPushButton {"
        "  border: none;"
        "  background: transparent;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(255, 255, 255, 30);"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgba(255, 255, 255, 60);"
        "}"
        );
    AddItem();
    //读取ini文件中当前启动的项然后设置开关
    _Set->beginGroup("Autoexe");
    quint8 AutoID=_Set->value("Id").toUInt();
    qDebug()<<AutoID;
    _Set->endGroup();
    for (MyItem *item : m_items)
    {
        if(AutoID==item->GetAppid())
        {
            item->setChecked(true);
        }
    }
}

void Widget::InitInfo()
{
    //初始化屏幕
    _Set->beginGroup("Screen");
    _Screen.Screen_Height=_Set->value("Screen_Height").toUInt();
    _Screen.Screen_Width=_Set->value("Screen_Width").toUInt();//Screen/Screen_Height读取的位置类似于
    qDebug() << "cwd:" << QDir::currentPath();//获取的那当前文件执行位置
    _Set->endGroup();
    //初始化APP信息
    QStringList groups = _Set->childGroups();
    for(auto group: groups)
    {
        if (!group.startsWith("APP"))
        continue;
        _Set->beginGroup(group);
        quint8 id = _Set->value("Id").toUInt();
        QString name = _Set->value("Name").toString();
        QString path = _Set->value("Path").toString();
        QString Log = _Set->value("Log").toString();
        _Set->endGroup();
        qDebug() << id << name << path << Log;
        AppInfo TempInfo(id,name,path,Log);
        _AppInfos.append(TempInfo);
    }
}

void Widget::AddItem()
{
    ui->listWidget->clear();

    for (int i = 0; i < _AppInfos.count(); ++i)
    {
        const AppInfo &app = _AppInfos[i];

        // 1️⃣ 创建 item（只负责占一行）
        QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
        item->setSizeHint(QSize(ui->listWidget->width(), 60));

        // 2️⃣ 创建真正显示的控件
        MyItem  * w = new MyItem(_AppInfos[i],ui->listWidget);
        // 3️⃣ 绑定 item 与 widget
        connect(w,&MyItem::S_MyItem_btn,this,&Widget::SetAutoManager);
        ui->listWidget->setItemWidget(item, w);
        m_items.append(w);
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_close_btn_clicked()
{

    QString exeDir   = QCoreApplication::applicationDirPath(); // /usr/local/qt-app/AppEex
    QDir dir(exeDir);
    dir.cdUp();
    QString stateFile = dir.absoluteFilePath("tmp/desktop_state"); // → /usr/local/qt-app/tmp/desktop_state
    QFile f(stateFile + ".tmp");
    if (f.open(QIODevice::WriteOnly)) {
        f.write("desktop");          // 小写！
        f.close();
        ::rename(qPrintable(stateFile + ".tmp"), qPrintable(stateFile));
    }
    qDebug()<<stateFile;
    qApp->quit();

}

void Widget::SetAutoManager(bool ischecked)
{
    // 找到是谁触发的
    MyItem *senderItem = qobject_cast<MyItem *>(sender());
    if (!senderItem)
        return;

    // 如果是关闭动作，可以直接返回（可选）
    if (!ischecked)
        return;

    // ⭐⭐⭐ 打开一个 → 关闭其他所有
    for (MyItem *item : m_items)
    {

        if (item != senderItem)
        {
            item->setChecked(false);
        }
        else
        {
            _Set->beginGroup("Autoexe");
            _Set->setValue("Id",item->GetAppid());
            _Set->endGroup();
            qDebug()<<item->GetAppid();
        }
    }

    // 这里你可以做真正的“启动程序 / 设置自启动”等
    qDebug() << "Active item changed";
}


