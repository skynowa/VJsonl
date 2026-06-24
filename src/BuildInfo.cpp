/**
 * \file  BuildInfo.cpp
 * \brief Implements helpers for build and runtime information.
 */


#include "BuildInfo.h"

#include <QSysInfo>

//-------------------------------------------------------------------------------------------------
namespace BuildInfo
{
//-------------------------------------------------------------------------------------------------
// Build/runtime values shown in About
//-------------------------------------------------------------------------------------------------
QString
cxxStandard()
{
#if __cplusplus >= 202302L
    return QStringLiteral("C++23");
#elif __cplusplus >= 202002L
    return QStringLiteral("C++20");
#elif __cplusplus >= 201703L
    return QStringLiteral("C++17");
#elif __cplusplus >= 201402L
    return QStringLiteral("C++14");
#elif __cplusplus >= 201103L
    return QStringLiteral("C++11");
#else
    return QStringLiteral("pre-C++11");
#endif
}
//-------------------------------------------------------------------------------------------------
QString
buildDate()
{
    return QStringLiteral(__DATE__ " " __TIME__);
}
//-------------------------------------------------------------------------------------------------
QString
buildType()
{
#ifdef VJSON_BUILD_TYPE
    const QString type = QStringLiteral(VJSON_BUILD_TYPE);

    if (!type.isEmpty()) {
        return type;
    }
#endif

#ifdef NDEBUG
    return QStringLiteral("Release");
#else
    return QStringLiteral("Debug");
#endif
}
//-------------------------------------------------------------------------------------------------
QString
osInfo()
{
    const QString prettyName = QSysInfo::prettyProductName();

    if (!prettyName.isEmpty()) {
        return prettyName;
    }

    return QStringLiteral("%1 %2")
        .arg(QSysInfo::kernelType(), QSysInfo::kernelVersion())
        .trimmed();
}
}
//-------------------------------------------------------------------------------------------------
