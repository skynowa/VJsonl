#include "JsonUtils.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

QString jsonValueToString(const QJsonValue &value)
{
    if (value.isUndefined() || value.isNull()) {
        return {};
    }

    if (value.isString()) {
        return value.toString();
    }

    if (value.isBool()) {
        return value.toBool() ? QStringLiteral("true") : QStringLiteral("false");
    }

    if (value.isDouble()) {
        return QString::number(value.toDouble(), 'g', 15);
    }

    if (value.isObject()) {
        return QString::fromUtf8(
            QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact)
        );
    }

    if (value.isArray()) {
        QJsonObject wrapper;
        wrapper.insert(QStringLiteral("v"), value);

        QString text = QString::fromUtf8(
            QJsonDocument(wrapper).toJson(QJsonDocument::Compact)
        );

        static const QString prefix = QStringLiteral("{\"v\":");

        if (text.startsWith(prefix) && text.endsWith(QStringLiteral("}"))) {
            text.remove(0, prefix.size());
            text.chop(1);
        }

        return text;
    }

    return {};
}
