#include "MainWindow.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("Skynowa"));
    QCoreApplication::setApplicationName(QStringLiteral("VJson"));

    MainWindow window;
    window.show();

    return app.exec();
}
