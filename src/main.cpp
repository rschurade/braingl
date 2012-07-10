#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include "data/datastore.h"
#include "gui/mainwindow.h"

#ifndef QT_NO_DEBUG_OUTPUT

QTextStream *out = 0;

void logOutput(QtMsgType type, const char *msg)
{
    QString debugdate = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss");
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
#endif

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );


#ifndef QT_NO_DEBUG_OUTPUT
//    QString fileName = QCoreApplication::applicationFilePath().replace( ".exe", ".log" );
//    QFile *log = new QFile( fileName );
//    if ( log->open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) )
//    {
//        out = new QTextStream( stdout );
//        qInstallMsgHandler( logOutput );
//    }
//    else
//    {
//        qDebug() << "Error opening log file '" << fileName << "'. All debug output redirected to console.";
//    }
#endif
    out = new QTextStream( stdout );
    qInstallMsgHandler( logOutput );


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
    }

    MainWindow mainWin( dataStore );
    mainWin.show();


    return app.exec();
}
