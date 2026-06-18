/**
 * \file  Utils/Json.h
 * \brief Declares helpers for converting JSON values to display strings.
 */


#pragma once

#include <QJsonValue>
#include <QString>

//-------------------------------------------------------------------------------------------------
namespace json_utils
{

QString jsonValueToString(const QJsonValue &value);

}
//-------------------------------------------------------------------------------------------------
