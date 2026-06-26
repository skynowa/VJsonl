/**
 * \file  JsonlModel.h
 * \brief Declares the table model for parsed JSONL/log records.
 */


#pragma once

#include "JsonlRecord.h"

#include <QAbstractTableModel>
#include <QByteArray>
#include <QHash>
#include <QStringList>
#include <QVector>

#include <functional>

class QIODevice;
//-------------------------------------------------------------------------------------------------
class JsonlModel final :
    public QAbstractTableModel
{
    Q_OBJECT

public:
    using ProgressCallback = std::function<void(qint64 current, qint64 total)>;

    // Statistics collected while loading data
    struct MemoryStats
    {
        bool   hasValues {};
        int    count {};
        qint64 minKb {};
        qint64 maxKb {};
        double averageKb {};
    };

    explicit JsonlModel(QObject *parent = nullptr);
    Q_DISABLE_COPY_MOVE(JsonlModel)

    // QAbstractTableModel interface
    int      rowCount(const QModelIndex &parent = {}) const override;
    int      columnCount(const QModelIndex &parent = {}) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)
                const override;

    // Data loading
    bool     loadFile(const QString &fileName, QString *outError = nullptr,
                const ProgressCallback &progressCallback = {});
    bool     loadJsonlData(const QByteArray &data, const QString &sourceFileName,
                QString *outError = nullptr, const ProgressCallback &progressCallback = {});

    // Loaded data accessors
    const JsonlRecord *recordAt(int row) const;
    QString            fileName() const;
    int                invalidRowsCount() const;
    QHash<QString, int> levelCounts() const;
    MemoryStats        memoryStats() const;

private:
    bool loadDevice(QIODevice *device, const QString &sourceFileName, QString *outError,
        const ProgressCallback &progressCallback);

private:
    // Loaded records and table shape
    QVector<JsonlRecord> _records;
    QStringList          _columns {
        // QStringLiteral("line"),

        QStringLiteral("ts"),
        QStringLiteral("timestamp"),
        QStringLiteral("@timestamp"),

        QStringLiteral("valid"),
        QStringLiteral("project"),
        QStringLiteral("app"),
        QStringLiteral("proc_name"),
        QStringLiteral("module"),
        QStringLiteral("log_name"),
        QStringLiteral("level"),
        // QStringLiteral("error"),
        QStringLiteral("msg"),
        QStringLiteral("query"),
        QStringLiteral("page"),
        QStringLiteral("request"),
        QStringLiteral("descr"),
        QStringLiteral("request_time"),
        // QStringLiteral("message"),
        // QStringLiteral("service"),
        QStringLiteral("mem_usage_kb"),
        // QStringLiteral("request_id"),
        QStringLiteral("backtrace"),
        QStringLiteral("raw")
    };

    // Loaded file metadata and aggregate statistics
    QString              _fileName;
    int                  _invalidRowsCount {};
    QHash<QString, int>  _levelCounts;
    MemoryStats          _memoryStats;
    double               _memoryUsageTotalKb {};
};
//-------------------------------------------------------------------------------------------------
