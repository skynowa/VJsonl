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
bool
writeJsonlFile(
    QTemporaryFile *file
)
{
    if (!file->open()) {
        return false;
    }

    file->write(R"({"project":"suppliers","app":"sync","proc_name":"worker","module":"loader","log_name":"access","ts":"2026-06-15T08:00:00.000-0500","level":"info","msg":"Loaded hotel","query":"select * from hotel","request":"GET /hotels","page":"hotel list","descr":"hotel import"})");
    file->write("\n");
    file->write(R"({"project":"booked","app":"cms","proc_name":"api","module":"rooms","log_name":"audit","ts":"2026-06-15T09:30:00.000-0500","level":"error","msg":"Room failed","query":"update rooms set name = 'x'","request":"POST /rooms","page":"room editor","descr":"room update"})");
    file->write("\n");
    file->write(R"({"project":"suppliers","app":"sync","proc_name":"worker","module":"loader","log_name":"access","ts":"2026-06-15T11:00:00.000-0500","level":"debug","msg":"Cache warmed","query":"delete from cache","request":{"method":"DELETE","path":"/cache"},"page":42,"descr":"cache maintenance"})");
    file->write("\n");
    file->close();
    return true;
}

//-------------------------------------------------------------------------------------------------
int
columnByName(
    const QAbstractItemModel &model,
    const QString            &name
)
{
    for (int column = 0; column < model.columnCount(); ++column) {
        if (model.headerData(column, Qt::Horizontal).toString() == name) {
            return column;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------
QString
valueAt(
    const QAbstractItemModel &model,
    int                      row,
    const QString            &columnName
)
{
    const int column = columnByName(model, columnName);

    if (column < 0) {
        QTest::qFail(qPrintable(QStringLiteral("Missing column '%1'").arg(columnName)), __FILE__, __LINE__);
        return {};
    }

    return model.data(model.index(row, column)).toString();
}
}

//-------------------------------------------------------------------------------------------------
class TestLogFilterProxyModel final : public QObject
{
    Q_OBJECT

private slots:
    void
    filtersByExactColumnsAndClears()
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

        proxy.setDescrFilter(QStringLiteral("ROOM UPDATE"));
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(valueAt(proxy, 0, QStringLiteral("project")), QStringLiteral("booked"));

        proxy.setDescrFilter(QString());
        QCOMPARE(proxy.rowCount(), 3);
    }

    void
    filtersTextColumnsByContains()
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

        proxy.setRequestTextFilter(QStringLiteral("post /ROOMS"));
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(valueAt(proxy, 0, QStringLiteral("project")), QStringLiteral("booked"));

        proxy.setRequestTextFilter(QStringLiteral("DELETE"));
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(valueAt(proxy, 0, QStringLiteral("msg")), QStringLiteral("Cache warmed"));

        proxy.setRequestTextFilter(QString());
        QCOMPARE(proxy.rowCount(), 3);

        proxy.setPageTextFilter(QStringLiteral("ROOM EDIT"));
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(valueAt(proxy, 0, QStringLiteral("project")), QStringLiteral("booked"));

        proxy.setPageTextFilter(QStringLiteral("42"));
        QCOMPARE(proxy.rowCount(), 1);
        QCOMPARE(valueAt(proxy, 0, QStringLiteral("msg")), QStringLiteral("Cache warmed"));

        proxy.setPageTextFilter(QString());
        QCOMPARE(proxy.rowCount(), 3);
    }

    void
    filtersByTimestampRange()
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

    void
    sortsTextColumnsCaseInsensitively()
    {
        JsonlModel model;
        QString error;
        const QByteArray data(
            "{\"msg\":\"beta\"}\n"
            "{\"msg\":\"Alpha\"}\n"
            "{\"msg\":\"gamma\"}\n"
        );

        QVERIFY2(model.loadJsonlData(data, QStringLiteral("sort.jsonl"), &error), qPrintable(error));

        LogFilterProxyModel proxy;
        proxy.setSourceModel(&model);
        const int msgColumn = columnByName(proxy, QStringLiteral("msg"));
        QVERIFY(msgColumn >= 0);

        proxy.sort(msgColumn, Qt::AscendingOrder);
        QCOMPARE(valueAt(proxy, 0, QStringLiteral("msg")), QStringLiteral("Alpha"));
        QCOMPARE(valueAt(proxy, 1, QStringLiteral("msg")), QStringLiteral("beta"));
        QCOMPARE(valueAt(proxy, 2, QStringLiteral("msg")), QStringLiteral("gamma"));
    }
};

//-------------------------------------------------------------------------------------------------
QTEST_MAIN(TestLogFilterProxyModel)

#include "TestLogFilterProxyModel.moc"
