/**
 * \file  main.cpp
 * \brief Starts the VJson Qt application.
 */


#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
