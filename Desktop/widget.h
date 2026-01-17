#ifndef WIDGET_H
#define WIDGET_H
#include <QSettings>
#include <QWidget>
#include <globalconfig.h>
#include <QDebug>
#include <QPainter>
#include <QImageReader>
#include <QtMath>
#include <QMessageBox>
#include <QProcess>
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
    QVector<QRect> calcAppRects(quint32 screenW,quint32 screenH,int appCount);
    bool StartApp(const AppInfo &appinfo);//
    void AutoExe(quint8 AutoID);//开机自启动程序
    void InitInfo();
    ~Widget();
protected:
    virtual void paintEvent(QPaintEvent *event)override;
    virtual void mousePressEvent(QMouseEvent *event)override;
private:

    Ui::Widget *ui;
    Screen _Screen;
    QSettings _Set;
    QVector<AppInfo> _AppInfos;//App的信息
    QVector<QRect> _AppRects;//App框的区域
    QProcess *m_appProcess = nullptr;

};
#endif // WIDGET_H
