#include "widget.h"
#include "ui_widget.h"
#include <QDir>
#include <QMouseEvent>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget),//arm
    _Set(QCoreApplication::applicationDirPath() + "/AppInfo/Appinfo.ini", QSettings::IniFormat)
//_Set("../../Appinfo/Appinfo.ini",QSettings::IniFormat)
{
    //InitInfo();
    QFile f(QCoreApplication::applicationDirPath() +"/tmp/desktop_state");
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        f.write("desktop");
        f.close();
    }
    ui->setupUi(this);
    //this->resize(_Screen.Screen_Width,_Screen.Screen_Height);

    //arm
    // 2️⃣ 禁止窗口边框（嵌入式）
    setWindowFlags(Qt::FramelessWindowHint);
    // 3️⃣ 不要 resize，用全屏
    // resize 在 framebuffer 下极易出问题
    showFullScreen();

    // 4️⃣ 强制背景
    setAutoFillBackground(true);
    setStyleSheet("background-color:blue;");

    // 5️⃣ 最后再做业务初始化
    InitInfo();
}

void Widget::InitInfo()
{
    //初始化屏幕
    _Set.beginGroup("Screen");
    _Screen.Screen_Height=_Set.value("Screen_Height").toUInt();
    _Screen.Screen_Width=_Set.value("Screen_Width").toUInt();//Screen/Screen_Height读取的位置类似于
    qDebug() << "cwd:" << QDir::currentPath();//获取的那当前文件执行位置
    _Set.endGroup();

    //初始化APP信息
    QStringList groups = _Set.childGroups();
    for(auto group: groups)
    {

        if (!group.startsWith("APP"))
        continue;
        _Set.beginGroup(group);
        quint8 id = _Set.value("Id").toUInt();
        QString name = _Set.value("Name").toString();
        QString path = _Set.value("Path").toString();
        QString Log = _Set.value("Log").toString();
        _Set.endGroup();
        qDebug() << id << name << path << Log;
        AppInfo TempInfo(id,name,path,Log);
        _AppInfos.append(TempInfo);
    }
    //初始化矩形
    _AppRects=calcAppRects(_Screen.Screen_Width,_Screen.Screen_Height,_AppInfos.count());

    //开机自启动
    _Set.beginGroup("Autoexe");
    quint8 AutoID=_Set.value("Id").toUInt();
    qDebug() << "AutoID:" << AutoID;//获取自启动id
    _Set.endGroup();
    if(AutoID!=0)
    AutoExe(AutoID);

}
QVector<QRect> Widget::calcAppRects(quint32 screenW,
                                    quint32 screenH,
                                    int appCount)
{
    QVector<QRect> rects;
    if (appCount <= 0)
        return rects;

    // 🔒 固定 3×3
    const quint32 cols = 3;
    const quint32 rows = 3;

    quint32 margin  = screenW * 0.03;  // 原来 0.05 → 0.03
    quint32 spacing = screenW * 0.02;  // 原来 0.03 → 0.02

    quint32 usableW = screenW - margin * 2 - spacing * (cols - 1);
    quint32 usableH = screenH - margin * 2 - spacing * (rows - 1);

    quint32 itemW = usableW / cols;
    quint32 itemH = usableH / rows;


    // 最多画 9 个
    int count = qMin(appCount, 9);

    for (int i = 0; i < count; ++i)
    {
        int row = i / cols;   // 行：先 0 行
        int col = i % cols;   // 列：先向右

        int x = margin + col * (itemW + spacing);
        int y = margin + row * (itemH + spacing);

        // ✅ width / height 修正
        rects.append(QRect(x, y, itemW, itemH));

        qDebug() << "i" << i << "row" << row << "col" << col;
    }

    return rects;
}

bool Widget::StartApp(const AppInfo &appinfo)
{
    QString exePath = QCoreApplication::applicationDirPath() + "/" + appinfo.Path;
    if (!QFile::exists(exePath))
        return false;

    // 先写状态再启动，防止启动失败却留下 app 状态
    QString stateFile = QCoreApplication::applicationDirPath() + "/tmp/desktop_state";
    QFile f(stateFile + ".tmp");
    if (f.open(QIODevice::WriteOnly)) {
        f.write("app");
        f.close();
        ::rename(qPrintable(stateFile + ".tmp"), qPrintable(stateFile));
    }

    bool ok = QProcess::startDetached(exePath);
    if (!ok) {
        // 启动失败再把状态改回来
        QFile::remove(stateFile);
        return false;
    }
    qApp->quit();          // 成功就退出桌面
    return true;
}


void Widget::AutoExe(quint8 AutoID)
{
    for (int i = 0; i <_AppInfos.size(); ++i)
    {
        if (_AppInfos[i].Id==AutoID)
        {
            qDebug() << "Auto App index:" << i;
            StartApp(_AppInfos[i]);
        }
    }
}


Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 开启抗锯齿 & 平滑
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QPixmap bg(":/images/background.jpg");
    //查看支持什么图片文件
    qDebug() << QImageReader::supportedImageFormats();

    // 按窗口大小平滑缩放
    bg = bg.scaled(this->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);  // 填满窗口Qt::SmoothTransformation);
    painter.drawPixmap(0,0,bg);

    //画APP图表
    for (int i = 0; i < _AppInfos.size(); ++i)
    {
        const QRect& rect = _AppRects[i];
        const AppInfo& app = _AppInfos[i];

        // 1️⃣ 画背景框（可选）
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 40));
        painter.drawRoundedRect(rect, 8, 8);

        // 2️⃣ 加载 LOGO
        QString baseDir = QCoreApplication::applicationDirPath();
        QDir dir(baseDir);
        QPixmap logo(dir.absolutePath() + "/"+app.Log);//arm
        //QPixmap logo(app.Log);
        if (!logo.isNull())
        {
            // 3️⃣ LOGO 区域（留边）
            QRect iconRect = rect.adjusted(
                rect.width() * 0.15,
                rect.height() * 0.15,
                -rect.width() * 0.15,
                -rect.height() * 0.25
                );

            // 4️⃣ 等比缩放 + 平滑
            QPixmap scaled = logo.scaled(
                iconRect.size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );

            // 5️⃣ 居中画
            QPoint center = iconRect.center() - QPoint(scaled.width()/2, scaled.height()/2);
            painter.drawPixmap(center, scaled);
        }

        // 6️⃣ 画名字
        painter.setPen(Qt::white);
        painter.drawText(
            QRect(rect.left(), rect.bottom() - 20, rect.width(), 30),
            Qt::AlignCenter,
            app.Name
            );
    }



}
void Widget::mousePressEvent(QMouseEvent *event)
{

    QPoint pos = event->pos();  // 鼠标点击坐标（窗口内）
    qDebug()<<pos;
    for (int i = 0; i < _AppRects.size(); ++i)
    {
        if (_AppRects[i].contains(pos))
        {
            qDebug() << "Clicked App index:" << i;

            if (i < _AppInfos.size())
            {
                if(!StartApp(_AppInfos[i]))
                {
                    QMessageBox::warning(this,"提示","程序打开失败，请重试！");
                }
            }
            break;
        }
    }
}

