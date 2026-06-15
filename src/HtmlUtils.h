/**
 * \file  HtmlUtils.h
 * \brief Declares HTML detection helpers for preview switching.
 */


#pragma once

#include <QString>

namespace HtmlUtils
{
bool looksLikeHtml(const QString &text);
}
