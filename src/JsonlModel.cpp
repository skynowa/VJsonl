/**
 * \file  JsonlModel.cpp
 * \brief Implements the table model that loads and exposes JSONL/log records.
 */


#include "JsonlModel.h"

#include "FileUtils.h"
#include "LogLevelStyle.h"

#include <QBrush>
#include <QColor>
#include <QFile>
#include <QIcon>
#include <QJsonParseError>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QRegularExpression>

#include <utility>

//-------------------------------------------------------------------------------------------------
namespace
{

QString formatTimestamp(const QString &text)
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
    static const QRegularExpression timestampPattern(
        QStringLiteral(R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}:\d{2}:\d{2})(?:\.(\d+))?(?:Z|[+-]\d{2}:?\d{2})?\)?$)")
    );

    const QRegularExpressionMatch match = timestampPattern.match(text.trimmed());

    if (!match.hasMatch()) {
        return text;
    }

    bool ok = false;
    const int month = match.captured(2).toInt(&ok);

    if (!ok || month < 1 || month > months.size()) {
        return text;
    }

    const QString milliseconds = match.captured(5).leftJustified(3, QLatin1Char('0')).left(3);

    return QStringLiteral("%1-%2-%3 %4.%5")
        .arg(match.captured(1), months.at(month - 1), match.captured(3), match.captured(4), milliseconds);
}

QIcon invalidRowIcon()
{
    static const QIcon icon = [] {
        QPixmap pixmap(14, 14);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        QPen pen(QColor(210, 0, 0));
        pen.setWidth(3);
        pen.setCapStyle(Qt::RoundCap);
        painter.setPen(pen);
        painter.drawLine(3, 3, 11, 11);
        painter.drawLine(11, 3, 3, 11);

        return QIcon(pixmap);
    }();

    return icon;
}

}

//-------------------------------------------------------------------------------------------------
JsonlModel::JsonlModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

//-------------------------------------------------------------------------------------------------
int JsonlModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _records.size();
}

//-------------------------------------------------------------------------------------------------
int JsonlModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _columns.size();
}

//-------------------------------------------------------------------------------------------------
QVariant JsonlModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto &record = _records.at(index.row());
    const QString &column = _columns.at(index.column());
    const QString level = record.value(QStringLiteral("level"));
    const bool rowIsFatal = level.compare(QStringLiteral("fatal"), Qt::CaseInsensitive) == 0;
    const bool rowHasError =
        level.compare(QStringLiteral("error"), Qt::CaseInsensitive) == 0
        || !record.value(QStringLiteral("error")).isEmpty();
    const bool rowIsDebug = level.compare(QStringLiteral("debug"), Qt::CaseInsensitive) == 0;

    if (role == Qt::BackgroundRole && rowIsFatal) {
        return QBrush(QColor(170, 0, 255));
    }

    if (role == Qt::BackgroundRole && rowHasError) {
        return QBrush(QColor(200, 0, 0));
    }

    if (role == Qt::BackgroundRole && rowIsDebug) {
        return QBrush(QColor(215, 245, 215));
    }

    if (role == Qt::ForegroundRole && (rowIsFatal || rowHasError)) {
        return QBrush(Qt::white);
    }

    if (
        role == Qt::ForegroundRole
        && (
            column == QStringLiteral("app")
            || column == QStringLiteral("proc_name")
            || column == QStringLiteral("module")
        )
    ) {
        return QBrush(QColor(80, 170, 80));
    }

    if (role == Qt::DecorationRole && column == QStringLiteral("level")) {
        return LogLevelStyle::iconForLevel(level);
    }

    if (role == Qt::DecorationRole && column == QStringLiteral("valid") && !record.valid) {
        return invalidRowIcon();
    }

    auto cellText = [&record, &column]() -> QString {
        if (column == QStringLiteral("error")) {
            return record.value(column);
        }

        if (column == QStringLiteral("valid")) {
            return QStringLiteral("no: %1").arg(record.error);
        }

        if (column == QStringLiteral("raw")) {
            return record.raw;
        }

        if (column == QStringLiteral("mem_usage_kb")) {
            bool ok = false;
            const qint64 kb = record.value(column).toLongLong(&ok);

            if (ok) {
                return humanFileSize(kb * 1024);
            }

            return record.value(column);
        }

        return record.value(column);
    };

    if (role != Qt::DisplayRole && role != Qt::ToolTipRole) {
        return {};
    }

    if (column == QStringLiteral("line")) {
        return record.lineNo;
    }

    if (column == QStringLiteral("error")) {
        return cellText();
    }

    if (column == QStringLiteral("valid")) {
        return role == Qt::ToolTipRole && !record.valid ? cellText() : QString();
    }

    if (column == QStringLiteral("raw")) {
        return cellText();
    }

    if (column == QStringLiteral("mem_usage_kb")) {
        return cellText();
    }

    if (role == Qt::DisplayRole && column == QStringLiteral("ts")) {
        return formatTimestamp(record.value(column));
    }

    return record.value(column);
}

//-------------------------------------------------------------------------------------------------
QVariant JsonlModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return {};
    }

    if (orientation == Qt::Horizontal) {
        return _columns.value(section);
    }

    return section + 1;
}

//-------------------------------------------------------------------------------------------------
bool JsonlModel::loadFile(
    const QString &fileName,
    QString *outError,
    const ProgressCallback &progressCallback
)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        if (outError != nullptr) {
            *outError = file.errorString();
        }

        return false;
    }

    beginResetModel();

    _records.clear();
    _fileName = fileName;
    _invalidRowsCount = 0;
    _levelCounts.clear();

    const qint64 totalBytes = file.size();
    qint64 readBytes = 0;
    qint64 nextProgressBytes = 0;
    int lineNo = 0;

    if (progressCallback) {
        progressCallback(0, totalBytes);
    }

    while (!file.atEnd()) {
        ++lineNo;

        const QByteArray lineBytes = file.readLine();
        readBytes += lineBytes.size();

        JsonlRecord record;
        record.lineNo = lineNo;
        record.raw = QString::fromUtf8(lineBytes);

        if (record.raw.endsWith(QLatin1Char('\n'))) {
            record.raw.chop(1);
        }

        if (record.raw.endsWith(QLatin1Char('\r'))) {
            record.raw.chop(1);
        }

        if (progressCallback && readBytes >= nextProgressBytes) {
            progressCallback(qMin(readBytes, totalBytes), totalBytes);
            nextProgressBytes = readBytes + 1024 * 1024;
        }

        if (record.raw.trimmed().isEmpty()) {
            record.valid = false;
            record.error = QStringLiteral("empty line");
            ++_invalidRowsCount;
            _records.push_back(std::move(record));
            continue;
        }

        const QString trimmed = record.raw.trimmed();

        if (!trimmed.startsWith(QLatin1Char('{')) && !trimmed.startsWith(QLatin1Char('['))) {
            record.valid = true;
            _records.push_back(std::move(record));
            continue;
        }

        QJsonParseError parseError {};
        record.doc = QJsonDocument::fromJson(record.raw.toUtf8(), &parseError);
        record.valid = parseError.error == QJsonParseError::NoError;

        if (!record.valid) {
            record.error = parseError.errorString();
            ++_invalidRowsCount;
        } else {
            const QString level = record.value(QStringLiteral("level")).trimmed().toLower();

            if (!level.isEmpty()) {
                ++_levelCounts[level];
            }
        }

        _records.push_back(std::move(record));
    }

    endResetModel();

    if (progressCallback) {
        progressCallback(totalBytes, totalBytes);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
const JsonlRecord *JsonlModel::recordAt(int row) const
{
    if (row < 0 || row >= _records.size()) {
        return nullptr;
    }

    return &_records.at(row);
}

//-------------------------------------------------------------------------------------------------
QString JsonlModel::fileName() const
{
    return _fileName;
}

//-------------------------------------------------------------------------------------------------
int JsonlModel::invalidRowsCount() const
{
    return _invalidRowsCount;
}
//-------------------------------------------------------------------------------------------------
QMap<QString, int> JsonlModel::levelCounts() const
{
    return _levelCounts;
}
//-------------------------------------------------------------------------------------------------
