/**
 * \file  JsonSyntaxHighlighter.h
 * \brief Declares syntax highlighting for structured text previews.
 */


#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
//-------------------------------------------------------------------------------------------------
class JsonSyntaxHighlighter final :
    public QSyntaxHighlighter
{
public:
    // Supported highlighter modes
    enum class Mode
    {
        None,
        Json,
        Sql,
        Xml,
        Html,
        Backtrace
    };

    explicit JsonSyntaxHighlighter(QTextDocument *parent = nullptr);

    // Highlighter mode
    void setMode(Mode mode);

protected:
    // QSyntaxHighlighter interface
    void highlightBlock(const QString &text) override;

private:
    // Mode-specific highlighters
    void highlightJson(const QString &text);
    void highlightSql(const QString &text);
    void highlightXml(const QString &text);
    void highlightHtml(const QString &text);
    void highlightBacktrace(const QString &text);

private:
    // Current mode and text formats
    Mode            _mode { Mode::None };
    QTextCharFormat _keyFormat;
    QTextCharFormat _stringFormat;
    QTextCharFormat _numberFormat;
    QTextCharFormat _literalFormat;
    QTextCharFormat _keywordFormat;
    QTextCharFormat _commentFormat;
    QTextCharFormat _tagFormat;
    QTextCharFormat _attributeFormat;
    QTextCharFormat _addressFormat;
    QTextCharFormat _frameFormat;
    QTextCharFormat _pathFormat;
    QTextCharFormat _punctuationFormat;

    // Token patterns
    QRegularExpression _keyPattern;
    QRegularExpression _stringPattern;
    QRegularExpression _numberPattern;
    QRegularExpression _literalPattern;
    QRegularExpression _sqlKeywordPattern;
    QRegularExpression _sqlStringPattern;
    QRegularExpression _sqlLineCommentPattern;
    QRegularExpression _xmlCommentPattern;
    QRegularExpression _htmlDoctypePattern;
    QRegularExpression _xmlTagPattern;
    QRegularExpression _xmlAttributePattern;
    QRegularExpression _xmlStringPattern;
    QRegularExpression _backtraceFramePattern;
    QRegularExpression _backtraceAddressPattern;
    QRegularExpression _backtracePathPattern;
    QRegularExpression _backtraceFunctionPattern;
    QRegularExpression _punctuationPattern;
};
//-------------------------------------------------------------------------------------------------
