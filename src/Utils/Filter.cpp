/**
 * \file  Utils/Filter.cpp
 * \brief Implements helpers for table filter widgets.
 */


#include "Utils/Filter.h"

#include <QAbstractItemModel>
#include <QComboBox>

//-------------------------------------------------------------------------------------------------
namespace filter_utils
{
//-------------------------------------------------------------------------------------------------
// Filter widget state
//-------------------------------------------------------------------------------------------------
QString
selectedFilterValue(
    const QComboBox *filter
)
{
    if (filter == nullptr) {
        return {};
    }

    const QString text = filter->currentText().trimmed();

    if (filter->currentIndex() <= 0 || text.startsWith(QStringLiteral("All "), Qt::CaseInsensitive)) {
        return {};
    }

    const QString value = filter->currentData().toString().trimmed();
    return value.isEmpty() ? text : value;
}

//-------------------------------------------------------------------------------------------------
// Model column lookup
//-------------------------------------------------------------------------------------------------
int
columnByName(
    const QAbstractItemModel *model,
    const QString            &name
)
{
    if (model == nullptr) {
        return -1;
    }

    for (int column = 0; column < model->columnCount(); ++column) {
        if (model->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString() == name) {
            return column;
        }
    }

    return -1;
}
}
//-------------------------------------------------------------------------------------------------
