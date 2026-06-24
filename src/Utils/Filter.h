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

// Filter widget state
QString selectedFilterValue(const QComboBox *filter);

// Model column lookup
int     columnByName(const QAbstractItemModel *model, const QString &name);

}
//-------------------------------------------------------------------------------------------------
