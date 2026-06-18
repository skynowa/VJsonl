/**
 * \file  Utils/TableHeader.cpp
 * \brief Implements helpers for saving and restoring table column order.
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
QStringList columnOrder(const QHeaderView *header, const QAbstractItemModel *model)
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
void restoreColumnOrder(QHeaderView *header, const QAbstractItemModel *model, const QStringList &order)
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
QStringList hiddenColumns(const QHeaderView *header, const QAbstractItemModel *model)
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
void restoreHiddenColumns(QHeaderView *header, const QAbstractItemModel *model, const QStringList &hiddenColumnNames)
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
void populateColumnVisibilityMenu(QMenu *menu, QTableView *table)
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
}
//-------------------------------------------------------------------------------------------------
