/**
 * \file  ThemeManager.h
 * \brief Declares application theme helpers.
 */


#pragma once

#include <QString>

class QApplication;
//-------------------------------------------------------------------------------------------------
namespace ThemeManager
{

// Supported application themes
enum class Theme
{
    Light,
    LightGray,
    Grey,
    MediumGray,
    DarkGrey,
    Dark
};

// Theme conversion and application
Theme   themeFromString(const QString &value);
QString themeToString(Theme theme);
void    applyTheme(QApplication *application, Theme theme);

}
//-------------------------------------------------------------------------------------------------
