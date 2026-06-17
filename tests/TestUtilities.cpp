/**
 * \file  TestUtilities.cpp
 * \brief Tests standalone utility helpers.
 */


#include "CodeFormatter.h"
#include "Utils/Demangle.h"
#include "Utils/File.h"
#include "Utils/Html.h"
#include "Utils/Json.h"
#include "Utils/Timestamp.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QTest>

//-------------------------------------------------------------------------------------------------
class TestUtilities final : public QObject
{
    Q_OBJECT

private slots:
    void jsonValueToStringConvertsSupportedTypes()
    {
        QCOMPARE(json_utils::jsonValueToString(QJsonValue(QStringLiteral("text"))), QStringLiteral("text"));
        QCOMPARE(json_utils::jsonValueToString(QJsonValue(true)), QStringLiteral("true"));
        QCOMPARE(json_utils::jsonValueToString(QJsonValue(42)), QStringLiteral("42"));
        QCOMPARE(json_utils::jsonValueToString(QJsonArray {1, 2}), QStringLiteral("[1,2]"));
        QCOMPARE(json_utils::jsonValueToString(QJsonObject {{QStringLiteral("a"), 1}}), QStringLiteral("{\"a\":1}"));
    }

    void timestampParsesAndFormats()
    {
        const QDateTime timestamp = datetime_utils::parseTimestamp(QStringLiteral("2026-06-15T08:43:08.735921-0500)"));

        QVERIFY(timestamp.isValid());
        QCOMPARE(timestamp.offsetFromUtc(), -5 * 60 * 60);
        QCOMPARE(datetime_utils::formatTimestamp(QStringLiteral("2026-06-15T08:43:08.735921-0500)")), QStringLiteral("2026-jun-15 08:43:08.735"));
        QCOMPARE(datetime_utils::formatTimestamp(QStringLiteral("not a timestamp")), QStringLiteral("not a timestamp"));
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
        QVERIFY(html_utils::looksLikeHtml(QStringLiteral("<div>hello</div>")));
        QVERIFY(!html_utils::looksLikeHtml(QStringLiteral("plain text")));
        QCOMPARE(file_utils::humanFileSize(999), QStringLiteral("999 B"));
        QCOMPARE(file_utils::humanFileSize(1500), QStringLiteral("1.5 KB"));
    }

    void demanglesKnownCppSymbol()
    {
        const QString demangled = demangle_utils::demangleSymbols(QStringLiteral("_Z3fooi"));

        QVERIFY(demangled.contains(QStringLiteral("foo")));
        QVERIFY(demangled.contains(QStringLiteral("int")));
    }
};

//-------------------------------------------------------------------------------------------------
QTEST_MAIN(TestUtilities)

#include "TestUtilities.moc"
