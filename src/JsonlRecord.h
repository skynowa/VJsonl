/**
 * \file  JsonlRecord.h
 * \brief Defines the in-memory representation of one loaded JSONL/log line.
 */


#pragma once

#include "JsonUtils.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

//-------------------------------------------------------------------------------------------------
struct JsonlRecord final
{
    int           lineNo {};
    QString       raw;
    QJsonDocument doc;
    bool          valid {};
    QString       error;

    QString value(const QString &key) const
    {
        if (!valid || !doc.isObject()) {
            return {};
        }

        return jsonValueToString(doc.object().value(key));
    }
};
//-------------------------------------------------------------------------------------------------
