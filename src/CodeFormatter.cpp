/**
 * \file  CodeFormatter.cpp
 * \brief Implements lightweight formatting for JSON, XML, SQL, and embedded fragments.
 */


#include "CodeFormatter.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QRegularExpression>
#include <QVector>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <utility>

namespace
{
QString normalizedSql(QString text)
{
    text.replace(QRegularExpression(QStringLiteral("\\s+")), QStringLiteral(" "));
    return text.trimmed();
}

bool isSqlText(const QString &text)
{
    const QString sql = normalizedSql(text);

    if (sql.isEmpty()) {
        return false;
    }

    static const QStringList prefixes {
        QStringLiteral("SELECT "),
        QStringLiteral("UPDATE "),
        QStringLiteral("INSERT "),
        QStringLiteral("DELETE "),
        QStringLiteral("WITH "),
        QStringLiteral("CREATE "),
        QStringLiteral("ALTER "),
        QStringLiteral("DROP ")
    };

    for (const QString &prefix : prefixes) {
        if (sql.startsWith(prefix, Qt::CaseInsensitive)) {
            return true;
        }
    }

    return false;
}

bool isJsonText(const QString &text)
{
    QJsonParseError error {};
    QJsonDocument::fromJson(text.trimmed().toUtf8(), &error);
    return error.error == QJsonParseError::NoError;
}

bool isXmlText(const QString &text)
{
    const QString xml = text.trimmed();

    if (!xml.startsWith(QStringLiteral("<"))) {
        return false;
    }

    QXmlStreamReader reader(xml);

    while (!reader.atEnd()) {
        reader.readNext();
    }

    return !reader.hasError();
}

QString formatSql(QString text)
{
    QString sql = normalizedSql(std::move(text));

    if (sql.isEmpty()) {
        return {};
    }

    const QStringList breakBefore {
        QStringLiteral("SELECT"),
        QStringLiteral("FROM"),
        QStringLiteral("LEFT JOIN"),
        QStringLiteral("RIGHT JOIN"),
        QStringLiteral("INNER JOIN"),
        QStringLiteral("OUTER JOIN"),
        QStringLiteral("JOIN"),
        QStringLiteral("SET"),
        QStringLiteral("VALUES"),
        QStringLiteral("WHERE"),
        QStringLiteral("GROUP BY"),
        QStringLiteral("ORDER BY"),
        QStringLiteral("HAVING"),
        QStringLiteral("LIMIT"),
        QStringLiteral("OFFSET"),
        QStringLiteral("RETURNING")
    };

    for (const QString &keyword : breakBefore) {
        const QString pattern = QStringLiteral("\\s+%1\\b").arg(
            keyword.toLower().replace(QStringLiteral(" "), QStringLiteral("\\s+"))
        );
        sql.replace(
            QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption),
            QStringLiteral("\n") + keyword
        );
    }

    sql.replace(QRegularExpression(QStringLiteral("\\s+AND\\s+"), QRegularExpression::CaseInsensitiveOption), QStringLiteral("\n    AND "));
    sql.replace(QRegularExpression(QStringLiteral("\\s+OR\\s+"), QRegularExpression::CaseInsensitiveOption), QStringLiteral("\n    OR "));
    sql.replace(QRegularExpression(QStringLiteral(",\\s*")), QStringLiteral(",\n    "));

    return sql.trimmed();
}

QString formatJson(const QString &text)
{
    QJsonParseError error {};
    const QJsonDocument doc = QJsonDocument::fromJson(text.trimmed().toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        return text;
    }

    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

QString formatXml(const QString &text)
{
    QXmlStreamReader reader(text.trimmed());
    QString formatted;
    QXmlStreamWriter writer(&formatted);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(4);

    while (!reader.atEnd()) {
        reader.readNext();

        if (!reader.hasError()) {
            writer.writeCurrentToken(reader);
        }
    }

    return reader.hasError() ? text : formatted.trimmed();
}

int findJsonEnd(const QString &text, int start)
{
    const QChar open = text.at(start);
    const QChar close = open == QLatin1Char('{') ? QLatin1Char('}') : QLatin1Char(']');
    QVector<QChar> stack;
    bool inString = false;
    bool escaped = false;

    for (int i = start; i < text.size(); ++i) {
        const QChar ch = text.at(i);

        if (inString) {
            if (escaped) {
                escaped = false;
            } else if (ch == QLatin1Char('\\')) {
                escaped = true;
            } else if (ch == QLatin1Char('"')) {
                inString = false;
            }

            continue;
        }

        if (ch == QLatin1Char('"')) {
            inString = true;
        } else if (ch == QLatin1Char('{') || ch == QLatin1Char('[')) {
            stack.push_back(ch);
        } else if (ch == QLatin1Char('}') || ch == QLatin1Char(']')) {
            if (stack.isEmpty()) {
                return -1;
            }

            const QChar expected = stack.back() == QLatin1Char('{') ? QLatin1Char('}') : QLatin1Char(']');

            if (ch != expected) {
                return -1;
            }

            stack.pop_back();

            if (stack.isEmpty() && ch == close) {
                return i + 1;
            }
        }
    }

    return -1;
}

QString formatJsonFragments(QString text, bool *changed)
{
    for (int start = 0; start < text.size(); ++start) {
        const QChar ch = text.at(start);

        if (ch != QLatin1Char('{') && ch != QLatin1Char('[')) {
            continue;
        }

        const int end = findJsonEnd(text, start);

        if (end <= start) {
            continue;
        }

        const QString fragment = text.mid(start, end - start);

        if (!isJsonText(fragment)) {
            continue;
        }

        const QString formatted = formatJson(fragment);
        text.replace(start, end - start, formatted);
        start += formatted.size() - 1;
        *changed = true;
    }

    return text;
}

QString formatXmlFragments(QString text, bool *changed)
{
    int searchFrom = 0;

    while (searchFrom < text.size()) {
        const int start = text.indexOf(QLatin1Char('<'), searchFrom);

        if (start < 0 || start + 1 >= text.size() || !text.at(start + 1).isLetter()) {
            searchFrom = start < 0 ? text.size() : start + 1;
            continue;
        }

        bool formattedFragment = false;

        for (int end = text.indexOf(QLatin1Char('>'), start + 1); end >= 0; end = text.indexOf(QLatin1Char('>'), end + 1)) {
            const QString fragment = text.mid(start, end - start + 1);

            if (!isXmlText(fragment)) {
                continue;
            }

            const QString formatted = formatXml(fragment);
            text.replace(start, end - start + 1, formatted);
            searchFrom = start + formatted.size();
            formattedFragment = true;
            *changed = true;
            break;
        }

        if (!formattedFragment) {
            searchFrom = start + 1;
        }
    }

    return text;
}

int findSqlStart(const QString &text)
{
    static const QRegularExpression sqlStart(
        QStringLiteral("\\b(SELECT|UPDATE|INSERT|DELETE|WITH|CREATE|ALTER|DROP)\\b"),
        QRegularExpression::CaseInsensitiveOption
    );

    const QRegularExpressionMatch match = sqlStart.match(text);
    return match.hasMatch() ? match.capturedStart() : -1;
}

QString formatSqlFragment(const QString &text, bool *changed)
{
    const int start = findSqlStart(text);

    if (start < 0) {
        return text;
    }

    const int semicolon = text.indexOf(QLatin1Char(';'), start);
    const int end = semicolon >= 0 ? semicolon + 1 : text.size();
    QString formatted = text;
    formatted.replace(start, end - start, formatSql(text.mid(start, end - start)));
    *changed = true;
    return formatted;
}
}

namespace CodeFormatter
{
QString formatFragments(QString text, bool *changed)
{
    *changed = false;

    if (isSqlText(text)) {
        *changed = true;
        return formatSql(text);
    }

    if (isJsonText(text)) {
        *changed = true;
        return formatJson(text);
    }

    if (isXmlText(text)) {
        *changed = true;
        return formatXml(text);
    }

    text = formatJsonFragments(std::move(text), changed);
    text = formatXmlFragments(std::move(text), changed);
    text = formatSqlFragment(text, changed);
    return text;
}
}
