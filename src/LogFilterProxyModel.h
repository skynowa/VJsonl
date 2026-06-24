/**
 * \file  LogFilterProxyModel.h
 * \brief Declares the proxy model used for text and column filtering.
 */


#pragma once

#include <QDateTime>
#include <QMap>
#include <QSortFilterProxyModel>
#include <QString>
//-------------------------------------------------------------------------------------------------
class LogFilterProxyModel final :
    public QSortFilterProxyModel
{
public:
    explicit LogFilterProxyModel(QObject *parent = nullptr);

    // Global and column-specific filter setters
    void setTextFilter(const QString &text);
    void setMsgTextFilter(const QString &text);
    void setLevelFilter(const QString &level);
    void setLogNameFilter(const QString &logName);
    void setProjectFilter(const QString &project);
    void setAppFilter(const QString &app);
    void setProcNameFilter(const QString &procName);
    void setModuleFilter(const QString &module);
    void setDescrFilter(const QString &descr);
    void setQueryFilter(const QString &query);
    void setRequestTextFilter(const QString &text);
    void setPageTextFilter(const QString &text);
    void setTimestampRange(const QDateTime &from, bool hasFrom, const QDateTime &to, bool hasTo);

protected:
    // QSortFilterProxyModel interface
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    // Filter storage and column lookup helpers
    void setColumnFilter(const QString &columnName, const QString &value);
    QString columnName(int column) const;
    int  columnByName(const QString &name) const;

    // Row matching helpers
    bool columnMatches(int sourceRow, const QModelIndex &sourceParent, const QString &columnName,
            const QString &value) const;
    bool textColumnMatches(int sourceRow, const QModelIndex &sourceParent, const
            QString &columnName, const QString &text) const;
    bool timestampMatches(int sourceRow, const QModelIndex &sourceParent) const;

private:
    // Text filters
    QString                _textFilter;
    QString                _msgTextFilter;
    QString                _queryTextFilter;
    QString                _requestTextFilter;
    QString                _pageTextFilter;

    // Exact-match column filters
    QMap<QString, QString> _columnFilters;

    // Timestamp range filter
    QDateTime              _timestampFrom;
    QDateTime              _timestampTo;
    bool                   _hasTimestampFrom {};
    bool                   _hasTimestampTo {};
};
//-------------------------------------------------------------------------------------------------
