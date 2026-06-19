/**
 * \file  JsonlModel.h
 * \brief Declares the table model for parsed JSONL/log records.
 */


#pragma once

#include "JsonlRecord.h"

#include <QAbstractTableModel>
#include <QByteArray>
#include <QMap>
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

    struct MemoryStats
    {
        bool   hasValues {};
        int    count {};
        qint64 minKb {};
        qint64 maxKb {};
        double averageKb {};
    };

    explicit JsonlModel(QObject *parent = nullptr);

    int      rowCount(const QModelIndex &parent = {}) const override;
    int      columnCount(const QModelIndex &parent = {}) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)
                const override;

    bool     loadFile(const QString &fileName, QString *outError = nullptr,
                const ProgressCallback &progressCallback = {});
    bool     loadJsonlData(const QByteArray &data, const QString &sourceFileName,
                QString *outError = nullptr, const ProgressCallback &progressCallback = {});

    const JsonlRecord *recordAt(int row) const;
    QString            fileName() const;
    int                invalidRowsCount() const;
    QMap<QString, int> levelCounts() const;
    MemoryStats        memoryStats() const;

private:
    bool loadDevice(QIODevice *device, const QString &sourceFileName,
        const ProgressCallback &progressCallback);

private:
    QVector<JsonlRecord> _records;
    QStringList          _columns {
        // QStringLiteral("line"),
        QStringLiteral("valid"),
        QStringLiteral("project"),
        QStringLiteral("app"),
        QStringLiteral("proc_name"),
        QStringLiteral("module"),
        QStringLiteral("log_name"),
        QStringLiteral("ts"), // timestamp
        QStringLiteral("level"),
        // QStringLiteral("error"),
        QStringLiteral("msg"),
        QStringLiteral("query"),
        QStringLiteral("descr"),
        QStringLiteral("request_time"),
        // QStringLiteral("message"),
        // QStringLiteral("service"),
        QStringLiteral("mem_usage_kb"),
        // QStringLiteral("request_id"),
        QStringLiteral("backtrace"),
        QStringLiteral("raw")
    };
    QString              _fileName;
    int                  _invalidRowsCount {};
    QMap<QString, int>   _levelCounts;
    MemoryStats          _memoryStats;
    double               _memoryUsageTotalKb {};
};
//-------------------------------------------------------------------------------------------------
