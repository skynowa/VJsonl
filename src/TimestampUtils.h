/**
 * \file  TimestampUtils.h
 * \brief Declares timestamp parsing and formatting helpers.
 */


#pragma once

#include <QDateTime>
#include <QString>

//-------------------------------------------------------------------------------------------------
namespace TimestampUtils
{
QDateTime parseTimestamp(const QString &text);
QString formatTimestamp(const QString &text);
}
//-------------------------------------------------------------------------------------------------
