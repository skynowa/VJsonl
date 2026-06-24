/**
 * \file  TableSessionManager.cpp
 * \brief Implements named table layout session management.
 */


#include "TableSessionManager.h"

#include <QSettings>
#include <QVariantMap>

#include <utility>

namespace
{
//-------------------------------------------------------------------------------------------------
constexpr auto defaultSessionName = "Default";

//-------------------------------------------------------------------------------------------------
QVariantMap
widthsToVariantMap(
    const QMap<QString, int> &widths
)
{
    QVariantMap result;

    for (auto it = widths.cbegin(); it != widths.cend(); ++it) {
        result.insert(it.key(), it.value());
    }

    return result;
}

//-------------------------------------------------------------------------------------------------
QMap<QString, int>
widthsFromVariantMap(
    const QVariantMap &values
)
{
    QMap<QString, int> result;

    for (auto it = values.cbegin(); it != values.cend(); ++it) {
        bool ok = false;
        const int width = it.value().toInt(&ok);

        if (ok && width > 0) {
            result.insert(it.key(), width);
        }
    }

    return result;
}
}

//-------------------------------------------------------------------------------------------------
// Persistence
//-------------------------------------------------------------------------------------------------
void
TableSessionManager::load(
    QSettings *settings
)
{
    _sessions.clear();
    _activeName.clear();

    if (settings != nullptr) {
        settings->beginGroup(QStringLiteral("tableSessions"));
        _activeName = settings->value(QStringLiteral("active")).toString();
        const int count = settings->beginReadArray(QStringLiteral("items"));

        for (int index = 0; index < count; ++index) {
            settings->setArrayIndex(index);
            TableSession session;
            session.name = settings->value(QStringLiteral("name")).toString().trimmed();
            session.layout.columnOrder = settings->value(QStringLiteral("order")).toStringList();
            session.layout.hiddenColumns = settings->value(QStringLiteral("hidden")).toStringList();
            session.layout.columnWidths = widthsFromVariantMap(
                settings->value(QStringLiteral("widths")).toMap()
            );

            if (!session.name.isEmpty() && indexOf(session.name) < 0) {
                _sessions.push_back(std::move(session));
            }
        }

        settings->endArray();
        settings->endGroup();
    }

    if (_sessions.isEmpty()) {
        _sessions.push_back({QString::fromLatin1(defaultSessionName), {}});
    }

    if (indexOf(_activeName) < 0) {
        _activeName = _sessions.constFirst().name;
    }
}

//-------------------------------------------------------------------------------------------------
void
TableSessionManager::save(
    QSettings *settings
) const
{
    if (settings == nullptr) {
        return;
    }

    settings->beginGroup(QStringLiteral("tableSessions"));
    settings->remove(QString());
    settings->setValue(QStringLiteral("active"), _activeName);
    settings->beginWriteArray(QStringLiteral("items"), static_cast<int>(_sessions.size()));

    for (int index = 0; index < _sessions.size(); ++index) {
        const TableSession &session = _sessions.at(index);
        settings->setArrayIndex(index);
        settings->setValue(QStringLiteral("name"), session.name);
        settings->setValue(QStringLiteral("order"), session.layout.columnOrder);
        settings->setValue(QStringLiteral("hidden"), session.layout.hiddenColumns);
        settings->setValue(QStringLiteral("widths"), widthsToVariantMap(session.layout.columnWidths));
    }

    settings->endArray();
    settings->endGroup();
}

//-------------------------------------------------------------------------------------------------
// Session metadata
//-------------------------------------------------------------------------------------------------
QStringList
TableSessionManager::names() const
{
    QStringList result;

    for (const TableSession &session : _sessions) {
        result.push_back(session.name);
    }

    return result;
}

//-------------------------------------------------------------------------------------------------
QString
TableSessionManager::activeName() const
{
    return _activeName;
}

//-------------------------------------------------------------------------------------------------
int
TableSessionManager::sessionCount() const
{
    return static_cast<int>(_sessions.size());
}

//-------------------------------------------------------------------------------------------------
// Session management
//-------------------------------------------------------------------------------------------------
bool
TableSessionManager::addSession(
    const QString     &name,
    const TableLayout &layout
)
{
    const QString normalizedName = name.trimmed();

    if (normalizedName.isEmpty() || indexOf(normalizedName) >= 0) {
        return false;
    }

    _sessions.push_back({normalizedName, layout});
    _activeName = normalizedName;
    return true;
}

//-------------------------------------------------------------------------------------------------
bool
TableSessionManager::renameSession(
    const QString &oldName,
    const QString &newName
)
{
    const int index = indexOf(oldName);
    const QString normalizedName = newName.trimmed();
    const int duplicateIndex = indexOf(normalizedName);

    if (index < 0 || normalizedName.isEmpty() || (duplicateIndex >= 0 && duplicateIndex != index)) {
        return false;
    }

    const bool active = _sessions.at(index).name.compare(_activeName, Qt::CaseInsensitive) == 0;
    _sessions[index].name = normalizedName;

    if (active) {
        _activeName = normalizedName;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool
TableSessionManager::removeSession(
    const QString &name
)
{
    const int index = indexOf(name);

    if (index < 0 || _sessions.size() <= 1) {
        return false;
    }

    const bool active = _sessions.at(index).name.compare(_activeName, Qt::CaseInsensitive) == 0;
    _sessions.removeAt(index);

    if (active) {
        _activeName = _sessions.constFirst().name;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool
TableSessionManager::setActiveName(
    const QString &name
)
{
    const int index = indexOf(name);

    if (index < 0) {
        return false;
    }

    _activeName = _sessions.at(index).name;
    return true;
}

//-------------------------------------------------------------------------------------------------
// Active session layout
//-------------------------------------------------------------------------------------------------
TableLayout
TableSessionManager::activeLayout() const
{
    const int index = indexOf(_activeName);
    return index >= 0 ? _sessions.at(index).layout : TableLayout {};
}

//-------------------------------------------------------------------------------------------------
void
TableSessionManager::setActiveLayout(
    const TableLayout &layout
)
{
    const int index = indexOf(_activeName);

    if (index >= 0) {
        _sessions[index].layout = layout;
    }
}

//-------------------------------------------------------------------------------------------------
// Lookup helpers
//-------------------------------------------------------------------------------------------------
int
TableSessionManager::indexOf(
    const QString &name
) const
{
    for (int index = 0; index < _sessions.size(); ++index) {
        if (_sessions.at(index).name.compare(name, Qt::CaseInsensitive) == 0) {
            return index;
        }
    }

    return -1;
}
//-------------------------------------------------------------------------------------------------
