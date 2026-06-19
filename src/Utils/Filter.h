/**
 * \file  Utils/Filter.h
 * \brief Declares helpers for table filter widgets.
 */


#pragma once

#include <QString>

class QAbstractItemModel;
class QComboBox;
//-------------------------------------------------------------------------------------------------
namespace filter_utils
{

QString selectedFilterValue(const QComboBox *filter);
int     columnByName(const QAbstractItemModel *model, const QString &name);

}
//-------------------------------------------------------------------------------------------------
