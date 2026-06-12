#include "HtmlUtils.h"

#include <QRegularExpression>

namespace HtmlUtils
{
bool looksLikeHtml(const QString &text)
{
    const QString trimmed = text.trimmed();

    if (trimmed.isEmpty()) {
        return false;
    }

    if (
        trimmed.contains(QStringLiteral("<!doctype html"), Qt::CaseInsensitive)
        || trimmed.contains(QStringLiteral("<html"), Qt::CaseInsensitive)
        || trimmed.contains(QStringLiteral("<body"), Qt::CaseInsensitive)
    ) {
        return true;
    }

    static const QRegularExpression commonHtmlTag(
        QStringLiteral("<\\s*(div|span|p|a|img|table|tr|td|th|ul|ol|li|script|style|head|meta|title|section|article|main|header|footer|form|input|button)\\b"),
        QRegularExpression::CaseInsensitiveOption
    );

    return commonHtmlTag.match(trimmed).hasMatch();
}
}
