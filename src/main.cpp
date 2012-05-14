#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>

#include "data/datastore.h"
#include "gui/mainwindow.h"

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    QCoreApplication::setOrganizationDomain( "OrgDomain" );
	QCoreApplication::setOrganizationName( "OrgName" );
	QCoreApplication::setApplicationName( "Fibernavigator 2" );
	QCoreApplication::setApplicationVersion( "1.0.0" );

    Q_INIT_RESOURCE( resources );

    QStringList args = app.arguments();

    DataStore* dataStore = new DataStore();
    for ( int i = 1; i < args.size(); ++i )
    {
        dataStore->load( QDir( args.at( i ) ) );
        //qDebug() << i << " : " << args.at( i );
    }

    MainWindow mainWin( dataStore );
    mainWin.show();


    return app.exec();
}
