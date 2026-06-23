/**
 * \file  Utils/Icon.cpp
 * \brief Implements shared icon helpers.
 */


#include "Utils/Icon.h"

//-------------------------------------------------------------------------------------------------
static void
initResources()
{
    Q_INIT_RESOURCE(VJson);
}

//-------------------------------------------------------------------------------------------------
namespace icon_utils
{
QIcon
appIcon()
{
    initResources();
    return QIcon(QStringLiteral(":/icons/vjson.png"));
}

//-------------------------------------------------------------------------------------------------
QIcon
calendarIcon()
{
    initResources();
    return QIcon(QStringLiteral(":/icons/calendar.png"));
}

//-------------------------------------------------------------------------------------------------
QIcon
copyIcon()
{
    initResources();
    return QIcon(QStringLiteral(":/icons/copy.png"));
}

//-------------------------------------------------------------------------------------------------
QIcon
copyFormattedIcon()
{
    initResources();
    return QIcon(QStringLiteral(":/icons/copy-formatted.png"));
}

//-------------------------------------------------------------------------------------------------
QIcon
invalidRowIcon()
{
    initResources();
    return QIcon(QStringLiteral(":/icons/invalid-row.png"));
}

//-------------------------------------------------------------------------------------------------
QIcon
sessionIcon()
{
    initResources();
    QIcon icon;
    icon.addFile(QStringLiteral(":/icons/session.png"), {}, QIcon::Normal, QIcon::Off);
    icon.addFile(QStringLiteral(":/icons/session-active.png"), {}, QIcon::Normal, QIcon::On);
    return icon;
}
}
//-------------------------------------------------------------------------------------------------
