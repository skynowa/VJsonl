/**
 * \file  JsonlRecord.h
 * \brief Defines the in-memory representation of one loaded JSONL/log line.
 */


#pragma once

#include "Utils/Json.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
//-------------------------------------------------------------------------------------------------
struct JsonlRecord final
{
    int           lineNo {};
    QString       raw;
    QJsonDocument doc;
    QDateTime     timestamp;
    bool          valid {};
    QString       error;

    QString value(const QString &key) const
    {
        if (!valid || !doc.isObject()) {
            return {};
        }

        return json_utils::jsonValueToString(doc.object().value(key));
    }
};
//-------------------------------------------------------------------------------------------------
