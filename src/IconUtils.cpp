/**
 * \file  IconUtils.cpp
 * \brief Implements shared icon helpers.
 */


#include "IconUtils.h"

#include <QColor>
#include <QFont>
#include <QPainter>
#include <QPixmap>

//-------------------------------------------------------------------------------------------------
namespace IconUtils
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
}
//-------------------------------------------------------------------------------------------------
