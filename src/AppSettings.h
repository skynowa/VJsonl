/**
 * \file  AppSettings.h
 * \brief Declares helpers for locating application settings.
 */


#pragma once

#include <QString>

class QSettings;
//-------------------------------------------------------------------------------------------------
QString settingsFileName();
bool    syncSettings(QSettings *settings, QString *outError = nullptr);
//-------------------------------------------------------------------------------------------------
