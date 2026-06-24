/**
 * \file  Utils/Icon.h
 * \brief Declares shared icon helpers.
 */


#pragma once

#include <QIcon>
//-------------------------------------------------------------------------------------------------
namespace icon_utils
{

// Application and toolbar icons
QIcon appIcon();
QIcon calendarIcon();
QIcon copyIcon();
QIcon copyFormattedIcon();

// Table/status icons
QIcon invalidRowIcon();
QIcon sessionIcon();

}
//-------------------------------------------------------------------------------------------------
