#include <QtGui/QApplication>
#include <QtCore/QDebug>

#include "data/datastore.h"
#include "gui/mainwindow.h"

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );
    Q_INIT_RESOURCE( icons );

    QStringList args = app.arguments();

    DataStore* dataStore = new DataStore();
    for ( int i = 1; i < args.size(); ++i )
    {
        dataStore->load( args.at( i ) );
        //qDebug() << i << " : " << args.at( i );
    }

    MainWindow mainWin( dataStore );
    mainWin.show();


    return app.exec();
}
