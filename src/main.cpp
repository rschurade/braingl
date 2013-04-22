#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QDateTime>

#include "data/models.h"
#include "data/loader.h"
#include "data/vptr.h"
#include "gui/mainwindow.h"



QTextStream *out = 0;
bool logToFile = false;
bool verbose = false;

void logOutput( QtMsgType type, const char *msg )
{
    QString filedate = QDateTime::currentDateTime().toString( "yyyy.MM.dd hh:mm:ss:zzz" );
    QString debugdate = QDateTime::currentDateTime().toString( "hh:mm:ss:zzz" );

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
    if ( verbose )
    {
        (*out) << debugdate << " " << msg << endl;
    }
    if ( logToFile )
    {
        QFile outFile("log.txt");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << filedate << " " << msg << endl;
    }

    if (QtFatalMsg == type)
    {
        abort();
    }
}

void noOutput(QtMsgType type, const char *msg) {}


int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    QCoreApplication::setOrganizationDomain( "braingl" );
	QCoreApplication::setOrganizationName( "brainglInc" );
	QCoreApplication::setApplicationName( "braingl" );
	QCoreApplication::setApplicationVersion( "1.0.0" );

    Q_INIT_RESOURCE( resources );

    QStringList args = app.arguments();

    qInstallMsgHandler( noOutput );
    bool debug = false;
    for ( int i = 1; i < args.size(); ++i )
    {
        if ( args.at( i ) == "-v" )
        {
            verbose = true;
        }
        if ( args.at( i ) == "-d" )
        {
            debug = true;
        }
        if ( args.at( i ) == "-l" )
        {
            logToFile = true;
        }
    }
    out = new QTextStream( stdout );
    qInstallMsgHandler( logOutput );

#ifdef __DEBUG__
    debug = true;
#endif

    Models::init();

    MainWindow mainWin( debug );
    mainWin.show();

    for ( int i = 1; i < args.size(); ++i )
    {
        mainWin.load( args.at( i ) );
    }

    return app.exec();
}
