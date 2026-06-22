/**
 * \file  TestTableSessionManager.cpp
 * \brief Tests named table layout session persistence and management.
 */


#include "TableSessionManager.h"

#include <QSettings>
#include <QTemporaryDir>
#include <QTest>

//-------------------------------------------------------------------------------------------------
class TestTableSessionManager final : public QObject
{
    Q_OBJECT

private slots:
    void
    managesAndPersistsSessions()
    {
        QTemporaryDir directory;
        QVERIFY(directory.isValid());
        QSettings settings(directory.filePath(QStringLiteral("sessions.ini")), QSettings::IniFormat);

        TableSessionManager manager;
        manager.load(&settings);
        QCOMPARE(manager.names(), QStringList({QStringLiteral("Default")}));
        QCOMPARE(manager.activeName(), QStringLiteral("Default"));

        TableLayout defaultLayout;
        defaultLayout.columnOrder = {QStringLiteral("project"), QStringLiteral("msg")};
        defaultLayout.hiddenColumns = {QStringLiteral("msg")};
        defaultLayout.columnWidths.insert(QStringLiteral("project"), 180);
        manager.setActiveLayout(defaultLayout);

        TableLayout errorsLayout;
        errorsLayout.columnOrder = {QStringLiteral("level"), QStringLiteral("msg")};
        errorsLayout.columnWidths.insert(QStringLiteral("level"), 90);
        QVERIFY(manager.addSession(QStringLiteral("Errors"), errorsLayout));
        QCOMPARE(manager.activeName(), QStringLiteral("Errors"));
        QVERIFY(!manager.addSession(QStringLiteral("errors"), errorsLayout));
        QVERIFY(manager.renameSession(QStringLiteral("Errors"), QStringLiteral("Failures")));
        QVERIFY(!manager.renameSession(QStringLiteral("Failures"), QStringLiteral("default")));

        manager.save(&settings);
        settings.sync();

        TableSessionManager restored;
        restored.load(&settings);
        QCOMPARE(restored.names(), QStringList({QStringLiteral("Default"), QStringLiteral("Failures")}));
        QCOMPARE(restored.activeName(), QStringLiteral("Failures"));
        QCOMPARE(restored.activeLayout().columnOrder, errorsLayout.columnOrder);
        QCOMPARE(restored.activeLayout().columnWidths.value(QStringLiteral("level")), 90);

        QVERIFY(restored.removeSession(QStringLiteral("Failures")));
        QCOMPARE(restored.activeName(), QStringLiteral("Default"));
        QCOMPARE(restored.activeLayout().hiddenColumns, defaultLayout.hiddenColumns);
        QVERIFY(!restored.removeSession(QStringLiteral("Default")));
    }
};

//-------------------------------------------------------------------------------------------------
QTEST_MAIN(TestTableSessionManager)

#include "TestTableSessionManager.moc"
