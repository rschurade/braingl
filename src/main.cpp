#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include "data/datastore.h"
#include "gui/mainwindow.h"



QTextStream *out = 0;

void logOutput(QtMsgType type, const char *msg)
{
    QString debugdate = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss:zzz");
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

    DataStore* dataStore = new DataStore();

    qInstallMsgHandler( noOutput );

    #ifdef QT_DEBUG_OUTPUT
    out = new QTextStream( stdout );
    qInstallMsgHandler( logOutput );
    qDebug() << "Debug version";
    #else
    for ( int i = 1; i < args.size(); ++i )
    {
        if ( args.at( i ) == "-v" )
        {
            out = new QTextStream( stdout );
            qInstallMsgHandler( logOutput );
        }
    }
    #endif

    MainWindow mainWin( dataStore );
    mainWin.show();

    for ( int i = 1; i < args.size(); ++i )
    {
        dataStore->load( QDir( args.at( i ) ) );
    }

    return app.exec();
}
