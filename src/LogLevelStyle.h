/**
 * \file  LogLevelStyle.h
 * \brief Declares visual helpers for rendering log levels.
 */


#pragma once

#include <QIcon>
#include <QString>
//-------------------------------------------------------------------------------------------------
namespace LogLevelStyle
{

// Level icon rendering
QIcon   iconForLevel(const QString &level);
QString iconHtmlForLevel(const QString &level);

// Status-bar counter rendering
QString levelCounterHtml(const QString &level, int count);

}
//-------------------------------------------------------------------------------------------------
