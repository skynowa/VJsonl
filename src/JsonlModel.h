#pragma once

#include "JsonlRecord.h"

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>

#include <functional>

class JsonlModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    using ProgressCallback = std::function<void(qint64 current, qint64 total)>;

    explicit JsonlModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool loadFile(
        const QString &fileName,
        QString *outError = nullptr,
        const ProgressCallback &progressCallback = {}
    );

    const JsonlRecord *recordAt(int row) const;
    QString fileName() const;
    int invalidRowsCount() const;

private:
    QVector<JsonlRecord> _records;
    QStringList          _columns {
        QStringLiteral("line"),
        QStringLiteral("valid"),
        QStringLiteral("ts"), // timestamp
        QStringLiteral("level"),
        QStringLiteral("error"),
        QStringLiteral("log_name"),
        QStringLiteral("msg"),
        QStringLiteral("query"),
        // QStringLiteral("message"),
        QStringLiteral("proc_name"),
        QStringLiteral("service"),
        QStringLiteral("mem_usage_kb"),
        QStringLiteral("request_id"),
        QStringLiteral("backtrace"),
        QStringLiteral("raw")
    };
    QString              _fileName;
    int                  _invalidRowsCount {};
};
