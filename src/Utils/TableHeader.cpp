/**
 * \file  Utils/TableHeader.cpp
 * \brief Implements helpers for saving and restoring table column order.
 */


#include "Utils/TableHeader.h"

#include <QAbstractItemModel>
#include <QHeaderView>
#include <QSet>

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
}
//-------------------------------------------------------------------------------------------------
