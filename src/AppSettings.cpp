/**
 * \file  AppSettings.cpp
 * \brief Implements helpers for locating application settings.
 */


#include "AppSettings.h"

#include <QCoreApplication>
#include <QSettings>

//-------------------------------------------------------------------------------------------------
QString
settingsFileName()
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/VJson.ini");
}

//-------------------------------------------------------------------------------------------------
bool
syncSettings(
    QSettings *settings,
    QString   *outError
)
{
    if (outError != nullptr) {
        outError->clear();
    }

    if (settings == nullptr) {
        if (outError != nullptr) {
            *outError = QStringLiteral("Settings object is null");
        }

        return false;
    }

    settings->sync();

    if (settings->status() == QSettings::NoError) {
        return true;
    }

    if (outError != nullptr) {
        *outError = QStringLiteral("Failed to save settings");
    }

    return false;
}
//-------------------------------------------------------------------------------------------------
