/**
 * \file  LogFilterProxyModel.h
 * \brief Declares the proxy model used for text and column filtering.
 */


#pragma once

#include <QSortFilterProxyModel>
#include <QString>

//-------------------------------------------------------------------------------------------------
class LogFilterProxyModel final : public QSortFilterProxyModel
{
public:
    explicit LogFilterProxyModel(QObject *parent = nullptr);

    void setTextFilter(const QString &text);
    void setLevelFilter(const QString &level);
    void setLogNameFilter(const QString &logName);
    void setProjectFilter(const QString &project);
    void setProcNameFilter(const QString &procName);
    void setModuleFilter(const QString &module);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString columnName(int column) const;
    int columnByName(const QString &name) const;
    bool levelMatches(int sourceRow, const QModelIndex &sourceParent) const;
    bool logNameMatches(int sourceRow, const QModelIndex &sourceParent) const;
    bool projectMatches(int sourceRow, const QModelIndex &sourceParent) const;
    bool procNameMatches(int sourceRow, const QModelIndex &sourceParent) const;
    bool moduleMatches(int sourceRow, const QModelIndex &sourceParent) const;
    bool columnMatches(int sourceRow, const QModelIndex &sourceParent, const QString &columnName, const QString &value) const;

private:
    QString _textFilter;
    QString _levelFilter;
    QString _logNameFilter;
    QString _projectFilter;
    QString _procNameFilter;
    QString _moduleFilter;
};
//-------------------------------------------------------------------------------------------------
