/**
 * \file  JsonlModel.cpp
 * \brief Implements the table model that loads and exposes JSONL/log records.
 */


#include "JsonlModel.h"

#include "Utils/File.h"
#include "Utils/Icon.h"
#include "LogLevelStyle.h"
#include "Utils/Timestamp.h"

#include <QBuffer>
#include <QBrush>
#include <QColor>
#include <QFile>
#include <QFileDevice>
#include <QIcon>
#include <QJsonParseError>

#include <algorithm>
#include <utility>

//-------------------------------------------------------------------------------------------------
// Construction
//-------------------------------------------------------------------------------------------------
JsonlModel::JsonlModel(
    QObject *parent
) :
    QAbstractTableModel(parent)
{
}

//-------------------------------------------------------------------------------------------------
// QAbstractTableModel interface
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
    if (
        !index.isValid()
        || index.model() != this
        || index.row() < 0
        || index.row() >= _records.size()
        || index.column() < 0
        || index.column() >= _columns.size()
    ) {
        return {};
    }

    const auto &record = _records.at(index.row());
    const QString &column = _columns.at(index.column());

    if (role == Qt::UserRole && column == QStringLiteral("ts")) {
        return record.timestamp;
    }

    if (role == Qt::BackgroundRole || role == Qt::ForegroundRole || role == Qt::DecorationRole) {
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
            return icon_utils::invalidRowIcon();
        }
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
// Data loading
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

    return loadDevice(&file, fileName, outError, progressCallback);
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

    return loadDevice(&buffer, sourceFileName, outError, progressCallback);
}

//-------------------------------------------------------------------------------------------------
bool
JsonlModel::loadDevice(
    QIODevice              *device,
    const QString          &sourceFileName,
    QString                *outError,
    const ProgressCallback &progressCallback
)
{
    if (device == nullptr || !device->isOpen()) {
        return false;
    }

    QVector<JsonlRecord> records;
    QHash<QString, int> levelCounts;
    MemoryStats memoryStats;
    double memoryUsageTotalKb = 0.0;
    int invalidRowsCount = 0;

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
            progressCallback((std::min)(readBytes, totalBytes), totalBytes);
            nextProgressBytes = readBytes + 1024 * 1024;
        }

        if (record.raw.trimmed().isEmpty()) {
            record.valid = false;
            record.error = QStringLiteral("empty line");
            ++invalidRowsCount;
            records.push_back(std::move(record));
            continue;
        }

        const QString trimmed = record.raw.trimmed();

        if (!trimmed.startsWith(QLatin1Char('{')) && !trimmed.startsWith(QLatin1Char('['))) {
            record.valid = true;
            records.push_back(std::move(record));
            continue;
        }

        QJsonParseError parseError {};
        record.doc = QJsonDocument::fromJson(record.raw.toUtf8(), &parseError);
        record.valid = parseError.error == QJsonParseError::NoError && record.doc.isObject();

        if (!record.valid) {
            record.error = parseError.error == QJsonParseError::NoError
                ? QStringLiteral("JSON row is not an object")
                : parseError.errorString();
            ++invalidRowsCount;
        } else {
            const QString level = record.value(QStringLiteral("level")).trimmed().toLower();
            record.timestamp = datetime_utils::parseTimestamp(record.value(QStringLiteral("ts")));

            if (!level.isEmpty()) {
                ++levelCounts[level];
            }

            bool memoryOk = false;
            const qint64 memoryKb = record.value(QStringLiteral("mem_usage_kb")).trimmed().toLongLong(&memoryOk);

            if (memoryOk && memoryKb >= 0) {
                if (!memoryStats.hasValues) {
                    memoryStats.minKb = memoryKb;
                    memoryStats.maxKb = memoryKb;
                    memoryStats.hasValues = true;
                } else {
                    memoryStats.minKb = (std::min)(memoryStats.minKb, memoryKb);
                    memoryStats.maxKb = (std::max)(memoryStats.maxKb, memoryKb);
                }

                ++memoryStats.count;
                memoryUsageTotalKb += static_cast<double>(memoryKb);
                memoryStats.averageKb = memoryUsageTotalKb / memoryStats.count;
            }
        }

        records.push_back(std::move(record));
    }

    auto *fileDevice = qobject_cast<QFileDevice *>(device);

    if (fileDevice != nullptr && fileDevice->error() != QFileDevice::NoError) {
        if (outError != nullptr) {
            *outError = fileDevice->errorString();
        }

        return false;
    }

    beginResetModel();
    _records = std::move(records);
    _fileName = sourceFileName;
    _invalidRowsCount = invalidRowsCount;
    _levelCounts = std::move(levelCounts);
    _memoryStats = memoryStats;
    _memoryUsageTotalKb = memoryUsageTotalKb;
    endResetModel();

    if (progressCallback) {
        progressCallback(totalBytes, totalBytes);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
// Loaded data accessors
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
QHash<QString, int>
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
