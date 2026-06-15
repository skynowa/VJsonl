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
enum class Theme
{
    Light,
    Grey,
    DarkGrey,
    Dark
};

Theme themeFromString(const QString &value);
QString themeToString(Theme theme);
void applyTheme(QApplication *application, Theme theme);
}
//-------------------------------------------------------------------------------------------------
