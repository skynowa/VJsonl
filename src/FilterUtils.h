/**
 * \file  FilterUtils.h
 * \brief Declares helpers for table filter widgets.
 */


#pragma once

#include <QString>

class QAbstractItemModel;
class QComboBox;

//-------------------------------------------------------------------------------------------------
namespace FilterUtils
{
QString selectedFilterValue(const QComboBox *filter);
int columnByName(const QAbstractItemModel *model, const QString &name);
}
//-------------------------------------------------------------------------------------------------
