/**
 * \file  main.cpp
 * \brief Starts the VJson Qt application.
 */


#include "MainWindow.h"
#include "Utils/Icon.h"

#include <QApplication>

//-------------------------------------------------------------------------------------------------
int
main(
    int  argc,
    char *argv[]
)
{
    QApplication app(argc, argv);
    app.setWindowIcon(icon_utils::appIcon());

    MainWindow window;
    window.show();

    return app.exec();
}
//-------------------------------------------------------------------------------------------------
