#include <QtGui/QApplication>

#include "gui/mainwindow.h"

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );
    Q_INIT_RESOURCE( icons );
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
