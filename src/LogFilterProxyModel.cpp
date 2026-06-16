/**
 * \file  LogFilterProxyModel.cpp
 * \brief Implements filtering and sorting for log table rows.
 */


#include "LogFilterProxyModel.h"

#include <QAbstractItemModel>

//-------------------------------------------------------------------------------------------------
LogFilterProxyModel::LogFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setTextFilter(const QString &text)
{
    if (_textFilter == text) {
        return;
    }

    _textFilter = text;
    invalidateFilter();
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setLevelFilter(const QString &level)
{
    if (_levelFilter == level) {
        return;
    }

    _levelFilter = level;
    invalidateFilter();
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setLogNameFilter(const QString &logName)
{
    if (_logNameFilter == logName) {
        return;
    }

    _logNameFilter = logName;
    invalidateFilter();
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setProjectFilter(const QString &project)
{
    if (_projectFilter == project) {
        return;
    }

    _projectFilter = project;
    invalidateFilter();
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setProcNameFilter(const QString &procName)
{
    if (_procNameFilter == procName) {
        return;
    }

    _procNameFilter = procName;
    invalidateFilter();
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setModuleFilter(const QString &module)
{
    if (_moduleFilter == module) {
        return;
    }

    _moduleFilter = module;
    invalidateFilter();
}

//-------------------------------------------------------------------------------------------------
bool LogFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if (!projectMatches(sourceRow, sourceParent)) {
        return false;
    }

    if (!procNameMatches(sourceRow, sourceParent)) {
        return false;
    }

    if (!moduleMatches(sourceRow, sourceParent)) {
        return false;
    }

    if (!levelMatches(sourceRow, sourceParent)) {
        return false;
    }

    if (!logNameMatches(sourceRow, sourceParent)) {
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

//-------------------------------------------------------------------------------------------------
bool LogFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (columnName(left.column()) == QStringLiteral("line")) {
        return sourceModel()->data(left, Qt::DisplayRole).toInt()
            < sourceModel()->data(right, Qt::DisplayRole).toInt();
    }

    if (columnName(left.column()) == QStringLiteral("ts")) {
        return sourceModel()->data(left, Qt::ToolTipRole).toString()
            < sourceModel()->data(right, Qt::ToolTipRole).toString();
    }

    return QSortFilterProxyModel::lessThan(left, right);
}

//-------------------------------------------------------------------------------------------------
QString LogFilterProxyModel::columnName(int column) const
{
    return sourceModel()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
}

//-------------------------------------------------------------------------------------------------
int LogFilterProxyModel::columnByName(const QString &name) const
{
    const int columns = sourceModel()->columnCount();

    for (int column = 0; column < columns; ++column) {
        if (columnName(column) == name) {
            return column;
        }
    }

    return -1;
}

//-------------------------------------------------------------------------------------------------
bool LogFilterProxyModel::levelMatches(int sourceRow, const QModelIndex &sourceParent) const
{
    return columnMatches(sourceRow, sourceParent, QStringLiteral("level"), _levelFilter);
}

//-------------------------------------------------------------------------------------------------
bool LogFilterProxyModel::logNameMatches(int sourceRow, const QModelIndex &sourceParent) const
{
    return columnMatches(sourceRow, sourceParent, QStringLiteral("log_name"), _logNameFilter);
}

//-------------------------------------------------------------------------------------------------
bool LogFilterProxyModel::projectMatches(int sourceRow, const QModelIndex &sourceParent) const
{
    return columnMatches(sourceRow, sourceParent, QStringLiteral("project"), _projectFilter);
}

//-------------------------------------------------------------------------------------------------
bool LogFilterProxyModel::procNameMatches(int sourceRow, const QModelIndex &sourceParent) const
{
    return columnMatches(sourceRow, sourceParent, QStringLiteral("proc_name"), _procNameFilter);
}

//-------------------------------------------------------------------------------------------------
bool LogFilterProxyModel::moduleMatches(int sourceRow, const QModelIndex &sourceParent) const
{
    return columnMatches(sourceRow, sourceParent, QStringLiteral("module"), _moduleFilter);
}

//-------------------------------------------------------------------------------------------------
bool LogFilterProxyModel::columnMatches(
    int sourceRow,
    const QModelIndex &sourceParent,
    const QString &columnName,
    const QString &value
) const
{
    if (value.isEmpty()) {
        return true;
    }

    const int column = columnByName(columnName);

    if (column < 0) {
        return false;
    }

    const QModelIndex index = sourceModel()->index(sourceRow, column, sourceParent);
    const QString text = sourceModel()->data(index, Qt::DisplayRole).toString();
    return text.compare(value, Qt::CaseInsensitive) == 0;
}
//-------------------------------------------------------------------------------------------------
