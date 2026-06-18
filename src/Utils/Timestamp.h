/**
 * \file  Utils/Timestamp.h
 * \brief Declares timestamp parsing and formatting helpers.
 */


#pragma once

#include <QDateTime>
#include <QString>

//-------------------------------------------------------------------------------------------------
namespace datetime_utils
{

QDateTime parseTimestamp(const QString &text);
QString   formatTimestamp(const QString &text);

}
//-------------------------------------------------------------------------------------------------
