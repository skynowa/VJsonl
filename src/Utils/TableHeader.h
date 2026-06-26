/**
 * \file  Utils/TableHeader.h
 * \brief Declares helpers for table column layouts and visibility menus.
 */


#pragma once

#include <QHash>
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
    QHash<QString, int> columnWidths;
};

//-------------------------------------------------------------------------------------------------
namespace table_header_utils
{

// Column order persistence
QStringList columnOrder(const QHeaderView *header, const QAbstractItemModel *model);
void        restoreColumnOrder(QHeaderView *header, const QAbstractItemModel *model,
                const QStringList &order);

// Column visibility persistence and menu
QStringList hiddenColumns(const QHeaderView *header, const QAbstractItemModel *model);
void        restoreHiddenColumns(QHeaderView *header, const QAbstractItemModel *model,
                const QStringList &hiddenColumnNames);
void        populateColumnVisibilityMenu(QMenu *menu, QTableView *table);

// Complete table layout snapshots
TableLayout captureLayout(const QTableView *table);
void        applyLayout(QTableView *table, const TableLayout &layout);

}
//-------------------------------------------------------------------------------------------------
