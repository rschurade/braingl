#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include "data/datastore.h"
#include "data/globalpropertymodel.h"
#include "data/selectionboxmodel.h"
#include "data/loader.h"
#include "data/vptr.h"
#include "gui/mainwindow.h"



QTextStream *out = 0;

void logOutput(QtMsgType type, const char *msg)
{
    //QString debugdate = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz");
    QString debugdate = QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
    switch (type)
    {
    case QtDebugMsg:
        debugdate += " [D]";
        break;
    case QtWarningMsg:
        debugdate += " [W]";
        break;
    case QtCriticalMsg:
        debugdate += " [C]";
        break;
    case QtFatalMsg:
        debugdate += " [F]";
        break;
    }
    (*out) << debugdate << " " << msg << endl;

    if (QtFatalMsg == type)
    {
        abort();
    }
}

void noOutput(QtMsgType type, const char *msg) {}


int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    QCoreApplication::setOrganizationDomain( "OrgDomain" );
	QCoreApplication::setOrganizationName( "OrgName" );
	QCoreApplication::setApplicationName( "Fibernavigator 2" );
	QCoreApplication::setApplicationVersion( "1.0.0" );

    Q_INIT_RESOURCE( resources );

    QStringList args = app.arguments();

    qInstallMsgHandler( noOutput );
    bool debug = false;
    for ( int i = 1; i < args.size(); ++i )
    {
        if ( args.at( i ) == "-v" )
        {
            out = new QTextStream( stdout );
            qInstallMsgHandler( logOutput );
        }
        if ( args.at( i ) == "-d" )
        {
            debug = true;
        }
    }
    out = new QTextStream( stdout );
    qInstallMsgHandler( logOutput );

#ifdef __DEBUG__
    debug = true;
#endif

    GlobalPropertyModel* globalProps = new GlobalPropertyModel();
    DataStore* dataStore = new DataStore( globalProps );
    SelectionBoxModel* selBoxModel = new SelectionBoxModel( globalProps );

    MainWindow mainWin( dataStore, globalProps, selBoxModel, debug );
    mainWin.show();

    for ( int i = 1; i < args.size(); ++i )
    {
        Loader loader;
        loader.setFilename( QDir( args.at( i ) ) );
        if ( loader.load() )
        {
            for ( int k = 0; k < loader.getNumDatasets(); ++k )
            {
                dataStore->setData( dataStore->index( dataStore->rowCount(), (int)Fn::Property::NEW_DATASET ), VPtr<Dataset>::asQVariant( loader.getDataset( k ) ) );
            }
        }
    }

    return app.exec();
}
