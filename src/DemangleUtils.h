/**
 * \file  DemangleUtils.h
 * \brief Declares helpers for demangling C++ symbols in backtraces.
 */


#pragma once

#include <QString>

//-------------------------------------------------------------------------------------------------
QString demangleSymbols(const QString &text);
//-------------------------------------------------------------------------------------------------
