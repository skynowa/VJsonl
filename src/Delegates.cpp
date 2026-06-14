#include "Delegates.h"

#include <QColor>
#include <QPainter>
#include <QPalette>
#include <QPen>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QTableView>

ActiveCellDelegate::ActiveCellDelegate(QTableView *table, QObject *parent) :
    QStyledItemDelegate(parent),
    _table(table)
{
}

void ActiveCellDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem activeOption(option);

    if (_table != nullptr && index == _table->currentIndex()) {
        activeOption.state |= QStyle::State_Selected;
        activeOption.palette.setColor(QPalette::Highlight, QColor(255, 193, 7));
        activeOption.palette.setColor(QPalette::HighlightedText, Qt::black);
    }

    QStyledItemDelegate::paint(painter, activeOption, index);

    if (_table == nullptr || index != _table->currentIndex()) {
        return;
    }

    painter->save();
    QPen pen(QColor(255, 109, 0));
    pen.setWidth(2);
    painter->setPen(pen);
    painter->drawRect(option.rect.adjusted(1, 1, -2, -2));
    painter->restore();
}
