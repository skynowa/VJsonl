/**
 * \file  CodeFormatter.h
 * \brief Declares helpers for formatting structured text fragments shown in previews.
 */


#pragma once

#include <QString>
//-------------------------------------------------------------------------------------------------
namespace CodeFormatter
{

// Format detection
bool    looksLikeJson(const QString &text);
bool    looksLikeSql(const QString &text);
bool    looksLikeXml(const QString &text);

// Structured fragment formatting
QString formatFragments(QString text, bool *changed);

}
//-------------------------------------------------------------------------------------------------
