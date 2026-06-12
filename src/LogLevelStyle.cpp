#include "LogLevelStyle.h"

#include <QColor>
#include <QPainter>
#include <QPixmap>

namespace
{
QIcon makeIcon(const QColor &color, const QString &label)
{
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(color);
    painter.setPen(color.darker(140));
    painter.drawEllipse(1, 1, 14, 14);

    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    font.setPixelSize(10);
    painter.setFont(font);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, label.left(1).toUpper());

    return QIcon(pixmap);
}
}

namespace LogLevelStyle
{
QIcon iconForLevel(const QString &level)
{
    if (level.compare(QStringLiteral("fatal"), Qt::CaseInsensitive) == 0) {
        return makeIcon(QColor(170, 0, 255), QStringLiteral("F"));
    }

    if (level.compare(QStringLiteral("error"), Qt::CaseInsensitive) == 0) {
        return makeIcon(QColor(200, 0, 0), QStringLiteral("E"));
    }

    if (
        level.compare(QStringLiteral("warn"), Qt::CaseInsensitive) == 0
        || level.compare(QStringLiteral("warning"), Qt::CaseInsensitive) == 0
    ) {
        return makeIcon(QColor(220, 145, 0), QStringLiteral("W"));
    }

    if (level.compare(QStringLiteral("debug"), Qt::CaseInsensitive) == 0) {
        return makeIcon(QColor(35, 150, 75), QStringLiteral("D"));
    }

    if (level.compare(QStringLiteral("info"), Qt::CaseInsensitive) == 0) {
        return makeIcon(QColor(40, 115, 210), QStringLiteral("I"));
    }

    if (level.compare(QStringLiteral("trace"), Qt::CaseInsensitive) == 0) {
        return makeIcon(QColor(115, 115, 115), QStringLiteral("T"));
    }

    return {};
}
}
