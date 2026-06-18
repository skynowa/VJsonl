/**
 * \file  Utils/TextSearch.h
 * \brief Declares helpers for highlighting text search matches.
 */


#pragma once

#include <QString>

class QTextEdit;

//-------------------------------------------------------------------------------------------------
namespace text_search_utils
{

int highlightAll(QTextEdit *view, const QString &text);

}
//-------------------------------------------------------------------------------------------------
