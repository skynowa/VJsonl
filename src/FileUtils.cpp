#include "FileUtils.h"

#include <QStringList>

QString humanFileSize(qint64 bytes)
{
    static const QStringList units {
        QStringLiteral("B"),
        QStringLiteral("KB"),
        QStringLiteral("MB"),
        QStringLiteral("GB"),
        QStringLiteral("TB")
    };

    double size = static_cast<double>(bytes);
    int unit = 0;

    while (size >= 1000.0 && unit + 1 < units.size()) {
        size /= 1000.0;
        ++unit;
    }

    if (unit == 0) {
        return QStringLiteral("%1 %2").arg(bytes).arg(units.at(unit));
    }

    return QStringLiteral("%1 %2").arg(size, 0, 'f', 1).arg(units.at(unit));
}
