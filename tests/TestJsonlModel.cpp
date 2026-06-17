/**
 * \file  TestJsonlModel.cpp
 * \brief Tests JsonlModel loading and display roles.
 */


#include "JsonlModel.h"

#include <QBrush>
#include <QDir>
#include <QTemporaryFile>
#include <QTest>

namespace
{
//-------------------------------------------------------------------------------------------------
int columnByName(const JsonlModel &model, const QString &name)
{
    for (int column = 0; column < model.columnCount(); ++column) {
        if (model.headerData(column, Qt::Horizontal).toString() == name) {
            return column;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------
bool writeJsonlFile(QTemporaryFile *file)
{
    if (!file->open()) {
        return false;
    }

    file->write(R"({"project":"suppliers","app":"sync","proc_name":"worker","module":"loader","log_name":"access","ts":"2026-06-15T08:43:08.735921-0500","level":"Info","msg":"Loaded hotel","query":"select * from hotel","mem_usage_kb":"174080"})");
    file->write("\n");
    file->write("\n");
    file->write("{broken json\n");
    file->close();
    return true;
}
}

//-------------------------------------------------------------------------------------------------
class TestJsonlModel final : public QObject
{
    Q_OBJECT

private slots:
    void loadFileParsesRowsAndMetadata()
    {
        JsonlModel model;
        QString error;
        QTemporaryFile file(QDir::temp().filePath(QStringLiteral("vjson-model-test-XXXXXX.jsonl")));

        QVERIFY(writeJsonlFile(&file));
        QVERIFY(model.loadFile(file.fileName(), &error));
        QCOMPARE(error, QString());
        QCOMPARE(model.rowCount(), 3);
        QCOMPARE(model.invalidRowsCount(), 2);
        QCOMPARE(model.levelCounts().value(QStringLiteral("info")), 1);
        QVERIFY(model.recordAt(0) != nullptr);
        QVERIFY(model.recordAt(-1) == nullptr);
    }

    void dataFormatsSpecialColumns()
    {
        JsonlModel model;
        QTemporaryFile file(QDir::temp().filePath(QStringLiteral("vjson-model-test-XXXXXX.jsonl")));

        QVERIFY(writeJsonlFile(&file));
        QVERIFY(model.loadFile(file.fileName()));

        const int tsColumn = columnByName(model, QStringLiteral("ts"));
        const int memColumn = columnByName(model, QStringLiteral("mem_usage_kb"));
        const int validColumn = columnByName(model, QStringLiteral("valid"));

        QVERIFY(tsColumn >= 0);
        QVERIFY(memColumn >= 0);
        QVERIFY(validColumn >= 0);
        QCOMPARE(model.data(model.index(0, tsColumn)).toString(), QStringLiteral("2026-jun-15 08:43:08.735"));
        QCOMPARE(model.data(model.index(0, memColumn)).toString(), QStringLiteral("178.3 MB"));
        QCOMPARE(model.data(model.index(1, validColumn)).toString(), QString());
        QCOMPARE(model.data(model.index(1, validColumn), Qt::ToolTipRole).toString(), QStringLiteral("no: empty line"));
        QVERIFY(model.data(model.index(1, validColumn), Qt::DecorationRole).isValid());
    }
};

//-------------------------------------------------------------------------------------------------
QTEST_MAIN(TestJsonlModel)

#include "TestJsonlModel.moc"
