/**
 * \file  CodeFormatter.h
 * \brief Declares helpers for formatting structured text fragments shown in previews.
 */


#pragma once

#include <QString>

//-------------------------------------------------------------------------------------------------
namespace CodeFormatter
{
QString formatFragments(QString text, bool *changed);
}
//-------------------------------------------------------------------------------------------------
