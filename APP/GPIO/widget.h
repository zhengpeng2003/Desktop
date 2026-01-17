#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <mybtn.h>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    void InitMyBtns();
    void InitLayout();
    Widget(QWidget *parent = nullptr);
    ~Widget();
public slots:
    void onExitButtonClicked();
private:
    QVector<MyBtn*> _Mybtns;
    QGridLayout* m_gridLayout;
    Ui::Widget *ui;
};
#endif // WIDGET_H
