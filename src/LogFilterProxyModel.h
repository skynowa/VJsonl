#pragma once

#include <QSortFilterProxyModel>
#include <QString>

class LogFilterProxyModel final : public QSortFilterProxyModel
{
public:
    explicit LogFilterProxyModel(QObject *parent = nullptr);

    void setTextFilter(const QString &text);
    void setLevelFilter(const QString &level);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QString columnName(int column) const;
    int levelColumn() const;
    bool levelMatches(int sourceRow, const QModelIndex &sourceParent) const;

private:
    QString _textFilter;
    QString _levelFilter;
};
