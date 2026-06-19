/**
 * \file  JsonlModel.cpp
 * \brief Implements the table model that loads and exposes JSONL/log records.
 */


#include "JsonlModel.h"

#include "Utils/File.h"
#include "LogLevelStyle.h"
#include "Utils/Timestamp.h"

#include <QBuffer>
#include <QBrush>
#include <QColor>
#include <QFile>
#include <QIcon>
#include <QJsonParseError>
#include <QPainter>
#include <QPen>
#include <QPixmap>

#include <utility>

//-------------------------------------------------------------------------------------------------
namespace
{
QIcon
invalidRowIcon()
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
JsonlModel::JsonlModel(
    QObject *parent
) :
    QAbstractTableModel(parent)
{
}

//-------------------------------------------------------------------------------------------------
int
JsonlModel::rowCount(
    const QModelIndex &parent
) const
{
    return parent.isValid() ? 0 : static_cast<int>(_records.size());
}

//-------------------------------------------------------------------------------------------------
int
JsonlModel::columnCount(
    const QModelIndex &parent
) const
{
    return parent.isValid() ? 0 : static_cast<int>(_columns.size());
}

//-------------------------------------------------------------------------------------------------
QVariant
JsonlModel::data(
    const QModelIndex &index,
    int               role
) const
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
                return file_utils::humanFileSize(kb * 1024);
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
        return datetime_utils::formatTimestamp(record.value(column));
    }

    return record.value(column);
}

//-------------------------------------------------------------------------------------------------
QVariant
JsonlModel::headerData(
    int             section,
    Qt::Orientation orientation,
    int             role
) const
{
    if (role != Qt::DisplayRole && role != Qt::ToolTipRole) {
        return {};
    }

    if (orientation == Qt::Horizontal) {
        const QString column = _columns.value(section);

        if (role == Qt::ToolTipRole && column == QStringLiteral("mem_usage_kb") && _memoryStats.hasValues) {
            return QStringLiteral(
                "<table>"
                "<tr><td align=\"right\"><b>min</b></td><td>&nbsp;%1</td></tr>"
                "<tr><td align=\"right\"><b>max</b></td><td>&nbsp;%2</td></tr>"
                "<tr><td align=\"right\"><b>average</b></td><td>&nbsp;%3</td></tr>"
                "</table>"
            )
                .arg(file_utils::humanFileSize(_memoryStats.minKb * 1024))
                .arg(file_utils::humanFileSize(_memoryStats.maxKb * 1024))
                .arg(file_utils::humanFileSize(static_cast<qint64>(_memoryStats.averageKb * 1024.0 + 0.5)));
        }

        return role == Qt::DisplayRole ? column : QVariant();
    }

    return role == Qt::DisplayRole ? section + 1 : QVariant();
}

//-------------------------------------------------------------------------------------------------
bool
JsonlModel::loadFile(
    const QString          &fileName,
    QString                *outError,
    const ProgressCallback &progressCallback
)
{
    if (outError != nullptr) {
        outError->clear();
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        if (outError != nullptr) {
            *outError = file.errorString();
        }

        return false;
    }

    return loadDevice(&file, fileName, progressCallback);
}

//-------------------------------------------------------------------------------------------------
bool
JsonlModel::loadJsonlData(
    const QByteArray       &data,
    const QString          &sourceFileName,
    QString                *outError,
    const ProgressCallback &progressCallback
)
{
    if (outError != nullptr) {
        outError->clear();
    }

    QBuffer buffer;
    buffer.setData(data);

    if (!buffer.open(QIODevice::ReadOnly)) {
        if (outError != nullptr) {
            *outError = buffer.errorString();
        }

        return false;
    }

    return loadDevice(&buffer, sourceFileName, progressCallback);
}

//-------------------------------------------------------------------------------------------------
bool
JsonlModel::loadDevice(
    QIODevice              *device,
    const QString          &sourceFileName,
    const ProgressCallback &progressCallback
)
{
    if (device == nullptr || !device->isOpen()) {
        return false;
    }

    beginResetModel();

    _records.clear();
    _fileName = sourceFileName;
    _invalidRowsCount = 0;
    _levelCounts.clear();
    _memoryStats = {};
    _memoryUsageTotalKb = 0.0;

    const qint64 totalBytes = device->size();
    qint64 readBytes = 0;
    qint64 nextProgressBytes = 0;
    int lineNo = 0;

    if (progressCallback) {
        progressCallback(0, totalBytes);
    }

    while (!device->atEnd()) {
        ++lineNo;

        const QByteArray lineBytes = device->readLine();
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

            bool memoryOk = false;
            const qint64 memoryKb = record.value(QStringLiteral("mem_usage_kb")).trimmed().toLongLong(&memoryOk);

            if (memoryOk && memoryKb >= 0) {
                if (!_memoryStats.hasValues) {
                    _memoryStats.minKb = memoryKb;
                    _memoryStats.maxKb = memoryKb;
                    _memoryStats.hasValues = true;
                } else {
                    _memoryStats.minKb = qMin(_memoryStats.minKb, memoryKb);
                    _memoryStats.maxKb = qMax(_memoryStats.maxKb, memoryKb);
                }

                ++_memoryStats.count;
                _memoryUsageTotalKb += static_cast<double>(memoryKb);
                _memoryStats.averageKb = _memoryUsageTotalKb / _memoryStats.count;
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
const JsonlRecord *
JsonlModel::recordAt(
    int row
) const
{
    if (row < 0 || row >= _records.size()) {
        return nullptr;
    }

    return &_records.at(row);
}

//-------------------------------------------------------------------------------------------------
QString
JsonlModel::fileName() const
{
    return _fileName;
}

//-------------------------------------------------------------------------------------------------
int
JsonlModel::invalidRowsCount() const
{
    return _invalidRowsCount;
}
//-------------------------------------------------------------------------------------------------
QMap<QString, int>
JsonlModel::levelCounts() const
{
    return _levelCounts;
}

//-------------------------------------------------------------------------------------------------
JsonlModel::MemoryStats
JsonlModel::memoryStats() const
{
    return _memoryStats;
}
//-------------------------------------------------------------------------------------------------
