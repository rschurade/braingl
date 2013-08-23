/*
 * mainwindow.cpp
 *
 * Created on: May 03, 2012
 * @author Ralph Schurade
 */

#include "mainwindow.h"

#include "views/datasetinfoview.h"

#include "widgets/glwidget.h"
#include "widgets/fndockwidget.h"
#include "widgets/datasetpropertywidget.h"
#include "widgets/roipropertywidget.h"
#include "widgets/globalpropertywidget.h"
#include "widgets/docknavglwidget.h"
#include "widgets/combinednavglwidget.h"
#include "widgets/singleshwidget.h"
#include "widgets/roiwidget.h"
#include "widgets/datasetlistwidget.h"
#include "widgets/navglwidget.h"
#include "widgets/combinednavglwidget.h"
#include "widgets/toolbar.h"
#include "widgets/statusbar.h"
#include "widgets/shadereditwidget.h"
#include "widgets/colormapeditwidget.h"
#include "widgets/scriptwidget.h"
#include "widgets/algoStarterWidgets/newdatasetwidget.h"

#include "gl/camerabase.h"
#include "gl/arcball.h"
#include "gl/camera.h"
#include "gl/glfunctions.h"
#include "gl/colormapfunctions.h"

#include "../data/loader.h"
#include "../data/writer.h"
#include "../data/vptr.h"
#include "../data/enums.h"
#include "../data/models.h"
#include "../data/roi.h"
#include "../data/roibox.h"

#include <QtGui>
#include <QWebView>

int MainWindow::countMainGL = 2;

MainWindow::MainWindow( bool debug, bool resetSettings ) :
	QMainWindow(),
    m_debug( debug )
{
	m_centralWidget = new QMainWindow();
	m_centralWidget->setObjectName( "central widget" );
	m_centralWidget->setDockOptions( QMainWindow::AnimatedDocks |  QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks );
	m_centralWidget->setDocumentMode( true );
	setCentralWidget( m_centralWidget );

	loadColormaps( resetSettings );

    createActions();
    createMenus();
    createToolBars();
    createDockWindows();


    // this needs to be done after the view is created
    m_toolsToolBar->setSelectionModel( m_datasetWidget->selectionModel() );

    createStatusBar();

    setWindowTitle( tr( "brainGL" ) );

    QIcon logo( ":/logo_blue_orange_64.png" );
    setWindowIcon( logo );

    setUnifiedTitleAndToolBarOnMac( true );

    if ( !resetSettings )
    {
        loadSettings();
    }
}

void MainWindow::closeEvent( QCloseEvent *event )
{
    saveSettings();
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue( "mainWindowGeometry", saveGeometry() );
    settings.setValue( "mainWindowState", saveState() );

    settings.setValue( "centralWidgetGeometry", m_centralWidget->saveGeometry() );
    settings.setValue( "centralWidgetState", m_centralWidget->saveState() );

    settings.setValue( "lockDockTitles", lockDockTitlesAct->isChecked() );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_LAST_PATH ), Models::g()->data( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ) ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_PATH ), Models::g()->data( Models::g()->index( (int)Fn::Property::G_SCREENSHOT_PATH, 0 ) ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_BACKGROUND_COLOR_MAIN ), Models::g()->data( Models::g()->index( (int)Fn::Property::G_BACKGROUND_COLOR_MAIN, 0 ) ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_BACKGROUND_COLOR_COMBINED ), Models::g()->data( Models::g()->index( (int)Fn::Property::G_BACKGROUND_COLOR_COMBINED, 0 ) ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_BACKGROUND_COLOR_NAV1 ), Models::g()->data( Models::g()->index( (int)Fn::Property::G_BACKGROUND_COLOR_NAV1, 0 ) ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_BACKGROUND_COLOR_NAV2 ), Models::g()->data( Models::g()->index( (int)Fn::Property::G_BACKGROUND_COLOR_NAV2, 0 ) ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_BACKGROUND_COLOR_NAV3 ), Models::g()->data( Models::g()->index( (int)Fn::Property::G_BACKGROUND_COLOR_NAV3, 0 ) ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_CROSSHAIR_COLOR ), Models::g()->data( Models::g()->index( (int)Fn::Property::G_CROSSHAIR_COLOR, 0 ) ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SHOW_NAV_SLIDERS ), Models::g()->data( Models::g()->index( (int)Fn::Property::G_SHOW_NAV_SLIDERS, 0 ) ) );

    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_WIDTH ), Models::getGlobal( Fn::Property::G_SCREENSHOT_WIDTH ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_HEIGHT ), Models::getGlobal( Fn::Property::G_SCREENSHOT_HEIGHT ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_KEEP_ASPECT ), Models::getGlobal( Fn::Property::G_SCREENSHOT_KEEP_ASPECT ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_SIZE_RATIO ), Models::getGlobal( Fn::Property::G_SCREENSHOT_SIZE_RATIO ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_SIZE_SELECTION ), Models::getGlobal( Fn::Property::G_SCREENSHOT_SIZE_SELECTION ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_COPY_CURRENT ), Models::getGlobal( Fn::Property::G_SCREENSHOT_COPY_CURRENT ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_DO_MAINGL ), Models::getGlobal( Fn::Property::G_SCREENSHOT_DO_MAINGL ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_DO_MAINGL2 ), Models::getGlobal( Fn::Property::G_SCREENSHOT_DO_MAINGL2 ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_STEREOSCOPIC ), Models::getGlobal( Fn::Property::G_SCREENSHOT_STEREOSCOPIC ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_PREFIX ), Models::getGlobal( Fn::Property::G_SCREENSHOT_PREFIX ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_PREFIX2 ), Models::getGlobal( Fn::Property::G_SCREENSHOT_PREFIX2 ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_DIGITS ), Models::getGlobal( Fn::Property::G_SCREENSHOT_DIGITS ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_CURRENT_NUMBER ), Models::getGlobal( Fn::Property::G_SCREENSHOT_CURRENT_NUMBER ) );

    QByteArray ar;
    QDataStream out( &ar, QIODevice::WriteOnly );   // write the data

    out << ColormapFunctions::size();

    for ( int i = 0; i < ColormapFunctions::size(); ++i )
    {
        ColormapBase c = ColormapFunctions::get( i );
        out << c.getName();
        out << c.size();
        for ( int k = 0; k < c.size(); ++k )
        {
            out << c.get( k ).value;
            out << c.get( k ).color;
        }
    }
    settings.setValue( "colormaps", ar );

}

void MainWindow::loadSettings()
{
    QSettings settings;
    restoreGeometry( settings.value( "mainWindowGeometry" ).toByteArray() );
    restoreState( settings.value( "mainWindowState" ).toByteArray() );

    m_centralWidget->restoreGeometry( settings.value( "centralWidgetGeometry" ).toByteArray() );
    m_centralWidget->restoreState( settings.value( "centralWidgetState" ).toByteArray() );

    if ( settings.contains( "lockDockTitles") )
    {
        if ( settings.value( "lockDockTitles" ).toBool() )
        {
            lockDockTitlesAct->activate( QAction::Trigger );
        }
    }
    loadSetting( settings, Fn::Property::G_LAST_PATH );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_PATH );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_MAIN );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_COMBINED );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_NAV1 );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_NAV2 );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_NAV3 );
    loadSetting( settings, Fn::Property::G_CROSSHAIR_COLOR );
    loadSetting( settings, Fn::Property::G_SHOW_NAV_SLIDERS );

    loadSetting( settings, Fn::Property::G_SCREENSHOT_WIDTH );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_HEIGHT );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_KEEP_ASPECT );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_SIZE_RATIO );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_SIZE_SELECTION );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_COPY_CURRENT );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_DO_MAINGL );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_DO_MAINGL2 );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_STEREOSCOPIC );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_PREFIX );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_PREFIX2 );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_DIGITS );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_CURRENT_NUMBER );
}

void MainWindow::loadSetting( QSettings &settings, Fn::Property setting )
{
    if ( settings.contains( Fn::Prop2String::s( setting) ) )
    {
        QVariant s = settings.value( Fn::Prop2String::s( setting ) );
        Models::g()->setData( Models::g()->index( (int)setting, 0 ), s );
    }
}

void MainWindow::loadColormaps( bool resetSettings )
{
    QSettings settings;
    if ( settings.contains( "colormaps" ) && !resetSettings )
    {
        qDebug() << "restore colormaps";
        QByteArray ar = settings.value( "colormaps" ).toByteArray();
        QDataStream in( &ar,QIODevice::ReadOnly );
        int countColormaps;
        in >> countColormaps;
        for ( int i = 0; i < countColormaps; ++i )
        {
            QString name;
            QVector< ColormapPair >values;
            in >> name;
            int cmapSize;
            in >> cmapSize;
            for ( int k = 0; k < cmapSize; ++k)
            {
                ColormapPair pair;
                in >> pair.value;
                in >> pair.color;
                values.push_back( pair );
            }
            ColormapBase cmap( name, values );
            ColormapFunctions::addColormap( cmap );
        }
    }
    else
    {
        qDebug() << "create colormaps";
        ColormapBase colormap( "grey", QColor( 3, 3, 3 ), QColor( 255, 255, 255 ) );
        ColormapFunctions::addColormap( colormap );

        ColormapBase cmap( "rainbow", QColor( 0, 0, 255 ), QColor( 255, 0, 0 ) );
        cmap.insertValue( 0.25, QColor( 0, 255, 255 ) );
        cmap.insertValue( 0.5, QColor( 0, 255, 0 ) );
        cmap.insertValue( 0.75, QColor( 255, 255, 0 ) );
        ColormapFunctions::addColormap( cmap );

        ColormapBase cmap2( "rainbow 2", QColor( 0, 0, 255 ), QColor( 255, 0, 255 ) );
        cmap2.insertValue( 0.2, QColor( 0, 255, 255 ) );
        cmap2.insertValue( 0.4, QColor( 0, 255, 0 ) );
        cmap2.insertValue( 0.6, QColor( 255, 255, 0 ) );
        cmap2.insertValue( 0.8, QColor( 255, 0, 0 ) );
        ColormapFunctions::addColormap( cmap2 );

        ColormapBase cmap3( "blue white red", QColor( 0, 0, 255 ), QColor( 255, 0, 0 ) );
        cmap3.insertValue( 0.5, QColor( 255, 255, 255 ) );
        ColormapFunctions::addColormap( cmap3 );
    }
}

void MainWindow::open()
{
    QString fn = Models::g()->data( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ) ).toString();
    QStringList fileNames = QFileDialog::getOpenFileNames( this, "Open File", fn );
    for ( int i = 0; i < fileNames.size(); ++i )
    {
        load( fileNames[i] );
    }

}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast< QAction * >( sender() );
    if ( action )
    {
        load( action->data().toString() );
    }
}

void MainWindow::load( QString fileName )
{
    if ( !fileName.isEmpty() )
    {
        if ( fileName.endsWith( "scn" ) )
        {
            loadScene( fileName );
        }
        else if( fileName.endsWith( "bgscript" ) )
        {
            m_scriptWidget->loadScript( fileName, true );
        }
        else
        {
            Loader loader;
            loader.setFilename( QDir( fileName ) );
            if ( loader.load() )
            {
                for ( int k = 0; k < loader.getNumDatasets(); ++k )
                {
                    Models::d()->setData( Models::d()->index( Models::d()->rowCount(), (int)Fn::Property::D_NEW_DATASET ), VPtr<Dataset>::asQVariant( loader.getDataset( k ) ), Qt::DisplayRole );
                }
                QFileInfo fi( fileName );
                QDir dir = fi.absoluteDir();
                QString lastPath = dir.absolutePath();
                Models::g()->setData( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ), lastPath );

                setCurrentFile(fileName);

                GLFunctions::reloadShaders();
            }
        }
    }
}

void MainWindow::setCurrentFile( const QString &fileName )
{
    curFile = fileName;
    setWindowFilePath( curFile );

    QSettings settings;
    QStringList files = settings.value( "recentFileList" ).toStringList();
    files.removeAll( fileName );
    files.prepend( fileName );
    while ( files.size() > MaxRecentFiles )
        files.removeLast();

    settings.setValue( "recentFileList", files );

    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        MainWindow *mainWin = qobject_cast< MainWindow * >( widget );
        if ( mainWin )
            mainWin->updateRecentFileActions();
    }
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value( "recentFileList" ).toStringList();

    int numRecentFiles = qMin( files.size(), (int) MaxRecentFiles );

    for ( int i = 0; i < numRecentFiles; ++i )
    {
        QString text = tr( "&%1 %2" ).arg( i + 1 ).arg( strippedName( files[ i ] ) );
        recentFileActs[ i ]->setText( text );
        recentFileActs[ i ]->setData( files[ i ] );
        recentFileActs[ i ]->setVisible( true );
    }
    for ( int j = numRecentFiles; j < MaxRecentFiles; ++j )
        recentFileActs[ j ]->setVisible( false );

    separatorAct->setVisible( numRecentFiles > 0 );
}

QString MainWindow::strippedName( const QString &fullFileName )
{
    return QFileInfo( fullFileName ).fileName();
}

void MainWindow::save()
{
    QString fn = Models::g()->data( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ) ).toString();
    QString fileName = QFileDialog::getSaveFileName( this, "Save File", fn );

    if ( !fileName.isEmpty() )
    {
        QDir dir;
        if ( dir.exists( fileName ) )
        {
            QMessageBox msgBox;
            msgBox.setText("File already exists.");
            msgBox.setInformativeText("Do you want to overwrite it?");
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            switch ( ret )
            {
                case QMessageBox::Save :
                {
                    Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( m_datasetWidget->getSelected(), (int)Fn::Property::D_DATASET_POINTER) ) );
                    Writer writer( ds, fileName );
                    writer.save();
                    break;
                }
                case QMessageBox::Cancel :
                    break;
            }
        }
        else
        {
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( m_datasetWidget->getSelected(), (int)Fn::Property::D_DATASET_POINTER) ) );
            Writer writer( ds, fileName );
            writer.save();
        }

        QFileInfo fi( fileName );
        dir = fi.absoluteDir();
        QString lastPath = dir.absolutePath();
        Models::g()->setData( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ), lastPath );
    }
}
//TODO
void MainWindow::saveScene()
{
    QString fn = Models::g()->data( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ) ).toString();
    QString fileName = QFileDialog::getSaveFileName( this, "Save File", fn );
    saveScene( fileName );
}

void MainWindow::saveScene( QString fileName )
{
    QSettings settings( fileName, QSettings::IniFormat );
    //qDebug() << settings.status();

    settings.setValue( "appName", "braingl" );
    settings.setValue( "version", "0.7.0" );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  save global settings
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SAGITTAL ), Models::getGlobal( Fn::Property::G_SAGITTAL ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_CORONAL ), Models::getGlobal( Fn::Property::G_CORONAL ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_AXIAL ), Models::getGlobal( Fn::Property::G_AXIAL ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SHOW_AXIAL ), Models::getGlobal( Fn::Property::G_SHOW_AXIAL ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SHOW_CORONAL ), Models::getGlobal( Fn::Property::G_SHOW_CORONAL ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SHOW_SAGITTAL ), Models::getGlobal( Fn::Property::G_SHOW_SAGITTAL ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_BACKGROUND_COLOR_MAIN ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_MAIN ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_BACKGROUND_COLOR_COMBINED ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_COMBINED ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_BACKGROUND_COLOR_NAV1 ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_NAV1 ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_BACKGROUND_COLOR_NAV2 ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_NAV2 ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_BACKGROUND_COLOR_NAV3 ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_NAV3 ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_RENDER_CROSSHAIRS ), Models::getGlobal( Fn::Property::G_RENDER_CROSSHAIRS ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_CROSSHAIR_COLOR ), Models::getGlobal( Fn::Property::G_CROSSHAIR_COLOR ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SHOW_NAV_SLIDERS ), Models::getGlobal( Fn::Property::G_SHOW_NAV_SLIDERS ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_SCREENSHOT_QUALITY ), Models::getGlobal( Fn::Property::G_SCREENSHOT_QUALITY ) );
    settings.setValue( Fn::Prop2String::s( Fn::Property::G_TRANSPARENCY ), Models::getGlobal( Fn::Property::G_TRANSPARENCY ) );

    settings.setValue( "camera_maingl", mainGLWidget->getCamera()->getState() );
    settings.setValue( "arcball_maingl", mainGLWidget->getArcBall()->getState() );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  save loaded datasets
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    int countDatasets = Models::d()->rowCount();
    settings.setValue( "countDatasets", countDatasets );

    QList<QVariant> fileNames;

    for ( int i = 0; i < countDatasets; ++i )
    {
        Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int) Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
        QVariant fn = ds->properties()->get( Fn::Property::D_FILENAME );
        fileNames.push_back( fn );
        QList<QVariant>state = ds->properties()->getState();
        settings.setValue( "file_" + QString::number( i ) + "_state", state );
    }
    settings.setValue( "fileNames", fileNames );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  save loaded rois
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    int numBranches = Models::r()->rowCount( QModelIndex() );
    QList<QVariant> roiStates;

    for ( int i = 0; i < numBranches; ++i )
    {
        int leafCount = Models::r()->rowCount( createIndex( i, 0, 0 ) );

        QList<QVariant>branch;
        for ( int k = 0; k < leafCount+1; ++k )
        {
            ROI* roi = VPtr<ROI>::asPtr( Models::r()->data( createIndex( i, k, (int)Fn::Property::R_POINTER ), Qt::DisplayRole ) );

            QList<QVariant>state = roi->properties()->getState();
            if ( roi->properties()->get( Fn::Property::R_SHAPE).toInt() != 10 )
            {
                branch.push_back( state );
            }

            //settings.setValue( "roi" + QString::number( i ) + "_" + QString::number( k ) + "state", state );
        }
        if ( branch.size() > 0 )
        {
            roiStates.push_back( branch );
        }
    }
    settings.setValue(  "roiStates", roiStates );

    settings.sync();
}

QModelIndex MainWindow::createIndex( int branch, int pos, int column )
{
    int row;
    QModelIndex parent;
    if ( pos == 0 )
    {
        row = branch;
    }
    else
    {
        row = pos - 1;
        parent = Models::r()->index( branch, 0 );
    }
    return Models::r()->index( row, column, parent );
}

void MainWindow::loadScene( QString fileName )
{
    QSettings settings( fileName, QSettings::IniFormat );
    //qDebug() << settings.status();

    QList<QVariant> files = settings.value( "fileNames" ).toList();

    for ( int i = 0; i < files.size(); ++i )
    {
        load( files[i].toString() );
    }

    int countDatasets = Models::d()->rowCount();

    for ( int i = 0; i < countDatasets; ++i )
    {
        Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int) Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
        QVariant fn = ds->properties()->get( Fn::Property::D_FILENAME );

        if ( settings.contains( "file_" + QString::number( i ) + "_state" ) )
        {
            ds->properties()->setState( settings.value( "file_" + QString::number( i ) + "_state" ).toList() );
        }
    }

    GLFunctions::reloadShaders();

    int numBranches = Models::r()->rowCount( QModelIndex() );

    QList<QVariant> rois = settings.value( "roiStates" ).toList();
    for ( int i = 0; i < rois.size(); ++i )
    {
        QList<QVariant>branch = rois[i].toList();

        QList<QVariant>roiState = branch[0].toList();
        GLFunctions::roi = new ROIBox();
        GLFunctions::roi->properties()->setState( roiState );

        Models::r()->insertRows( 0, 0, QModelIndex() );

        if ( branch.size() > 1 )
        {
            for ( int k = 1; k < branch.size(); ++k )
            {
                QList<QVariant>roiState = branch[k].toList();
                GLFunctions::roi = new ROIBox();
                GLFunctions::roi->properties()->setState( roiState );
                Models::r()->insertRows( 0, 0, createIndex( numBranches +i, 0, 0 ) );

                for ( int l = 0; l < roiState.size() / 2; ++l )
                {
                    if ( (Fn::Property)( roiState[l*2].toInt() ) == Fn::Property::R_COLOR )
                    {
                        GLFunctions::roi->properties()->set( (Fn::Property)( roiState[l*2].toInt() ), roiState[l*2+1] );
                    }
                }
            }
        }
    }



    loadSetting( settings, Fn::Property::G_SAGITTAL );
    loadSetting( settings, Fn::Property::G_CORONAL );
    loadSetting( settings, Fn::Property::G_AXIAL );
    loadSetting( settings, Fn::Property::G_SHOW_AXIAL );
    loadSetting( settings, Fn::Property::G_SHOW_CORONAL );
    loadSetting( settings, Fn::Property::G_SHOW_SAGITTAL );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_MAIN );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_COMBINED );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_NAV1 );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_NAV2 );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_NAV3 );
    loadSetting( settings, Fn::Property::G_RENDER_CROSSHAIRS );
    loadSetting( settings, Fn::Property::G_CROSSHAIR_COLOR );
    loadSetting( settings, Fn::Property::G_SHOW_NAV_SLIDERS );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_QUALITY );
    loadSetting( settings, Fn::Property::G_TRANSPARENCY );

    if ( settings.contains( "camera_maingl" ) )
    {
        mainGLWidget->getCamera()->setState( settings.value( "camera_maingl" ).toList() );
    }
    if ( settings.contains( "arcball_maingl" ) )
    {
        mainGLWidget->getArcBall()->setState( settings.value( "arcball_maingl" ).toList() );
    }


    Models::g()->submit();

}


void MainWindow::about()
{
    QMessageBox::about( this, tr( "About brainGL" ),
            tr( "<b>brainGL</b> exploring and visualizing anatomical and functional connectivity in the brain." ) );
}

void MainWindow::createActions()
{
    newAct = new QAction( QIcon( ":/icons/new.png" ), tr( "&New..." ), this );
    newAct->setShortcuts( QKeySequence::New );
    newAct->setStatusTip( tr( "New Dataset" ) );
    connect( newAct, SIGNAL( triggered() ), this, SLOT( slotNew() ) );

    openAct = new QAction( QIcon( ":/icons/open.png" ), tr( "&Open..." ), this );
    openAct->setShortcuts( QKeySequence::Open );
    openAct->setStatusTip( tr( "Load Dataset" ) );
    connect( openAct, SIGNAL(triggered()), this, SLOT(open()) );

    saveAct = new QAction( QIcon( ":/icons/save.png" ), tr( "&Save..." ), this );
    saveAct->setShortcuts( QKeySequence::Save );
    saveAct->setStatusTip( tr( "Save the current form letter" ) );
    connect( saveAct, SIGNAL(triggered()), this, SLOT(save()) );

    quitAct = new QAction( tr( "&Quit" ), this );
    quitAct->setShortcuts( QKeySequence::Quit );
    quitAct->setStatusTip( tr( "Quit the application" ) );
    connect( quitAct, SIGNAL(triggered()), this, SLOT(close()) );

    saveSceneAct = new QAction( tr( "Save Scene" ), this );
    saveSceneAct->setStatusTip( tr( "Save the current scene" ) );
    connect( saveSceneAct, SIGNAL(triggered()), this, SLOT(saveScene()) );

    screenshotAct = new QAction( tr( "Screenshot" ), this );
    screenshotAct->setStatusTip( tr( "Sreenshot" ) );
    connect( screenshotAct, SIGNAL(triggered()), this, SLOT(screenshot()) );

    resetSettingsAct = new QAction( tr( "Reset Settings" ), this );
    resetSettingsAct->setStatusTip( tr( "Reset Settings" ) );
    connect( resetSettingsAct, SIGNAL(triggered()), this, SLOT(resetSettings()) );


    aboutAct = new QAction( tr( "&About" ), this );
    aboutAct->setStatusTip( tr( "Show the application's About box" ) );
    connect( aboutAct, SIGNAL(triggered()), this, SLOT(about()) );


    newMainGLAct = new QAction( tr( "New 3D view" ), this );
    newMainGLAct->setStatusTip( tr( "New 3D view" ) );
    connect( newMainGLAct, SIGNAL(triggered()), this, SLOT( newMainGL() ) );

    showAxialAct = new QAction( QIcon( ":/icons/axial.png" ), tr( "Show Axial Slice" ), this );
    showAxialAct->setStatusTip( tr( "Toggle rendering of the axial slice." ) );
    showAxialAct->setCheckable( true );
    showAxialAct->setChecked( true );
    connect( showAxialAct, SIGNAL( toggled( bool ) ), this, SLOT( slotToggleAxialSlice() ) );

    showCoronalAct = new QAction( QIcon( ":/icons/coronal.png" ), tr( "Show Coronal Slice" ), this );
    showCoronalAct->setStatusTip( tr( "Toggle rendering of the coronal slice." ) );
    showCoronalAct->setCheckable( true );
    showCoronalAct->setChecked( true );
    connect( showCoronalAct, SIGNAL( toggled( bool ) ), this, SLOT( slotToggleCoronalSlice() ) );

    showSagittalAct = new QAction( QIcon( ":/icons/sagittal.png" ), tr( "Show Sagittal Slice" ), this );
    showSagittalAct->setStatusTip( tr( "Toggle rendering of the sagittal slice." ) );
    showSagittalAct->setCheckable( true );
    showSagittalAct->setChecked( true );
    connect( showSagittalAct, SIGNAL( toggled( bool ) ), this, SLOT( slotToggleSagittalSlice() ) );

    standardViewAxialAct = new QAction( QIcon( ":/icons/axial.png" ), tr( "Axial View" ), this );
    standardViewAxialAct->setStatusTip( tr( "Toggle rendering of the axial slice." ) );
    connect( standardViewAxialAct, SIGNAL( triggered() ), this, SLOT( slotStandardAxialView() ) );

    standardViewCoronalAct = new QAction( QIcon( ":/icons/coronal.png" ), tr( "Coronal View" ), this );
    standardViewCoronalAct->setStatusTip( tr( "Toggle rendering of the coronal slice." ) );
    connect( standardViewCoronalAct, SIGNAL( triggered() ), this, SLOT( slotStandardCoronalView() ) );

    standardViewSagittalAct = new QAction( QIcon( ":/icons/sagittal.png" ), tr( "Sagittal view" ), this );
    standardViewSagittalAct->setStatusTip( tr( "Toggle rendering of the sagittal slice." ) );
    connect( standardViewSagittalAct, SIGNAL( triggered() ), this, SLOT( slotStandardSagittalView() ) );

    toggleShaderEditAct = new QAction( QIcon( ":/icons/cat.png" ), tr( "Toggle shader edit" ), this );
    toggleShaderEditAct->setStatusTip( tr( "Toggle the shader edit widget." ) );
    connect( toggleShaderEditAct, SIGNAL( triggered() ), this, SLOT( slotToggleShaderEdit() ) );

    lockDockTitlesAct = new QAction( tr( "Lock Widgets" ), this );
    lockDockTitlesAct->setStatusTip( tr( "Lock all dock widgets in place" ) );
    lockDockTitlesAct->setCheckable( true );
    lockDockTitlesAct->setChecked( false );

    renderCrosshairsAct = new QAction( tr( "Render Crosshairs" ), this );
    renderCrosshairsAct->setStatusTip( tr( "render crosshairs in navigation widgets." ) );
    renderCrosshairsAct->setCheckable( true );
    renderCrosshairsAct->setChecked( true );
    connect( renderCrosshairsAct, SIGNAL( toggled( bool ) ), this, SLOT( slotRenderCrosshairs( bool ) ) );

    newSelectionBoxAct = new QAction( QIcon( ":/icons/box.png" ), tr( "New ROI" ), this );
    newSelectionBoxAct->setStatusTip( tr( "Add a new ROI." ) );
#ifndef __WINDOWS__
    dilbertAct = new QAction( QIcon( ":/icons/box.png" ), tr( "Dilbert" ), this );
    dilbertAct->setStatusTip( tr( "Dilbert" ) );
    connect( dilbertAct, SIGNAL( triggered() ), this, SLOT( slotDilbert() ) );
#endif
    for ( int i = 0; i < MaxRecentFiles; ++i )
    {
        recentFileActs[ i ] = new QAction( this );
        recentFileActs[ i ]->setVisible( false );
        connect( recentFileActs[ i ], SIGNAL(triggered()), this, SLOT(openRecentFile()) );
    }

    continousRenderingAct = new QAction( QIcon( ":/icons/continous.png" ), tr( "Continous Rendering" ), this );
    continousRenderingAct->setStatusTip( tr( "continous rendering, warning this might slow down your computer" ) );
    continousRenderingAct->setCheckable( true );
    continousRenderingAct->setChecked( false );
    connect( continousRenderingAct, SIGNAL( toggled( bool ) ), this, SLOT( continousRendering() ) );

}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu( tr( "&File" ) );
    fileMenu->addAction( openAct );
    fileMenu->addAction( saveAct );
    separatorAct = fileMenu->addSeparator();
    fileMenu->addAction( saveSceneAct );
    //fileMenu->addAction( printAct );
    separatorAct = fileMenu->addSeparator();

    for ( int i = 0; i < MaxRecentFiles; ++i )
    {
        fileMenu->addAction( recentFileActs[ i ] );
    }
    fileMenu->addSeparator();
    fileMenu->addAction( quitAct );
    updateRecentFileActions();

    optionMenu = menuBar()->addMenu( tr( "&Options" ) );
    optionMenu->addAction( lockDockTitlesAct );
    optionMenu->addAction( renderCrosshairsAct );

    viewMenu = menuBar()->addMenu( tr( "&View" ) );
//    viewMenu->addAction( newMainGLAct );
//    viewMenu->addSeparator();

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu( tr( "&Help" ) );
    helpMenu->addAction( screenshotAct );
    helpMenu->addAction( resetSettingsAct );
#ifndef __WINDOWS__
    helpMenu->addAction( dilbertAct );
#endif
    helpMenu->addAction( aboutAct );
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar( tr( "File" ) );
    fileToolBar->addAction( newAct );
    fileToolBar->addAction( openAct );
    fileToolBar->addAction( saveAct );
    fileToolBar->addAction( continousRenderingAct );
    //fileToolBar->addAction( printAct );
    if ( m_debug )
    {
        fileToolBar->addAction( toggleShaderEditAct );
    }
    fileToolBar->setObjectName( "fileToolbar");

    editToolBar = addToolBar( tr( "Edit" ) );
    editToolBar->addAction( showAxialAct );
    editToolBar->addAction( showCoronalAct );
    editToolBar->addAction( showSagittalAct );
    editToolBar->addAction( standardViewAxialAct );
    editToolBar->addAction( standardViewCoronalAct );
    editToolBar->addAction( standardViewSagittalAct );
    editToolBar->addAction( newSelectionBoxAct );
    editToolBar->setObjectName( "editToolbar");

    m_toolsToolBar = new ToolBar( tr( "Tools" ), this );
    addToolBar( m_toolsToolBar );
}

void MainWindow::createStatusBar()
{
    StatusBar* sbar = new StatusBar( this );
    sbar->setSelectionModel( m_datasetWidget->selectionModel() );
    this->setStatusBar( sbar );
}

void MainWindow::createDockWindows()
{
    m_datasetWidget = new DatasetListWidget();
	FNDockWidget* dockDSW = new FNDockWidget( QString("Dataset List"), m_datasetWidget, this );
	addDockWidget( Qt::LeftDockWidgetArea, dockDSW );
	viewMenu->addAction( dockDSW->toggleViewAction() );
	connect( lockDockTitlesAct, SIGNAL( triggered() ), dockDSW, SLOT( toggleTitleWidget() ) );

	connect( m_datasetWidget, SIGNAL( moveSelectedItemUp( int ) ), Models::d(), SLOT( moveItemUp( int ) ) );
	connect( m_datasetWidget, SIGNAL( moveSelectedItemDown( int ) ), Models::d(), SLOT( moveItemDown( int ) ) );
	connect( m_datasetWidget, SIGNAL( deleteSelectedItem( int ) ), Models::d(), SLOT( deleteItem( int ) ) );

	ColormapEditWidget* colormapEditWidget = new ColormapEditWidget( this );
    FNDockWidget* dockCE = new FNDockWidget( QString("colormap edit"), colormapEditWidget, this );
    addDockWidget( Qt::LeftDockWidgetArea, dockCE );
    viewMenu->addAction( dockCE->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockCE, SLOT( toggleTitleWidget() ) );
    dockCE->hide();

	m_roiWidget = new ROIWidget( this );
	FNDockWidget* dockSBW = new FNDockWidget( QString("ROIs"), m_roiWidget, this );
    addDockWidget( Qt::RightDockWidgetArea, dockSBW );
    viewMenu->addAction( dockSBW->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockSBW, SLOT( toggleTitleWidget() ) );
    connect( newSelectionBoxAct, SIGNAL( triggered() ), m_roiWidget, SLOT( addBox() ) );

	DatasetPropertyWidget* dsProperties = new DatasetPropertyWidget( "maingl", this );
	FNDockWidget* dockDSP = new FNDockWidget( QString("dataset properties"), dsProperties, this );
    addDockWidget( Qt::LeftDockWidgetArea, dockDSP );
    dsProperties->setSelectionModel( m_datasetWidget->selectionModel() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockDSP, SLOT( toggleTitleWidget() ) );
    connect( colormapEditWidget, SIGNAL( signalUpdate() ), dsProperties, SLOT( update() ) );

    DatasetPropertyWidget* dsProperties2 = new DatasetPropertyWidget( "maingl2", this );
    FNDockWidget* dockDSP2 = new FNDockWidget( QString("dataset properties 2"), dsProperties2, this );
    addDockWidget( Qt::LeftDockWidgetArea, dockDSP2 );
    dsProperties2->setSelectionModel( m_datasetWidget->selectionModel() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockDSP2, SLOT( toggleTitleWidget() ) );
    connect( colormapEditWidget, SIGNAL( signalUpdate() ), dsProperties2, SLOT( update() ) );
    dockDSP2->hide();

    ROIPropertyWidget* roiProperties = new ROIPropertyWidget( this );
    FNDockWidget* dockRP = new FNDockWidget( QString("roi properties"), roiProperties, this );
    addDockWidget( Qt::RightDockWidgetArea, dockRP );
    roiProperties->setModel( Models::r() );
    roiProperties->setSelectionModel( m_roiWidget->selectionModel() );
    viewMenu->addAction( dockRP->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockRP, SLOT( toggleTitleWidget() ) );


    GlobalPropertyWidget* globalProperties = new GlobalPropertyWidget( this );
    FNDockWidget* dockGP = new FNDockWidget( QString("Global Properties"), globalProperties, this );
    addDockWidget( Qt::LeftDockWidgetArea, dockGP );
    globalProperties->setModel( Models::g() );
    viewMenu->addAction( dockGP->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockGP, SLOT( toggleTitleWidget() ) );

	DatasetInfoView *dsInfo = new DatasetInfoView( this );
	FNDockWidget* dockDSI = new FNDockWidget( QString("dataset info"), dsInfo, this );
	addDockWidget( Qt::BottomDockWidgetArea, dockDSI );
	dsInfo->setModel( Models::d() );
    dsInfo->setSelectionModel( m_datasetWidget->selectionModel() );
    viewMenu->addAction( dockDSI->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockDSI, SLOT( toggleTitleWidget() ) );
    dockDSI->hide();


    tabifyDockWidget( dockGP, dockDSP );
    tabifyDockWidget( dockGP, dockCE );
    tabifyDockWidget( dockGP, dockDSP2 );

    // GL Widgets

    mainGLWidget = new GLWidget( "maingl", m_roiWidget->selectionModel() );
    FNDockWidget* dockMainGL = new FNDockWidget( QString("main gl"), mainGLWidget, this );
    m_centralWidget->addDockWidget( Qt::LeftDockWidgetArea, dockMainGL );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockMainGL, SLOT( toggleTitleWidget() ) );
    connect( dockMainGL, SIGNAL( visibilityChanged( bool ) ), mainGLWidget, SLOT( visibilityChanged( bool ) ) );
    mainGLWidget->update();

    m_scriptWidget = new ScriptWidget( mainGLWidget, this );
    FNDockWidget* dockSW = new FNDockWidget( "script", m_scriptWidget, this );
    addDockWidget( Qt::LeftDockWidgetArea, dockSW );
    viewMenu->addAction( dockSW->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockSW, SLOT( toggleTitleWidget() ) );
    connect( m_scriptWidget, SIGNAL( screenshot() ), this, SLOT( screenshot() ) );
    //connect( mainGLWidget, SIGNAL( signalCameraChanged() ), m_scriptWidget, SLOT( slotCameraChanged() ) );
    connect( mainGLWidget, SIGNAL( signalCopyCameraToScript( int ) ), m_scriptWidget, SLOT( slotCopyCamera( int ) ) );
    dockSW->hide();
    tabifyDockWidget( dockGP, dockSW );

    mainGLWidget2 = new GLWidget( "maingl2", m_roiWidget->selectionModel(), this, mainGLWidget );
    FNDockWidget* dockMainGL2 = new FNDockWidget( QString("main gl 2"), mainGLWidget2, this );
    m_centralWidget->addDockWidget( Qt::LeftDockWidgetArea, dockMainGL2 );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockMainGL2, SLOT( toggleTitleWidget() ) );
    connect( dockMainGL2, SIGNAL( visibilityChanged( bool ) ), mainGLWidget2, SLOT( visibilityChanged( bool ) ) );

    DockNavGLWidget* nav1 = new DockNavGLWidget( QString("axial"), 2, this, mainGLWidget );
    FNDockWidget* dockNav1 = new FNDockWidget( QString("axial"), nav1, this );
    m_centralWidget->addDockWidget( Qt::RightDockWidgetArea, dockNav1 );
    viewMenu->addAction( dockNav1->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockNav1, SLOT( toggleTitleWidget() ) );
    connect( dockNav1, SIGNAL( visibilityChanged( bool ) ), nav1, SLOT( setWidgetVisible( bool) ) );

    DockNavGLWidget* nav2 = new DockNavGLWidget( QString( "sagittal" ), 0, this, mainGLWidget );
    FNDockWidget* dockNav2 = new FNDockWidget( QString("sagittal"), nav2, this );
    m_centralWidget->addDockWidget( Qt::RightDockWidgetArea, dockNav2 );
    viewMenu->addAction( dockNav2->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockNav2, SLOT( toggleTitleWidget() ) );
    connect( dockNav2, SIGNAL( visibilityChanged( bool ) ), nav2, SLOT( setWidgetVisible( bool) ) );

    DockNavGLWidget* nav3 = new DockNavGLWidget( QString( "coronal" ), 1, this, mainGLWidget );
    FNDockWidget* dockNav3 = new FNDockWidget( QString("coronal"), nav3, this );
    m_centralWidget->addDockWidget( Qt::RightDockWidgetArea, dockNav3 );
    viewMenu->addAction( dockNav3->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockNav3, SLOT( toggleTitleWidget() ) );
    connect( dockNav3, SIGNAL( visibilityChanged( bool ) ), nav3, SLOT( setWidgetVisible( bool ) ) );

    CombinedNavGLWidget* nav4 = new CombinedNavGLWidget( QString( "combined" ), this, mainGLWidget );
    FNDockWidget* dockNav4 = new FNDockWidget( QString("Combined Nav"), nav4, this );
    m_centralWidget->addDockWidget( Qt::LeftDockWidgetArea, dockNav4 );
    viewMenu->addAction( dockNav4->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockNav4, SLOT( toggleTitleWidget() ) );
    connect( dockNav4, SIGNAL( visibilityChanged( bool ) ), nav4, SLOT( setWidgetVisible( bool ) ) );

    dockNav1->hide();
    dockNav2->hide();
    dockNav3->hide();
    dockNav4->hide();
    dockMainGL2->hide();

    SingleSHWidget* sshw = new SingleSHWidget( QString( "single sh" ), this, mainGLWidget );
    FNDockWidget* dockSSHW = new FNDockWidget( QString("single sh" ), sshw, this );
    m_centralWidget->addDockWidget( Qt::LeftDockWidgetArea, dockSSHW );
    viewMenu->addAction( dockSSHW->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockSSHW, SLOT( toggleTitleWidget() ) );
    connect( dockSSHW, SIGNAL( visibilityChanged( bool ) ), sshw, SLOT( visibilityChanged( bool ) ) );
    dockSSHW->hide();

    viewMenu->addSeparator();
    viewMenu->addAction( dockMainGL->toggleViewAction() );
    viewMenu->addAction( dockDSP->toggleViewAction() );
    viewMenu->addAction( dockMainGL2->toggleViewAction() );
    viewMenu->addAction( dockDSP2->toggleViewAction() );

    m_centralWidget->tabifyDockWidget( dockSSHW, dockNav4 );
    m_centralWidget->tabifyDockWidget( dockNav4, dockMainGL );
    m_centralWidget->tabifyDockWidget( dockMainGL2, dockMainGL );

    connect( mainGLWidget, SIGNAL( signalKeyPressed( int, Qt::KeyboardModifiers ) ), m_datasetWidget, SLOT( slotKeyPressed( int, Qt::KeyboardModifiers ) ) );
    connect( mainGLWidget, SIGNAL( signalKeyPressed( int, Qt::KeyboardModifiers ) ), m_scriptWidget, SLOT( slotKeyPressed( int, Qt::KeyboardModifiers ) ) );
}

void MainWindow::slotToggleAxialSlice()
{
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_SHOW_AXIAL, 0 ), showAxialAct->isChecked() );
    Models::g()->submit();
}

void MainWindow::slotToggleCoronalSlice()
{
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_SHOW_CORONAL, 0 ), showCoronalAct->isChecked() );
    Models::g()->submit();
}

void MainWindow::slotToggleSagittalSlice()
{
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_SHOW_SAGITTAL, 0 ), showSagittalAct->isChecked() );
    Models::g()->submit();
}

void  MainWindow::slotStandardAxialView()
{
    Qt::KeyboardModifiers mods = QApplication::queryKeyboardModifiers();
    GLWidget* target = mainGLWidget;
    if ( mods & Qt::ControlModifier )
    {
        target = mainGLWidget2;
    }

    if ( mods & Qt::ShiftModifier )
    {
        target->setView( Fn::Orient::AXIAL2 );
    }
    else
    {
        target->setView( Fn::Orient::AXIAL );
    }
}

void  MainWindow::slotStandardCoronalView()
{
    Qt::KeyboardModifiers mods = QApplication::queryKeyboardModifiers();
    GLWidget* target = mainGLWidget;
    if ( mods & Qt::ControlModifier )
    {
        target = mainGLWidget2;
    }

    if ( mods & Qt::ShiftModifier )
    {
        target->setView( Fn::Orient::CORONAL2 );
    }
    else
    {
        target->setView( Fn::Orient::CORONAL );
    }
}

void  MainWindow::slotStandardSagittalView()
{
    Qt::KeyboardModifiers mods = QApplication::queryKeyboardModifiers();
    GLWidget* target = mainGLWidget;
    if ( mods & Qt::ControlModifier )
    {
        target = mainGLWidget2;
    }

    if ( mods & Qt::ShiftModifier )
    {
        target->setView( Fn::Orient::SAGITTAL2 );
    }
    else
    {
        target->setView( Fn::Orient::SAGITTAL );
    }
}

void MainWindow::slotToggleShaderEdit()
{
    m_shaderEditWidget = new ShaderEditWidget( this );
    FNDockWidget* dockSEW = new FNDockWidget( QString("shader edit" ), m_shaderEditWidget, this );
    viewMenu->addAction( dockSEW->toggleViewAction() );
    m_centralWidget->addDockWidget( Qt::RightDockWidgetArea, dockSEW );
    tabifiedDockWidgets( dockSEW );
}

void MainWindow::slotRenderCrosshairs( bool value )
{
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_RENDER_CROSSHAIRS, 0 ), value );
}

void MainWindow::screenshot()
{

    QString path = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SCREENSHOT_PATH, 0 ) ).toString();
    if ( !path.endsWith( '/') )
    {
        path += '/';
    }
    QString numberString = Models::getGlobal( Fn::Property::G_SCREENSHOT_CURRENT_NUMBER ).toString();
    Models::setGlobal( Fn::Property::G_SCREENSHOT_CURRENT_NUMBER, Models::getGlobal( Fn::Property::G_SCREENSHOT_CURRENT_NUMBER ).toInt() + 1 );
    int nss = numberString.size();
    int numDigits = Models::getGlobal( Fn::Property::G_SCREENSHOT_DIGITS ).toInt();
    QString prefix = Models::getGlobal( Fn::Property::G_SCREENSHOT_PREFIX ).toString();
    QString prefix2 = Models::getGlobal( Fn::Property::G_SCREENSHOT_PREFIX2 ).toString();

    for ( int i = 0; i < numDigits - nss; ++i )
    {
        numberString = "0" + numberString;
    }

    if ( Models::getGlobal( Fn::Property::G_SCREENSHOT_DO_MAINGL ).toBool() )
    {
        mainGLWidget->screenshot( path + prefix + numberString + QString( ".png" ) );
    }
    if ( Models::getGlobal( Fn::Property::G_SCREENSHOT_DO_MAINGL2 ).toBool() )
    {
        mainGLWidget2->screenshot( path + prefix2 + numberString + QString( ".png" ) );
    }
}

void MainWindow::resetSettings()
{
    QMessageBox msgBox;
    msgBox.setText("Reset settings.");
    msgBox.setInformativeText("This will shut down brainGL. Continue?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch ( ret )
    {
        case QMessageBox::Yes :
        {
            QSettings settings;
            settings.clear();
            settings.sync();
            exit( 0 );
            break;
        }
        case QMessageBox::Cancel :
            break;
    }
}

void MainWindow::slotDilbert()
{
#ifndef __WINDOWS__
    QWidget* widget = new QWidget();
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setContentsMargins( 1, 1, 1, 1 );
    vLayout->setSpacing( 1 );
    QWebView* wv = new QWebView( this );
    wv->setUrl( QUrl( "http://www.dilbert.com/fast/") );
    vLayout->addWidget( wv );
    widget->setLayout( vLayout );
    widget->show();
#endif
}

void MainWindow::slotNew()
{
    NewDatasetWidget* ndw = new NewDatasetWidget( m_roiWidget, this );
    ndw->show();
}

void MainWindow::newMainGL()
{
    MainWindow::countMainGL++;
    GLWidget* glw = new GLWidget( "maingl" + QString::number( MainWindow::countMainGL ), m_roiWidget->selectionModel(), this, mainGLWidget );
    FNDockWidget* dockGLW = new FNDockWidget( "main gl " + QString::number( MainWindow::countMainGL ), glw, this );
    m_centralWidget->addDockWidget( Qt::LeftDockWidgetArea, dockGLW );
    viewMenu->addAction( dockGLW->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockGLW, SLOT( toggleTitleWidget() ) );
    connect( dockGLW, SIGNAL( visibilityChanged( bool ) ), glw, SLOT( visibilityChanged( bool ) ) );

    int countDatasets = Models::d()->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int) Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
        ds->copySettings( "maingl" + QString::number( MainWindow::countMainGL ) );
    }


    DatasetPropertyWidget* dsProperties2 = new DatasetPropertyWidget( "maingl" + QString::number( MainWindow::countMainGL ), this );
    FNDockWidget* dockDSP2 = new FNDockWidget( QString("dataset properties " + QString::number( MainWindow::countMainGL ) ), dsProperties2, this );
    addDockWidget( Qt::LeftDockWidgetArea, dockDSP2 );
    dsProperties2->setSelectionModel( m_datasetWidget->selectionModel() );
    viewMenu->addAction( dockDSP2->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockDSP2, SLOT( toggleTitleWidget() ) );
    dockDSP2->hide();
}

void MainWindow::continousRendering()
{
    if ( continousRenderingAct->isChecked() )
    {
        QTimer::singleShot( 40, this, SLOT( continousRendering() ) );
        Models::g()->submit();
    }
}
