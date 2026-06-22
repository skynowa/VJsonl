/**
 * \file  Utils/TableHeader.h
 * \brief Declares helpers for table column layouts and visibility menus.
 */


#pragma once

#include <QMap>
#include <QStringList>

class QAbstractItemModel;
class QHeaderView;
class QMenu;
class QTableView;
//-------------------------------------------------------------------------------------------------
struct TableLayout final
{
    QStringList        columnOrder;
    QStringList        hiddenColumns;
    QMap<QString, int> columnWidths;
};

//-------------------------------------------------------------------------------------------------
namespace table_header_utils
{

QStringList columnOrder(const QHeaderView *header, const QAbstractItemModel *model);
void        restoreColumnOrder(QHeaderView *header, const QAbstractItemModel *model,
                const QStringList &order);
QStringList hiddenColumns(const QHeaderView *header, const QAbstractItemModel *model);
void        restoreHiddenColumns(QHeaderView *header, const QAbstractItemModel *model,
                const QStringList &hiddenColumnNames);
void        populateColumnVisibilityMenu(QMenu *menu, QTableView *table);
TableLayout captureLayout(const QTableView *table);
void        applyLayout(QTableView *table, const TableLayout &layout);

}
//-------------------------------------------------------------------------------------------------
