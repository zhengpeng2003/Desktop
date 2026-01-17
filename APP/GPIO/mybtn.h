#ifndef MYBTN_H
#define MYBTN_H

#include <QPushButton>
#include <QPixmap>
#include <QString>

class MyBtn : public QPushButton
{
    Q_OBJECT
public:
    explicit MyBtn(const QString& openImgPath,
                   const QString& closeImgPath,
                   const QString& statePath,
                   QWidget* parent = nullptr);

private slots:
    void onClicked();

private:
    bool readStateFromPath() const;
    void writeStateToPath(bool on);
    void updateIcon(bool on);

private:
    QPixmap m_openPixmap;
    QPixmap m_closePixmap;
    QString m_statePath;
};

#endif // MYBTN_H
