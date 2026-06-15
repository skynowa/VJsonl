/**
 * \file  AppSettings.cpp
 * \brief Implements helpers for locating application settings.
 */


#include "AppSettings.h"

#include <QCoreApplication>

//-------------------------------------------------------------------------------------------------
QString settingsFileName()
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/VJson.ini");
}
//-------------------------------------------------------------------------------------------------
