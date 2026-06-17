/**
 * \file  JsonSyntaxHighlighter.cpp
 * \brief Implements syntax highlighting for structured text previews.
 */


#include "JsonSyntaxHighlighter.h"

#include <QColor>
#include <QFont>

//-------------------------------------------------------------------------------------------------
JsonSyntaxHighlighter::JsonSyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent),
    _keyPattern(QStringLiteral(R"("([^"\\]|\\.)*"(?=\s*:))")),
    _stringPattern(QStringLiteral(R"("([^"\\]|\\.)*")")),
    _numberPattern(QStringLiteral(R"(\b-?(0|[1-9]\d*)(\.\d+)?([eE][+-]?\d+)?\b)")),
    _literalPattern(QStringLiteral(R"(\b(true|false|null)\b)")),
    _sqlKeywordPattern(
        QStringLiteral(R"(\b(SELECT|FROM|WHERE|JOIN|LEFT|RIGHT|INNER|OUTER|ON|AND|OR|GROUP|BY|ORDER|HAVING|LIMIT|OFFSET|INSERT|INTO|VALUES|UPDATE|SET|DELETE|WITH|CREATE|ALTER|DROP|TABLE|INDEX|RETURNING|CASE|WHEN|THEN|ELSE|END|AS|DISTINCT|UNION|ALL|NULL|TRUE|FALSE|IS|NOT|IN|LIKE|BETWEEN|EXISTS)\b)"),
        QRegularExpression::CaseInsensitiveOption
    ),
    _sqlStringPattern(QStringLiteral(R"('([^']|'')*')")),
    _sqlLineCommentPattern(QStringLiteral(R"(--[^\n]*)")),
    _xmlCommentPattern(QStringLiteral(R"(<!--.*-->)")),
    _htmlDoctypePattern(QStringLiteral(R"(<!doctype[^>]*>)"), QRegularExpression::CaseInsensitiveOption),
    _xmlTagPattern(QStringLiteral(R"(</?\s*[A-Za-z_][A-Za-z0-9_.:-]*|/?>)")),
    _xmlAttributePattern(QStringLiteral(R"(\b[A-Za-z_][A-Za-z0-9_.:-]*(?=\s*=))")),
    _xmlStringPattern(QStringLiteral(R"("([^"\\]|\\.)*"|'([^'\\]|\\.)*')")),
    _backtraceFramePattern(QStringLiteral(R"(^\s*(#?\d+|\[\d+\]))")),
    _backtraceAddressPattern(QStringLiteral(R"(0x[0-9A-Fa-f]+)")),
    _backtracePathPattern(QStringLiteral(R"((/[^:\s\)]+)+(:\d+)?)")),
    _backtraceFunctionPattern(QStringLiteral(R"([A-Za-z_~][A-Za-z0-9_:~<>]*\s*\([^)]*\))")),
    _punctuationPattern(QStringLiteral(R"([{}\[\],:])"))
{
    _keyFormat.setForeground(QColor(20, 85, 170));
    _keyFormat.setFontWeight(QFont::Bold);

    _stringFormat.setForeground(QColor(20, 130, 70));
    _numberFormat.setForeground(QColor(145, 75, 170));
    _literalFormat.setForeground(QColor(190, 95, 0));
    _literalFormat.setFontWeight(QFont::Bold);
    _keywordFormat.setForeground(QColor(20, 85, 170));
    _keywordFormat.setFontWeight(QFont::Bold);
    _commentFormat.setForeground(QColor(120, 120, 120));
    _commentFormat.setFontItalic(true);
    _tagFormat.setForeground(QColor(170, 65, 120));
    _tagFormat.setFontWeight(QFont::Bold);
    _attributeFormat.setForeground(QColor(20, 85, 170));
    _addressFormat.setForeground(QColor(145, 75, 170));
    _frameFormat.setForeground(QColor(190, 95, 0));
    _frameFormat.setFontWeight(QFont::Bold);
    _pathFormat.setForeground(QColor(100, 100, 100));
    _punctuationFormat.setForeground(QColor(100, 100, 100));
}
//-------------------------------------------------------------------------------------------------
void JsonSyntaxHighlighter::setMode(Mode mode)
{
    if (_mode == mode) {
        return;
    }

    _mode = mode;
    rehighlight();
}
//-------------------------------------------------------------------------------------------------
void JsonSyntaxHighlighter::highlightBlock(const QString &text)
{
    switch (_mode) {
    case Mode::Json:
        highlightJson(text);
        break;

    case Mode::Sql:
        highlightSql(text);
        break;

    case Mode::Xml:
        highlightXml(text);
        break;

    case Mode::Html:
        highlightHtml(text);
        break;

    case Mode::Backtrace:
        highlightBacktrace(text);
        break;

    case Mode::None:
        break;
    }
}
//-------------------------------------------------------------------------------------------------
void JsonSyntaxHighlighter::highlightBacktrace(const QString &text)
{
    const auto applyMatches = [this, &text](const QRegularExpression &pattern, const QTextCharFormat &format) {
        QRegularExpressionMatchIterator matches = pattern.globalMatch(text);

        while (matches.hasNext()) {
            const QRegularExpressionMatch match = matches.next();
            setFormat(static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), format);
        }
    };

    applyMatches(_backtraceFramePattern, _frameFormat);
    applyMatches(_backtraceAddressPattern, _addressFormat);
    applyMatches(_backtracePathPattern, _pathFormat);
    applyMatches(_backtraceFunctionPattern, _keywordFormat);
}
//-------------------------------------------------------------------------------------------------
void JsonSyntaxHighlighter::highlightJson(const QString &text)
{
    const auto applyMatches = [this, &text](const QRegularExpression &pattern, const QTextCharFormat &format) {
        QRegularExpressionMatchIterator matches = pattern.globalMatch(text);

        while (matches.hasNext()) {
            const QRegularExpressionMatch match = matches.next();
            setFormat(static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), format);
        }
    };

    applyMatches(_stringPattern, _stringFormat);
    applyMatches(_numberPattern, _numberFormat);
    applyMatches(_literalPattern, _literalFormat);
    applyMatches(_punctuationPattern, _punctuationFormat);
    applyMatches(_keyPattern, _keyFormat);
}
//-------------------------------------------------------------------------------------------------
void JsonSyntaxHighlighter::highlightSql(const QString &text)
{
    const auto applyMatches = [this, &text](const QRegularExpression &pattern, const QTextCharFormat &format) {
        QRegularExpressionMatchIterator matches = pattern.globalMatch(text);

        while (matches.hasNext()) {
            const QRegularExpressionMatch match = matches.next();
            setFormat(static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), format);
        }
    };

    applyMatches(_sqlKeywordPattern, _keywordFormat);
    applyMatches(_numberPattern, _numberFormat);
    applyMatches(_sqlStringPattern, _stringFormat);
    applyMatches(_sqlLineCommentPattern, _commentFormat);
}
//-------------------------------------------------------------------------------------------------
void JsonSyntaxHighlighter::highlightXml(const QString &text)
{
    const auto applyMatches = [this, &text](const QRegularExpression &pattern, const QTextCharFormat &format) {
        QRegularExpressionMatchIterator matches = pattern.globalMatch(text);

        while (matches.hasNext()) {
            const QRegularExpressionMatch match = matches.next();
            setFormat(static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), format);
        }
    };

    applyMatches(_xmlTagPattern, _tagFormat);
    applyMatches(_xmlAttributePattern, _attributeFormat);
    applyMatches(_xmlStringPattern, _stringFormat);
    applyMatches(_xmlCommentPattern, _commentFormat);
}
//-------------------------------------------------------------------------------------------------
void JsonSyntaxHighlighter::highlightHtml(const QString &text)
{
    highlightXml(text);

    QRegularExpressionMatchIterator matches = _htmlDoctypePattern.globalMatch(text);

    while (matches.hasNext()) {
        const QRegularExpressionMatch match = matches.next();
        setFormat(static_cast<int>(match.capturedStart()), static_cast<int>(match.capturedLength()), _tagFormat);
    }
}
//-------------------------------------------------------------------------------------------------
