#include "widget.h"
#include "ui_widget.h"

#include <qdir.h>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(6);          // 按钮间距
    m_gridLayout->setContentsMargins(8,8,8,8);

    showFullScreen();
    InitMyBtns();
    InitLayout();
}
void Widget::InitMyBtns()
{
    MyBtn * LedBtn=new MyBtn(":/images/ledoff.png",":/images/ledon.png","/sys/class/leds/red/brightness",this);
    MyBtn * BuzzerBtn=new MyBtn(":/images/buzzeroff.png",":/images/buzzeron.png","/sys/class/gpio/gpio19/value",this);
    MyBtn * ExitBtn=new MyBtn(":/images/exit.png",":/images/exit.png"," ",this);
    connect(ExitBtn, &QPushButton::clicked,this, &Widget::onExitButtonClicked);
    _Mybtns.append(LedBtn);
    _Mybtns.append(BuzzerBtn);
    _Mybtns.append(ExitBtn);
}
void Widget::onExitButtonClicked()
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

    qApp->quit();

}
Widget::~Widget()
{
    delete ui;
}
void Widget::InitLayout()
{
    const int BTN_W = 150;
    const int BTN_H = 80;

    for (int i = 0; i < _Mybtns.size(); ++i)
    {
        MyBtn* btn = _Mybtns[i];
        btn->setFixedSize(BTN_W, BTN_H);

        int row = i / 3;
        int col = i % 3;

        m_gridLayout->addWidget(btn, row, col);
    }
}
