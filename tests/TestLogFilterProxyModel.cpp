/**
 * \file  TestLogFilterProxyModel.cpp
 * \brief Tests LogFilterProxyModel column, text, and timestamp filters.
 */


#include "JsonlModel.h"
#include "LogFilterProxyModel.h"

#include <QDir>
#include <QTemporaryFile>
#include <QTest>

namespace
{
//-------------------------------------------------------------------------------------------------
bool writeJsonlFile(QTemporaryFile *file)
{
    if (!file->open()) {
        return false;
    }

    file->write(R"({"project":"suppliers","app":"sync","proc_name":"worker","module":"loader","log_name":"access","ts":"2026-06-15T08:00:00.000-0500","level":"info","msg":"Loaded hotel","query":"select * from hotel"})");
    file->write("\n");
    file->write(R"({"project":"booked","app":"cms","proc_name":"api","module":"rooms","log_name":"audit","ts":"2026-06-15T09:30:00.000-0500","level":"error","msg":"Room failed","query":"update rooms set name = 'x'"})");
    file->write("\n");
    file->write(R"({"project":"suppliers","app":"sync","proc_name":"worker","module":"loader","log_name":"access","ts":"2026-06-15T11:00:00.000-0500","level":"debug","msg":"Cache warmed","query":"delete from cache"})");
    file->write("\n");
    file->close();
    return true;
}

//-------------------------------------------------------------------------------------------------
int columnByName(const QAbstractItemModel &model, const QString &name)
{
    for (int column = 0; column < model.columnCount(); ++column) {
        if (model.headerData(column, Qt::Horizontal).toString() == name) {
            return column;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------
QString valueAt(const QAbstractItemModel &model, int row, const QString &columnName)
{
    const int column = columnByName(model, columnName);
    Q_ASSERT(column >= 0);
    return model.data(model.index(row, column)).toString();
}
}

//-------------------------------------------------------------------------------------------------
class TestLogFilterProxyModel final : public QObject
{
    Q_OBJECT

private slots:
    void filtersByExactColumnsAndClears()
    {
        JsonlModel model;
        QTemporaryFile file(QDir::temp().filePath(QStringLiteral("vjson-proxy-test-XXXXXX.jsonl")));

        QVERIFY(writeJsonlFile(&file));
        QVERIFY(model.loadFile(file.fileName()));

        LogFilterProxyModel proxy;
        proxy.setSourceModel(&model);
        QCOMPARE(proxy.rowCount(), 3);

        proxy.setProjectFilter(QStringLiteral("suppliers"));
        QCOMPARE(proxy.rowCount(), 2);

        proxy.setLevelFilter(QStringLiteral("debug"));
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(valueAt(proxy, 0, QStringLiteral("msg")), QStringLiteral("Cache warmed"));

        proxy.setProjectFilter(QString());
        QCOMPARE(proxy.rowCount(), 1);

        proxy.setLevelFilter(QString());
        QCOMPARE(proxy.rowCount(), 3);
    }

    void filtersTextColumnsByContains()
    {
        JsonlModel model;
        QTemporaryFile file(QDir::temp().filePath(QStringLiteral("vjson-proxy-test-XXXXXX.jsonl")));

        QVERIFY(writeJsonlFile(&file));
        QVERIFY(model.loadFile(file.fileName()));

        LogFilterProxyModel proxy;
        proxy.setSourceModel(&model);

        proxy.setMsgTextFilter(QStringLiteral("hotel"));
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(valueAt(proxy, 0, QStringLiteral("project")), QStringLiteral("suppliers"));

        proxy.setQueryFilter(QStringLiteral("UPDATE rooms"));
        QCOMPARE(proxy.rowCount(), 0);

        proxy.setMsgTextFilter(QString());
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(valueAt(proxy, 0, QStringLiteral("project")), QStringLiteral("booked"));

        proxy.setQueryFilter(QString());
        QCOMPARE(proxy.rowCount(), 3);
    }

    void filtersByTimestampRange()
    {
        JsonlModel model;
        QTemporaryFile file(QDir::temp().filePath(QStringLiteral("vjson-proxy-test-XXXXXX.jsonl")));

        QVERIFY(writeJsonlFile(&file));
        QVERIFY(model.loadFile(file.fileName()));

        LogFilterProxyModel proxy;
        proxy.setSourceModel(&model);

        proxy.setTimestampRange(
            QDateTime(QDate(2026, 6, 15), QTime(9, 0), Qt::OffsetFromUTC, -5 * 60 * 60),
            true,
            QDateTime(QDate(2026, 6, 15), QTime(10, 0), Qt::OffsetFromUTC, -5 * 60 * 60),
            true
        );

        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(valueAt(proxy, 0, QStringLiteral("msg")), QStringLiteral("Room failed"));
    }
};

//-------------------------------------------------------------------------------------------------
QTEST_MAIN(TestLogFilterProxyModel)

#include "TestLogFilterProxyModel.moc"
