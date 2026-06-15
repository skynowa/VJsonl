/**
 * \file  LogFilterProxyModel.h
 * \brief Declares the proxy model used for text, level, and log-name filtering.
 */


#pragma once

#include <QSortFilterProxyModel>
#include <QString>

class LogFilterProxyModel final : public QSortFilterProxyModel
{
public:
    explicit LogFilterProxyModel(QObject *parent = nullptr);

    void setTextFilter(const QString &text);
    void setLevelFilter(const QString &level);
    void setLogNameFilter(const QString &logName);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString columnName(int column) const;
    int columnByName(const QString &name) const;
    bool levelMatches(int sourceRow, const QModelIndex &sourceParent) const;
    bool logNameMatches(int sourceRow, const QModelIndex &sourceParent) const;
    bool columnMatches(int sourceRow, const QModelIndex &sourceParent, const QString &columnName, const QString &value) const;

private:
    QString _textFilter;
    QString _levelFilter;
    QString _logNameFilter;
};
