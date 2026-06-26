/**
 * \file  Delegates.h
 * \brief Declares custom item delegates used by the log table.
 */


#pragma once

#include <QStyledItemDelegate>

//-------------------------------------------------------------------------------------------------
class QTableView;
//-------------------------------------------------------------------------------------------------
class ActiveCellDelegate final :
    public QStyledItemDelegate
{
public:
    // Construction
    explicit ActiveCellDelegate(QTableView *table, QObject *parent = nullptr);
    Q_DISABLE_COPY_MOVE(ActiveCellDelegate)

    // QStyledItemDelegate interface
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
            const override;

protected:
    // Style preparation
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;

private:
    // Associated table view
    QTableView *_table {};
};
//-------------------------------------------------------------------------------------------------
