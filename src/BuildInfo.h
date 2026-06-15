/**
 * \file  BuildInfo.h
 * \brief Declares helpers for build and runtime information.
 */


#pragma once

#include <QString>

//-------------------------------------------------------------------------------------------------
namespace BuildInfo
{
QString cxxStandard();
QString buildDate();
QString buildType();
QString osInfo();
}
//-------------------------------------------------------------------------------------------------
