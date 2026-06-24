/**
 * \file  Utils/TableHeader.cpp
 * \brief Implements helpers for table column layouts and visibility menus.
 */


#include "Utils/TableHeader.h"

#include "Utils/Filter.h"

#include <QAction>
#include <QAbstractItemModel>
#include <QHeaderView>
#include <QMenu>
#include <QSet>
#include <QTableView>

//-------------------------------------------------------------------------------------------------
namespace table_header_utils
{
//-------------------------------------------------------------------------------------------------
// Column order persistence
//-------------------------------------------------------------------------------------------------
QStringList
columnOrder(
    const QHeaderView        *header,
    const QAbstractItemModel *model
)
{
    QStringList order;

    if (header == nullptr || model == nullptr) {
        return order;
    }

    for (int visualIndex = 0; visualIndex < header->count(); ++visualIndex) {
        const int logicalIndex = header->logicalIndex(visualIndex);
        const QString name = model->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole).toString();

        if (!name.isEmpty()) {
            order.push_back(name);
        }
    }

    return order;
}

//-------------------------------------------------------------------------------------------------
void
restoreColumnOrder(
    QHeaderView              *header,
    const QAbstractItemModel *model,
    const QStringList        &order
)
{
    if (header == nullptr || model == nullptr) {
        return;
    }

    int targetVisualIndex = 0;
    QSet<int> restoredColumns;

    for (const QString &name : order) {
        int logicalIndex = -1;

        for (int column = 0; column < model->columnCount(); ++column) {
            if (model->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString() == name) {
                logicalIndex = column;
                break;
            }
        }

        if (logicalIndex < 0 || restoredColumns.contains(logicalIndex)) {
            continue;
        }

        const int currentVisualIndex = header->visualIndex(logicalIndex);

        if (currentVisualIndex < 0) {
            continue;
        }

        if (currentVisualIndex != targetVisualIndex) {
            header->moveSection(currentVisualIndex, targetVisualIndex);
        }

        restoredColumns.insert(logicalIndex);
        ++targetVisualIndex;
    }
}

//-------------------------------------------------------------------------------------------------
// Column visibility persistence and menu
//-------------------------------------------------------------------------------------------------
QStringList
hiddenColumns(
    const QHeaderView        *header,
    const QAbstractItemModel *model
)
{
    QStringList columns;

    if (header == nullptr || model == nullptr) {
        return columns;
    }

    for (int column = 0; column < model->columnCount(); ++column) {
        if (!header->isSectionHidden(column)) {
            continue;
        }

        const QString name = model->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();

        if (!name.isEmpty()) {
            columns.push_back(name);
        }
    }

    return columns;
}

//-------------------------------------------------------------------------------------------------
void
restoreHiddenColumns(
    QHeaderView              *header,
    const QAbstractItemModel *model,
    const QStringList        &hiddenColumnNames
)
{
    if (header == nullptr || model == nullptr) {
        return;
    }

    for (int column = 0; column < model->columnCount(); ++column) {
        header->setSectionHidden(column, false);
    }

    for (const QString &name : hiddenColumnNames) {
        const int column = filter_utils::columnByName(model, name);

        if (column >= 0) {
            header->setSectionHidden(column, true);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void
populateColumnVisibilityMenu(
    QMenu      *menu,
    QTableView *table
)
{
    if (menu == nullptr || table == nullptr || table->model() == nullptr) {
        return;
    }

    menu->clear();
    QHeaderView *header = table->horizontalHeader();
    int visibleColumnCount = 0;

    for (int column = 0; column < table->model()->columnCount(); ++column) {
        if (!table->isColumnHidden(column)) {
            ++visibleColumnCount;
        }
    }

    for (int visualIndex = 0; visualIndex < header->count(); ++visualIndex) {
        const int logicalIndex = header->logicalIndex(visualIndex);
        const QString name = table->model()->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole).toString();

        if (name.isEmpty()) {
            continue;
        }

        const bool visible = !table->isColumnHidden(logicalIndex);
        QAction *action = menu->addAction(name);
        action->setCheckable(true);
        action->setChecked(visible);
        action->setEnabled(!visible || visibleColumnCount > 1);

        QObject::connect(action, &QAction::toggled, table, [table, logicalIndex](bool checked) {
            table->setColumnHidden(logicalIndex, !checked);
        });
    }
}

//-------------------------------------------------------------------------------------------------
// Complete table layout snapshots
//-------------------------------------------------------------------------------------------------
TableLayout
captureLayout(
    const QTableView *table
)
{
    TableLayout layout;

    if (table == nullptr || table->model() == nullptr) {
        return layout;
    }

    layout.columnOrder = columnOrder(table->horizontalHeader(), table->model());
    layout.hiddenColumns = hiddenColumns(table->horizontalHeader(), table->model());

    for (int column = 0; column < table->model()->columnCount(); ++column) {
        const QString name = table->model()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();

        if (!name.isEmpty()) {
            layout.columnWidths.insert(name, table->columnWidth(column));
        }
    }

    return layout;
}

//-------------------------------------------------------------------------------------------------
void
applyLayout(
    QTableView        *table,
    const TableLayout &layout
)
{
    if (table == nullptr || table->model() == nullptr) {
        return;
    }

    restoreColumnOrder(table->horizontalHeader(), table->model(), layout.columnOrder);
    restoreHiddenColumns(table->horizontalHeader(), table->model(), layout.hiddenColumns);

    for (auto it = layout.columnWidths.cbegin(); it != layout.columnWidths.cend(); ++it) {
        const int column = filter_utils::columnByName(table->model(), it.key());

        if (column >= 0 && it.value() > 0) {
            table->setColumnWidth(column, it.value());
        }
    }
}
}
//-------------------------------------------------------------------------------------------------
