#pragma once

#include <QStyledItemDelegate>

class QTableView;

class ActiveCellDelegate final : public QStyledItemDelegate
{
public:
    explicit ActiveCellDelegate(QTableView *table, QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QTableView *_table {};
};
