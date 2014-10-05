#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QDateTime>

#include <buildtime.h>

#include "data/enums.h"
#include "data/models.h"
#include "data/vptr.h"
#include "gui/mainwindow.h"

#include "io/loader.h"

QTextStream *out = 0;
bool logToFile = false;
bool verbose = false;
bool debug = false;
bool makeScreenshot = false;

void logOutput( QtMsgType type, const QMessageLogContext& context, const QString &message )
{
    QString filedate = QDateTime::currentDateTime().toString( "yyyy.MM.dd hh:mm:ss:zzz" );
    QString debugdate = QDateTime::currentDateTime().toString( "hh:mm:ss:zzz" );

    switch ( type )
    {
    case QtDebugMsg:
        debugdate += " [D]";
        if ( debug )
        {
            (*out) << debugdate << " " << message << endl;
        }
        break;
    case QtWarningMsg:
        debugdate += " [W]";
        (*out) << debugdate << " " << message << endl;
        break;
    case QtCriticalMsg:
        debugdate += " [C]";
        (*out) << debugdate << " " << message << endl;
        break;
    case QtFatalMsg:
        debugdate += " [F]";
        (*out) << debugdate << " " << message << endl;
        break;
    }

    if ( logToFile )
    {
        QFile outFile("log.txt");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << filedate << " " << message << endl;
    }

}

void noOutput( QtMsgType type, const QMessageLogContext& context, const QString &message )
{
}

int main( int argc, char *argv[] )
{
#ifdef __DEBUG__
    debug = true;
    verbose = true;
#endif

    QString hg = HGTIP;
    hg.remove( ";" );
    hg.remove( "changeset:" );
    hg.replace( " ","" );

    qDebug() << "brainGL development version" << hg << BUILDDATE << BUILDTIME;
    qDebug() << "(c) 2012, 2014 Ralph Schurade, Joachim Boettger";
    qDebug() << "Submit suggestions, feature requests, bug reports to https://code.google.com/p/braingl/";

    QApplication app( argc, argv );

    QCoreApplication::setOrganizationDomain( "braingl.de" );
	QCoreApplication::setOrganizationName( "MPI_CBS" );
	QCoreApplication::setApplicationName( "braingl" );
	QCoreApplication::setApplicationVersion( "0.9.1" );

    Q_INIT_RESOURCE( resources );

    QStringList args = app.arguments();

    bool resetSettings = false;
    bool runScript = false;

    int x = -1;
    int y = -1;
    int z = -1;

    QList<QString> filesToLoad;

    for ( int i = 1; i < args.size(); ++i )
    {
        QString arg = args.at( i );
        if ( arg.size() == 2 && arg.startsWith( '-' ) )
        {
            char c = arg[ arg.length() - 1 ].toLatin1();

            switch( c )
            {
                case 'd':
                    debug = true;
                    break;
                case 'h':
                    qDebug() << "Command line options:";
                    qDebug() << "-d : debug mode, show more messages at the console, enables shader edit widget";
                    qDebug() << "-h : displays this message";
                    qDebug() << "-l : logs debug messages to text file";
                    qDebug() << "-r : resets saved settings";
                    qDebug() << "-s : makes a screenshot and quits";
                    qDebug() << "-t : runs the loaded script";
                    qDebug() << "-v : toggles verbose mode, warning: this will spam your console with messages";
                    qDebug() << "---";
                    qDebug() << "--isosurface <isoValue> <fileName> : creates an isosurface dataset";
                    qDebug() << "--isoline <isoValue> <fileName> : creates an isoline dataset";
                    qDebug() << "---";
                    exit( 0 );
                    break;
                case 'l':
                    logToFile = true;
                    break;
                case'r':
                    // reset saved settings
                    resetSettings = true;
                    break;
                case's':
                    makeScreenshot = true;
                    break;
                case't':
                    runScript = true;
                    break;
                case 'v':
                    verbose = true;
                    break;
                case'x':
                {
                    bool ok = false;
                    if ( args.size() > i + 1 )
                    {
                        int tmp = args.at( i + 1 ).toInt( &ok );
                        if ( ok )
                        {
                            x = tmp;
                        }
                    }
                    break;
                }
                case'y':
                {
                    bool ok = false;
                    if ( args.size() > i + 1 )
                    {
                        int tmp = args.at( i + 1 ).toInt( &ok );
                        if ( ok )
                        {
                            y = tmp;
                        }
                    }
                    break;
                }

                case'z':
                {
                    bool ok = false;
                    if ( args.size() > i + 1 )
                    {
                        int tmp = args.at( i + 1 ).toInt( &ok );
                        if ( ok )
                        {
                            z = tmp;
                        }
                    }
                    break;
                }
            }
        }
        else
        {
            if ( arg == "--isosurface")
            {
                if ( args.length() > i + 1 )
                {
                    QString isoValue = args.at( ++i );
                    QString fileName = args.at( ++i );
                    QFile file( fileName );
                    if ( file.exists() )
                    {
                        filesToLoad.push_back( fileName );
                        filesToLoad.push_back( "isosurface" );
                        filesToLoad.push_back( isoValue );
                    }
                }

            }
            else if ( arg == "--isoline")
            {
                if ( args.length() > i + 1 )
                {
                    QString isoValue = args.at( ++i );
                    QString fileName = args.at( ++i );
                    QFile file( fileName );
                    if ( file.exists() )
                    {
                        filesToLoad.push_back( fileName );
                        filesToLoad.push_back( "isoline" );
                        filesToLoad.push_back( isoValue );
                    }
                }

            }
            else
            {
                QFile file( arg );
                if ( file.exists() )
                {
                    qDebug() << "load " << arg;
                    filesToLoad.push_back( arg );
                    filesToLoad.push_back( "" );
                    filesToLoad.push_back( 0 );
                }
            }
        }

    }

    qInstallMessageHandler( noOutput );
    out = new QTextStream( stdout );
    qInstallMessageHandler( logOutput );

    Models::init();

    MainWindow mainWin( debug, resetSettings );
    mainWin.show();

    for ( int i = 0; i < filesToLoad.size(); i += 3 )
    {
        mainWin.loadAndAlgo( filesToLoad.at( i ), filesToLoad.at( i + 1 ), filesToLoad.at( i + 2 ) );
    }

    if ( x != - 1 )
    {
        Models::setGlobal( Fn::Property::G_SAGITTAL, x );
    }

    if ( y != - 1 )
    {
        Models::setGlobal( Fn::Property::G_CORONAL, y );
    }

    if ( z != - 1 )
    {
        Models::setGlobal( Fn::Property::G_AXIAL, z );
    }

    if ( runScript )
    {
        mainWin.runScript();
    }

    if ( makeScreenshot )
    {
        mainWin.screenshot( true );
    }

    return app.exec();
}
