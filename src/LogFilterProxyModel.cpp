#include "LogFilterProxyModel.h"

#include <QAbstractItemModel>

LogFilterProxyModel::LogFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

void LogFilterProxyModel::setTextFilter(const QString &text)
{
    if (_textFilter == text) {
        return;
    }

    _textFilter = text;
    invalidateFilter();
}

void LogFilterProxyModel::setLevelFilter(const QString &level)
{
    if (_levelFilter == level) {
        return;
    }

    _levelFilter = level;
    invalidateFilter();
}

bool LogFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (!levelMatches(sourceRow, sourceParent)) {
        return false;
    }

    if (_textFilter.isEmpty()) {
        return true;
    }

    const int columns = sourceModel()->columnCount(sourceParent);

    for (int column = 0; column < columns; ++column) {
        const QModelIndex index = sourceModel()->index(sourceRow, column, sourceParent);
        const QString text = sourceModel()->data(index, Qt::DisplayRole).toString();

        if (text.contains(_textFilter, Qt::CaseInsensitive)) {
            return true;
        }
    }

    return false;
}

bool LogFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (columnName(left.column()) == QStringLiteral("line")) {
        return sourceModel()->data(left, Qt::DisplayRole).toInt()
            < sourceModel()->data(right, Qt::DisplayRole).toInt();
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

QString LogFilterProxyModel::columnName(int column) const
{
    return sourceModel()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
}

int LogFilterProxyModel::levelColumn() const
{
    const int columns = sourceModel()->columnCount();

    for (int column = 0; column < columns; ++column) {
        if (columnName(column) == QStringLiteral("level")) {
            return column;
        }
    }

    return -1;
}

bool LogFilterProxyModel::levelMatches(int sourceRow, const QModelIndex &sourceParent) const
{
    if (_levelFilter.isEmpty()) {
        return true;
    }

    const int column = levelColumn();

    if (column < 0) {
        return false;
    }

    const QModelIndex index = sourceModel()->index(sourceRow, column, sourceParent);
    const QString level = sourceModel()->data(index, Qt::DisplayRole).toString();
    return level.compare(_levelFilter, Qt::CaseInsensitive) == 0;
}
