/**
 * \file  Utils/Demangle.h
 * \brief Declares helpers for demangling C++ symbols in backtraces.
 */


#pragma once

#include <QString>

//-------------------------------------------------------------------------------------------------
namespace demangle_utils
{

QString demangleSymbols(const QString &text);

}
//-------------------------------------------------------------------------------------------------
