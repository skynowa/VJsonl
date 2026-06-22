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
QIcon
calendarIcon()
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
QIcon
copyIcon()
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
QIcon
copyFormattedIcon()
{
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(QColor(90, 90, 90), 1));
    painter.setBrush(QColor(245, 245, 245));
    painter.drawRect(QRect(5, 1, 9, 11));
    painter.setBrush(QColor(225, 238, 255));
    painter.drawRect(QRect(2, 4, 11, 11));

    painter.setPen(QColor(25, 95, 185));
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(8);
    font.setStyleHint(QFont::Monospace);
    painter.setFont(font);
    painter.drawText(QRect(2, 4, 11, 11), Qt::AlignCenter, QStringLiteral("{}"));

    return QIcon(pixmap);
}

//-------------------------------------------------------------------------------------------------
QIcon
sessionIcon()
{
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(QColor(75, 85, 95), 1));
    painter.setBrush(QColor(225, 238, 255));
    painter.drawRect(QRect(1, 2, 14, 12));

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(45, 110, 190));
    painter.drawRect(QRect(2, 3, 12, 3));

    painter.setPen(QColor(105, 120, 135));
    painter.drawLine(6, 6, 6, 13);
    painter.drawLine(10, 6, 10, 13);
    painter.drawLine(2, 9, 14, 9);

    QPixmap activePixmap = pixmap;
    QPainter activePainter(&activePixmap);
    activePainter.setRenderHint(QPainter::Antialiasing);
    activePainter.setPen(QPen(QColor(25, 105, 45), 1));
    activePainter.setBrush(QColor(40, 170, 75));
    activePainter.drawEllipse(QRect(7, 7, 8, 8));
    activePainter.setPen(QPen(Qt::white, 1.5));
    activePainter.drawLine(9, 11, 11, 13);
    activePainter.drawLine(11, 13, 14, 9);

    QIcon icon;
    icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
    icon.addPixmap(activePixmap, QIcon::Normal, QIcon::On);
    return icon;
}
}
//-------------------------------------------------------------------------------------------------
