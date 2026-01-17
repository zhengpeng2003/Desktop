#include "mybtn.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

MyBtn::MyBtn(const QString& openImgPath,
             const QString& closeImgPath,
             const QString& statePath,
             QWidget* parent)
    : QPushButton(parent),
    m_openPixmap(openImgPath),
    m_closePixmap(closeImgPath),
    m_statePath(statePath)
{
    setFlat(true);
    setCursor(Qt::PointingHandCursor);

    // ⚠️ 只有有路径的按钮才读状态
    if (!m_statePath.trimmed().isEmpty())
        updateIcon(readStateFromPath());
    else
        updateIcon(false);

    connect(this, &QPushButton::clicked,
            this, &MyBtn::onClicked);
}

bool MyBtn::readStateFromPath() const
{
    if (m_statePath.trimmed().isEmpty())
        return false;

    QFile file(m_statePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "open state file failed:" << m_statePath;
        return false;
    }

    QString value = file.readAll().trimmed();
    return (value == "1");
}

void MyBtn::writeStateToPath(bool on)
{
    if (m_statePath.trimmed().isEmpty())
        return;

    QFile file(m_statePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "write state file failed:" << m_statePath;
        return;
    }

    file.write(on ? "1" : "0");
}

void MyBtn::updateIcon(bool on)
{
    setIcon(QIcon(on ? m_openPixmap : m_closePixmap));
    setIconSize(on ? m_openPixmap.size()
                   : m_closePixmap.size());
}

void MyBtn::onClicked()
{
    // ExitBtn（没路径）只负责发 clicked 信号
    if (m_statePath.trimmed().isEmpty())
        return;

    bool cur = readStateFromPath();
    bool next = !cur;

    writeStateToPath(next);
    updateIcon(next);
}

