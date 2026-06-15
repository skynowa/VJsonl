/**
 * \file  JsonUtils.h
 * \brief Declares helpers for converting JSON values to display strings.
 */


#pragma once

#include <QJsonValue>
#include <QString>

QString jsonValueToString(const QJsonValue &value);
