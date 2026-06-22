/**
 * \file  TableSessionManager.h
 * \brief Declares named table layout session management.
 */


#pragma once

#include "Utils/TableHeader.h"

#include <QStringList>
#include <QVector>

class QSettings;

//-------------------------------------------------------------------------------------------------
struct TableSession final
{
    QString     name;
    TableLayout layout;
};

//-------------------------------------------------------------------------------------------------
class TableSessionManager final
{
public:
    void load(QSettings *settings);
    void save(QSettings *settings) const;

    QStringList names() const;
    QString     activeName() const;
    int         sessionCount() const;

    bool addSession(const QString &name, const TableLayout &layout);
    bool renameSession(const QString &oldName, const QString &newName);
    bool removeSession(const QString &name);
    bool setActiveName(const QString &name);

    TableLayout activeLayout() const;
    void        setActiveLayout(const TableLayout &layout);

private:
    int indexOf(const QString &name) const;

private:
    QVector<TableSession> _sessions;
    QString               _activeName;
};
//-------------------------------------------------------------------------------------------------
