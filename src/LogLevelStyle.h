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
QIcon iconForLevel(const QString &level);
}
//-------------------------------------------------------------------------------------------------
