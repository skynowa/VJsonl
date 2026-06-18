/**
 * \file  Utils/Icon.cpp
 * \brief Implements shared icon helpers.
 */


#include "Utils/Icon.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPixmap>

//-------------------------------------------------------------------------------------------------
namespace icon_utils
{
QIcon calendarIcon()
{
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QColor(90, 90, 90));
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(QRect(1, 2, 14, 13), 2, 2);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(210, 0, 0));
    painter.drawRoundedRect(QRect(1, 2, 14, 5), 2, 2);
    painter.drawRect(QRect(1, 5, 14, 2));

    painter.setPen(QColor(40, 40, 40));
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(7);
    painter.setFont(font);
    painter.drawText(QRect(1, 6, 14, 9), Qt::AlignCenter, QStringLiteral("17"));

    return QIcon(pixmap);
}

//-------------------------------------------------------------------------------------------------
QIcon copyIcon()
{
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(QColor(90, 90, 90), 1));
    painter.setBrush(QColor(245, 245, 245));
    painter.drawRect(QRect(5, 2, 8, 10));
    painter.drawRect(QRect(2, 5, 8, 9));

    return QIcon(pixmap);
}

//-------------------------------------------------------------------------------------------------
QIcon copyFormattedIcon()
{
    QPixmap pixmap = copyIcon().pixmap(16, 16);

    QPainter painter(&pixmap);
    painter.setPen(QColor(35, 95, 170));
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(7);
    painter.setFont(font);
    painter.drawText(QRect(8, 8, 8, 8), Qt::AlignCenter, QStringLiteral("F"));

    return QIcon(pixmap);
}
}
//-------------------------------------------------------------------------------------------------
