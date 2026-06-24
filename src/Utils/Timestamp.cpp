/**
 * \file  Utils/Timestamp.cpp
 * \brief Implements timestamp parsing and formatting helpers.
 */


#include "Utils/Timestamp.h"

#include <QRegularExpression>
#include <QStringList>

//-------------------------------------------------------------------------------------------------
namespace
{
//-------------------------------------------------------------------------------------------------
// Timestamp pattern and offset parsing
//-------------------------------------------------------------------------------------------------
const QRegularExpression &
timestampPattern()
{
    static const QRegularExpression pattern(
        QStringLiteral(R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})(?:\.(\d+))?(Z|[+-]\d{2}:?\d{2})?\)?$)")
    );

    return pattern;
}

int
offsetSeconds(
    const QString &offsetText
)
{
    if (offsetText.isEmpty() || offsetText == QStringLiteral("Z")) {
        return 0;
    }

    const int sign = offsetText.startsWith(QLatin1Char('-')) ? -1 : 1;
    const QString digits = offsetText.mid(1).remove(QLatin1Char(':'));

    if (digits.size() != 4) {
        return 0;
    }

    bool hoursOk = false;
    bool minutesOk = false;
    const int hours = digits.left(2).toInt(&hoursOk);
    const int minutes = digits.mid(2, 2).toInt(&minutesOk);

    if (!hoursOk || !minutesOk) {
        return 0;
    }

    return sign * ((hours * 60 + minutes) * 60);
}
}

//-------------------------------------------------------------------------------------------------
// Timestamp parsing and display formatting
//-------------------------------------------------------------------------------------------------
namespace datetime_utils
{
QDateTime
parseTimestamp(
    const QString &text
)
{
    const QRegularExpressionMatch match = timestampPattern().match(text.trimmed());

    if (!match.hasMatch()) {
        return {};
    }

    const QDate date(match.captured(1).toInt(), match.captured(2).toInt(), match.captured(3).toInt());
    const QString millisecondsText = match.captured(7).leftJustified(3, QLatin1Char('0')).left(3);
    const QTime time(
        match.captured(4).toInt(),
        match.captured(5).toInt(),
        match.captured(6).toInt(),
        millisecondsText.toInt()
    );

    if (!date.isValid() || !time.isValid()) {
        return {};
    }

    const QString offsetText = match.captured(8);

    if (offsetText.isEmpty()) {
        return QDateTime(date, time);
    }

    if (offsetText == QStringLiteral("Z")) {
        return QDateTime(date, time, Qt::UTC);
    }

    QDateTime timestamp(date, time, Qt::OffsetFromUTC);
    timestamp.setOffsetFromUtc(offsetSeconds(offsetText));
    return timestamp;
}

//-------------------------------------------------------------------------------------------------
QString
formatTimestamp(
    const QString &text
)
{
    static const QStringList months {
        QStringLiteral("jan"),
        QStringLiteral("feb"),
        QStringLiteral("mar"),
        QStringLiteral("apr"),
        QStringLiteral("may"),
        QStringLiteral("jun"),
        QStringLiteral("jul"),
        QStringLiteral("aug"),
        QStringLiteral("sep"),
        QStringLiteral("oct"),
        QStringLiteral("nov"),
        QStringLiteral("dec")
    };

    const QDateTime timestamp = parseTimestamp(text);

    if (!timestamp.isValid()) {
        return text;
    }

    const QDate date = timestamp.date();
    const QTime time = timestamp.time();

    return QStringLiteral("%1-%2-%3 %4.%5")
        .arg(date.year(), 4, 10, QLatin1Char('0'))
        .arg(months.at(date.month() - 1))
        .arg(date.day(), 2, 10, QLatin1Char('0'))
        .arg(time.toString(QStringLiteral("HH:mm:ss")))
        .arg(time.msec(), 3, 10, QLatin1Char('0'));
}
}
//-------------------------------------------------------------------------------------------------
