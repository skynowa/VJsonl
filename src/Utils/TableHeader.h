/**
 * \file  Utils/TableHeader.h
 * \brief Declares helpers for saving and restoring table column order.
 */


#pragma once

#include <QStringList>

class QAbstractItemModel;
class QHeaderView;

//-------------------------------------------------------------------------------------------------
namespace table_header_utils
{
QStringList columnOrder(const QHeaderView *header, const QAbstractItemModel *model);
void restoreColumnOrder(QHeaderView *header, const QAbstractItemModel *model, const QStringList &order);
}
//-------------------------------------------------------------------------------------------------
