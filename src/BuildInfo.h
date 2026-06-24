/**
 * \file  BuildInfo.h
 * \brief Declares helpers for build and runtime information.
 */


#pragma once

#include <QString>
//-------------------------------------------------------------------------------------------------
namespace BuildInfo
{

// Build/runtime values shown in About
QString cxxStandard();
QString buildDate();
QString buildType();
QString osInfo();

}
//-------------------------------------------------------------------------------------------------
