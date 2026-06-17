/**
 * \file  TestUtils.cpp
 * \brief Tests standalone utility helpers.
 */


#include "CodeFormatter.h"
#include "DemangleUtils.h"
#include "FileUtils.h"
#include "HtmlUtils.h"
#include "JsonUtils.h"
#include "TimestampUtils.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QTest>

//-------------------------------------------------------------------------------------------------
class TestUtils final : public QObject
{
    Q_OBJECT

private slots:
    void jsonValueToStringConvertsSupportedTypes()
    {
        QCOMPARE(jsonValueToString(QJsonValue(QStringLiteral("text"))), QStringLiteral("text"));
        QCOMPARE(jsonValueToString(QJsonValue(true)), QStringLiteral("true"));
        QCOMPARE(jsonValueToString(QJsonValue(42)), QStringLiteral("42"));
        QCOMPARE(jsonValueToString(QJsonArray {1, 2}), QStringLiteral("[1,2]"));
        QCOMPARE(jsonValueToString(QJsonObject {{QStringLiteral("a"), 1}}), QStringLiteral("{\"a\":1}"));
    }

    void timestampUtilsParsesAndFormats()
    {
        const QDateTime timestamp = TimestampUtils::parseTimestamp(QStringLiteral("2026-06-15T08:43:08.735921-0500)"));

        QVERIFY(timestamp.isValid());
        QCOMPARE(timestamp.offsetFromUtc(), -5 * 60 * 60);
        QCOMPARE(TimestampUtils::formatTimestamp(QStringLiteral("2026-06-15T08:43:08.735921-0500)")), QStringLiteral("2026-jun-15 08:43:08.735"));
        QCOMPARE(TimestampUtils::formatTimestamp(QStringLiteral("not a timestamp")), QStringLiteral("not a timestamp"));
    }

    void codeFormatterDetectsAndFormatsFragments()
    {
        bool changed = false;

        QVERIFY(CodeFormatter::looksLikeJson(QStringLiteral("{\"a\":1}")));
        QVERIFY(CodeFormatter::looksLikeSql(QStringLiteral("select * from hotels")));
        QVERIFY(CodeFormatter::looksLikeXml(QStringLiteral("<root><a/></root>")));

        const QString formatted = CodeFormatter::formatFragments(QStringLiteral("prefix {\"a\":1,\"b\":2} suffix"), &changed);
        QVERIFY(changed);
        QVERIFY(formatted.contains(QStringLiteral("\n")));
        QVERIFY(formatted.contains(QStringLiteral("\"a\"")));
    }

    void htmlAndFileHelpers()
    {
        QVERIFY(HtmlUtils::looksLikeHtml(QStringLiteral("<div>hello</div>")));
        QVERIFY(!HtmlUtils::looksLikeHtml(QStringLiteral("plain text")));
        QCOMPARE(humanFileSize(999), QStringLiteral("999 B"));
        QCOMPARE(humanFileSize(1500), QStringLiteral("1.5 KB"));
    }

    void demanglesKnownCppSymbol()
    {
        const QString demangled = demangleSymbols(QStringLiteral("_Z3fooi"));

        QVERIFY(demangled.contains(QStringLiteral("foo")));
        QVERIFY(demangled.contains(QStringLiteral("int")));
    }
};

//-------------------------------------------------------------------------------------------------
QTEST_MAIN(TestUtils)

#include "TestUtils.moc"
