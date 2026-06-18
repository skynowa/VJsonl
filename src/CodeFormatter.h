/**
 * \file  CodeFormatter.h
 * \brief Declares helpers for formatting structured text fragments shown in previews.
 */


#pragma once

#include <QString>

//-------------------------------------------------------------------------------------------------
namespace CodeFormatter
{

bool    looksLikeJson(const QString &text);
bool    looksLikeSql(const QString &text);
bool    looksLikeXml(const QString &text);
QString formatFragments(QString text, bool *changed);

}
//-------------------------------------------------------------------------------------------------
