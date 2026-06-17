/**
 * \file  LogFilterProxyModel.cpp
 * \brief Implements filtering and sorting for log table rows.
 */


#include "LogFilterProxyModel.h"

#include "TimestampUtils.h"

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
void LogFilterProxyModel::setMsgTextFilter(const QString &text)
{
    if (_msgTextFilter == text) {
        return;
    }

    _msgTextFilter = text;
    invalidateFilter();
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setLevelFilter(const QString &level)
{
    setColumnFilter(QStringLiteral("level"), level);
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setLogNameFilter(const QString &logName)
{
    setColumnFilter(QStringLiteral("log_name"), logName);
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setProjectFilter(const QString &project)
{
    setColumnFilter(QStringLiteral("project"), project);
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setAppFilter(const QString &app)
{
    setColumnFilter(QStringLiteral("app"), app);
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setProcNameFilter(const QString &procName)
{
    setColumnFilter(QStringLiteral("proc_name"), procName);
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setModuleFilter(const QString &module)
{
    setColumnFilter(QStringLiteral("module"), module);
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setQueryFilter(const QString &query)
{
    setColumnFilter(QStringLiteral("query"), query);
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setTimestampRange(
    const QDateTime &from,
    bool hasFrom,
    const QDateTime &to,
    bool hasTo
)
{
    const bool changed =
        _hasTimestampFrom != hasFrom
        || _hasTimestampTo != hasTo
        || _timestampFrom != from
        || _timestampTo != to;

    if (!changed) {
        return;
    }

    _timestampFrom = from;
    _timestampTo = to;
    _hasTimestampFrom = hasFrom;
    _hasTimestampTo = hasTo;
    invalidateFilter();
}

//-------------------------------------------------------------------------------------------------
void LogFilterProxyModel::setColumnFilter(const QString &columnName, const QString &value)
{
    const QString normalizedValue = value.trimmed();

    if (normalizedValue.isEmpty()) {
        if (_columnFilters.remove(columnName) > 0) {
            invalidateFilter();
        }

        return;
    }

    if (_columnFilters.value(columnName) == normalizedValue) {
        return;
    }

    _columnFilters.insert(columnName, normalizedValue);
    invalidateFilter();
}

//-------------------------------------------------------------------------------------------------
bool LogFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    for (auto it = _columnFilters.cbegin(); it != _columnFilters.cend(); ++it) {
        if (!columnMatches(sourceRow, sourceParent, it.key(), it.value())) {
            return false;
        }
    }

    if (!timestampMatches(sourceRow, sourceParent)) {
        return false;
    }

    if (!textColumnMatches(sourceRow, sourceParent, QStringLiteral("msg"), _msgTextFilter)) {
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
bool LogFilterProxyModel::textColumnMatches(
    int sourceRow,
    const QModelIndex &sourceParent,
    const QString &columnName,
    const QString &text
) const
{
    if (text.isEmpty()) {
        return true;
    }

    const int column = columnByName(columnName);

    if (column < 0) {
        return false;
    }

    const QModelIndex index = sourceModel()->index(sourceRow, column, sourceParent);
    const QString value = sourceModel()->data(index, Qt::DisplayRole).toString();
    return value.contains(text, Qt::CaseInsensitive);
}

//-------------------------------------------------------------------------------------------------
bool LogFilterProxyModel::timestampMatches(int sourceRow, const QModelIndex &sourceParent) const
{
    if (!_hasTimestampFrom && !_hasTimestampTo) {
        return true;
    }

    const int column = columnByName(QStringLiteral("ts"));

    if (column < 0) {
        return false;
    }

    const QModelIndex index = sourceModel()->index(sourceRow, column, sourceParent);
    const QDateTime timestamp = TimestampUtils::parseTimestamp(sourceModel()->data(index, Qt::ToolTipRole).toString());

    if (!timestamp.isValid()) {
        return false;
    }

    if (_hasTimestampFrom && timestamp < _timestampFrom) {
        return false;
    }

    if (_hasTimestampTo && timestamp > _timestampTo) {
        return false;
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
