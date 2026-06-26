/**
 * \file  TestCsv.cpp
 * \brief Tests CSV parsing, JSONL conversion, and model loading.
 */


#include "ConverterCsvToJsonl.h"
#include "CsvReader.h"
#include "JsonlModel.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QTemporaryFile>
#include <QTest>

namespace
{
//-------------------------------------------------------------------------------------------------
int
columnByName(
    const JsonlModel &model,
    const QString    &name
)
{
    for (int column = 0; column < model.columnCount(); ++column) {
        if (model.headerData(column, Qt::Horizontal, Qt::DisplayRole).toString() == name) {
            return column;
        }
    }

    return -1;
}
}

//-------------------------------------------------------------------------------------------------
class TestCsv final : public QObject
{
    Q_OBJECT

private slots:
    void
    readsQuotedCsvFile()
    {
        QTemporaryFile file;
        QVERIFY(file.open());
        file.write(
            "project,msg,note,empty\r\n"
            "suppliers,\"hello, world\",\"line 1\r\nline 2\",\r\n"
            "booked,\"quote \"\"ok\"\"\",short\r\n"
        );
        file.close();

        CsvData csvData;
        QString error;
        QVERIFY2(CsvReader::readFile(file.fileName(), &csvData, &error), qPrintable(error));
        QCOMPARE(csvData.headers, QStringList({"project", "msg", "note", "empty"}));
        QCOMPARE(csvData.rows.size(), 2);
        QCOMPARE(csvData.rows.at(0), QStringList({"suppliers", "hello, world", "line 1\nline 2", ""}));
        QCOMPARE(csvData.rows.at(1), QStringList({"booked", "quote \"ok\"", "short", ""}));
    }

    void
    rejectsMalformedCsv()
    {
        CsvData csvData;
        QString error;

        QVERIFY(!CsvReader::parse(QByteArray("a,b\n\"unterminated,b\n"), &csvData, &error));
        QVERIFY(error.contains(QStringLiteral("Unterminated")));

        QVERIFY(!CsvReader::parse(QByteArray("a,a\n1,2\n"), &csvData, &error));
        QVERIFY(error.contains(QStringLiteral("duplicate")));

        QVERIFY(!CsvReader::parse(QByteArray("a,b\n1,2,3\n"), &csvData, &error));
        QVERIFY(error.contains(QStringLiteral("3 fields")));
    }

    void
    convertsCsvToJsonlAndLoadsModel()
    {
        CsvData csvData;
        QString error;
        QVERIFY(CsvReader::parse(
            QByteArray("project,level,msg\nsuppliers,Info,Loaded hotel\nbooked,Error,Failed\n"),
            &csvData,
            &error
        ));

        QByteArray jsonlData;
        QVERIFY2(ConverterCsvToJsonl::convert(csvData, &jsonlData, &error), qPrintable(error));
        const QList<QByteArray> jsonLines = jsonlData.trimmed().split('\n');
        QCOMPARE(jsonLines.size(), 2);
        QJsonParseError parseError {};
        const QJsonDocument firstLine = QJsonDocument::fromJson(jsonLines.at(0), &parseError);
        QCOMPARE(parseError.error, QJsonParseError::NoError);
        QVERIFY(firstLine.isObject());
        QCOMPARE(
            firstLine.object().value(QStringLiteral("msg")).toString(),
            QStringLiteral("Loaded hotel")
        );

        JsonlModel model;
        QVERIFY2(model.loadJsonlData(jsonlData, QStringLiteral("source.csv"), &error), qPrintable(error));
        QCOMPARE(model.fileName(), QStringLiteral("source.csv"));
        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.levelCounts().value(QStringLiteral("info")), 1);
        QCOMPARE(model.levelCounts().value(QStringLiteral("error")), 1);

        const int projectColumn = columnByName(model, QStringLiteral("project"));
        const int msgColumn = columnByName(model, QStringLiteral("msg"));
        QVERIFY(projectColumn >= 0);
        QVERIFY(msgColumn >= 0);
        QCOMPARE(model.data(model.index(0, projectColumn)).toString(), QStringLiteral("suppliers"));
        QCOMPARE(model.data(model.index(1, msgColumn)).toString(), QStringLiteral("Failed"));
    }
};

//-------------------------------------------------------------------------------------------------
QTEST_MAIN(TestCsv)

#include "TestCsv.moc"
