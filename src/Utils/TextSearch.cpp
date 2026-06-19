/**
 * \file  Utils/TextSearch.cpp
 * \brief Implements helpers for highlighting text search matches.
 */


#include "Utils/TextSearch.h"

#include <QPalette>
#include <QTextDocument>
#include <QTextEdit>

//-------------------------------------------------------------------------------------------------
namespace text_search_utils
{
int
highlightAll(
    QTextEdit     *view,
    const QString &text
)
{
    if (view == nullptr) {
        return 0;
    }

    QList<QTextEdit::ExtraSelection> selections;

    if (!text.isEmpty()) {
        QTextCursor cursor(view->document());

        while (!(cursor = view->document()->find(text, cursor)).isNull()) {
            QTextEdit::ExtraSelection selection;
            selection.cursor = cursor;
            selection.format.setBackground(view->palette().color(QPalette::Highlight));
            selection.format.setForeground(view->palette().color(QPalette::HighlightedText));
            selections.push_back(selection);
        }
    }

    view->setExtraSelections(selections);

    if (!selections.isEmpty()) {
        view->setTextCursor(selections.constFirst().cursor);
        view->ensureCursorVisible();
    }

    return static_cast<int>(selections.size());
}
}
//-------------------------------------------------------------------------------------------------
