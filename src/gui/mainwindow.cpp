/*
 * mainwindow.cpp
 *
 * Created on: May 03, 2012
 * @author Ralph Schurade
 */

#include "mainwindow.h"

#include <buildtime.h>

#include "views/datasetinfoview.h"
#include "widgets/colormapeditwidget.h"
#include "widgets/combinednavglwidget.h"

#include "widgets/datasetlistwidget.h"
#include "widgets/datasetpropertywidget.h"
#include "widgets/docknavglwidget.h"
#include "widgets/fndockwidget.h"
#include "widgets/globalpropertywidget.h"
#include "widgets/glwidget.h"
#include "widgets/hierarchicaltreeglwidget.h"
#include "widgets/navglwidget.h"
#include "widgets/roipropertywidget.h"
#include "widgets/roiwidget.h"
#include "widgets/scriptwidget.h"
#include "widgets/shadereditwidget.h"
#include "widgets/singleshwidget.h"
#include "widgets/statusbar.h"
#include "widgets/toolbar.h"
#include "widgets/algoStarterWidgets/newdatasetwidget.h"

#include "gl/arcball.h"
#include "gl/camera.h"
#include "gl/camerabase.h"
#include "gl/colormapfunctions.h"
#include "gl/glfunctions.h"

#include "../data/enums.h"
#include "../data/globalpropertymodel.h"
#include "../data/models.h"
#include "../data/roi.h"
#include "../data/roiarea.h"
#include "../data/roibox.h"
#include "../data/vptr.h"

#include "../data/datasets/datasetguides.h"
#include "../data/datasets/datasetlabel.h"
#include "../data/datasets/datasetplane.h"
#include "../data/datasets/datasetscalar.h"

#include "../io/loader.h"
#include "../io/roiwriter.h"
#include "../io/statewriter.h"
#include "../io/writer.h"

#include "../io/statereader.h"

#include <QAction>
#include <QWebView>
#include <QGLFormat>
#include <QKeySequence>

#include <iostream>

int MainWindow::countMainGL = 2;

MainWindow::MainWindow( bool debug, bool resetSettings ) :
	QMainWindow(),
    m_debug( debug )
{
    setDockOptions( QMainWindow::AnimatedDocks |  QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks | QMainWindow::VerticalTabs );

	m_centralWidget = new QMainWindow();
	m_centralWidget->setObjectName( "central widget" );
	m_centralWidget->setDockOptions( QMainWindow::AnimatedDocks |  QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks );
	m_centralWidget->setDocumentMode( true );
	setCentralWidget( m_centralWidget );

	GLFunctions::m_debug = debug;

    QGLFormat fmt;
    fmt.setVersion( 3, 3 );
    fmt.setProfile( QGLFormat::CoreProfile );
    fmt.setSampleBuffers( true );
    QGLFormat::setDefaultFormat( fmt );

    loadColormaps( resetSettings );

    createActions();
    createMenus();
    createToolBars();

    createDockWindows();

    // this needs to be done after the view is created
    m_toolsToolBar->setSelectionModel( m_datasetWidget->selectionModel() );

    createStatusBar();

    setWindowTitle( tr( "brainGL" ) );

    QIcon logo( ":/icons/logo_blue_orange_64.png" );
    setWindowIcon( logo );

    setUnifiedTitleAndToolBarOnMac( false );

    if ( !resetSettings )
    {
        mainGLWidget->makeCurrent();
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
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_LAST_PATH ), Models::getGlobal( Fn::Property::G_LAST_PATH ) );

    settings.setValue( Fn::Prop2String::e( Fn::Property::G_BACKGROUND_COLOR_MAIN ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_MAIN ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_BACKGROUND_COLOR_MAIN2 ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_MAIN2 ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_BACKGROUND_COLOR_COMBINED ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_COMBINED ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_BACKGROUND_COLOR_NAV1 ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_NAV1 ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_BACKGROUND_COLOR_NAV2 ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_NAV2 ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_BACKGROUND_COLOR_NAV3 ), Models::getGlobal( Fn::Property::G_BACKGROUND_COLOR_NAV3 ) );

    settings.setValue( Fn::Prop2String::e( Fn::Property::G_CROSSHAIR_COLOR ), Models::getGlobal( Fn::Property::G_CROSSHAIR_COLOR ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SHOW_NAV_SLIDERS ), Models::getGlobal( Fn::Property::G_SHOW_NAV_SLIDERS ) );

    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_PREFIX ), Models::getGlobal( Fn::Property::G_SCREENSHOT_PREFIX ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_PREFIX2 ), Models::getGlobal( Fn::Property::G_SCREENSHOT_PREFIX2 ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_PATH ), Models::getGlobal( Fn::Property::G_SCREENSHOT_PATH ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_WIDTH ), Models::getGlobal( Fn::Property::G_SCREENSHOT_WIDTH ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_HEIGHT ), Models::getGlobal( Fn::Property::G_SCREENSHOT_HEIGHT ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_KEEP_ASPECT ), Models::getGlobal( Fn::Property::G_SCREENSHOT_KEEP_ASPECT ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_SIZE_RATIO ), Models::getGlobal( Fn::Property::G_SCREENSHOT_SIZE_RATIO ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_SIZE_SELECTION ), Models::getGlobal( Fn::Property::G_SCREENSHOT_SIZE_SELECTION ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_COPY_CURRENT ), Models::getGlobal( Fn::Property::G_SCREENSHOT_COPY_CURRENT ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_DO_MAINGL ), Models::getGlobal( Fn::Property::G_SCREENSHOT_DO_MAINGL ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_DO_MAINGL2 ), Models::getGlobal( Fn::Property::G_SCREENSHOT_DO_MAINGL2 ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_STEREOSCOPIC ), Models::getGlobal( Fn::Property::G_SCREENSHOT_STEREOSCOPIC ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_PREFIX ), Models::getGlobal( Fn::Property::G_SCREENSHOT_PREFIX ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_PREFIX2 ), Models::getGlobal( Fn::Property::G_SCREENSHOT_PREFIX2 ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_DIGITS ), Models::getGlobal( Fn::Property::G_SCREENSHOT_DIGITS ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SCREENSHOT_CURRENT_NUMBER ), Models::getGlobal( Fn::Property::G_SCREENSHOT_CURRENT_NUMBER ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_ISOLINE_STANDARD_COLOR ), Models::getGlobal( Fn::Property::G_ISOLINE_STANDARD_COLOR ) );

    settings.setValue( Fn::Prop2String::e( Fn::Property::G_RENDER_CROSSHAIRS ), Models::getGlobal( Fn::Property::G_RENDER_CROSSHAIRS ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_RENDER_CROSSHAIRS_2 ), Models::getGlobal( Fn::Property::G_RENDER_CROSSHAIRS_2 ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_TRANSPARENCY ), Models::getGlobal( Fn::Property::G_TRANSPARENCY ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_MESH_TRANSPARENCY ), Models::getGlobal( Fn::Property::G_MESH_TRANSPARENCY ) );

    settings.setValue( Fn::Prop2String::e( Fn::Property::G_FILTER_SIZE ), Models::getGlobal( Fn::Property::G_FILTER_SIZE ) );

    settings.setValue( Fn::Prop2String::e( Fn::Property::G_SHOW_ORIENTHELPER ), Models::getGlobal( Fn::Property::G_SHOW_ORIENTHELPER ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_ORIENTHELPER_X ), Models::getGlobal( Fn::Property::G_ORIENTHELPER_X ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_ORIENTHELPER_Y ), Models::getGlobal( Fn::Property::G_ORIENTHELPER_Y ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_ORIENTHELPER_Z ), Models::getGlobal( Fn::Property::G_ORIENTHELPER_Z ) );
    settings.setValue( Fn::Prop2String::e( Fn::Property::G_ORIENTHELPER_SIZE ), Models::getGlobal( Fn::Property::G_ORIENTHELPER_SIZE ) );

    settings.setValue( Fn::Prop2String::e( Fn::Property::G_FIBERS_INITIAL_PERCENTAGE ), Models::getGlobal( Fn::Property::G_FIBERS_INITIAL_PERCENTAGE ) );


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

    if ( settings.contains( "lockDockTitles" ) )
    {
        if ( settings.value( "lockDockTitles" ).toBool() )
        {
            lockDockTitlesAct->activate( QAction::Trigger );
        }
    }
    loadSetting( settings, Fn::Property::G_LAST_PATH );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_PATH );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_MAIN );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_MAIN2 );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_COMBINED );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_NAV1 );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_NAV2 );
    loadSetting( settings, Fn::Property::G_BACKGROUND_COLOR_NAV3 );
    loadSetting( settings, Fn::Property::G_CROSSHAIR_COLOR );
    loadSetting( settings, Fn::Property::G_SHOW_NAV_SLIDERS );

    loadSetting( settings, Fn::Property::G_SCREENSHOT_PREFIX );
    loadSetting( settings, Fn::Property::G_SCREENSHOT_PREFIX2 );
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
    loadSetting( settings, Fn::Property::G_ISOLINE_STANDARD_COLOR );

    loadSetting( settings, Fn::Property::G_RENDER_CROSSHAIRS );
    loadSetting( settings, Fn::Property::G_RENDER_CROSSHAIRS_2 );
    loadSetting( settings, Fn::Property::G_TRANSPARENCY );
    loadSetting( settings, Fn::Property::G_MESH_TRANSPARENCY );

    loadSetting( settings, Fn::Property::G_FILTER_SIZE );
    loadSetting( settings, Fn::Property::G_SHOW_ORIENTHELPER );
    loadSetting( settings, Fn::Property::G_ORIENTHELPER_X );
    loadSetting( settings, Fn::Property::G_ORIENTHELPER_Y );
    loadSetting( settings, Fn::Property::G_ORIENTHELPER_Z );
    loadSetting( settings, Fn::Property::G_ORIENTHELPER_SIZE );
    loadSetting( settings, Fn::Property::G_FIBERS_INITIAL_PERCENTAGE );
}

void MainWindow::loadSetting( QSettings &settings, Fn::Property setting )
{
    if ( settings.contains( Fn::Prop2String::e( setting ) ) )
    {
        QVariant s = settings.value( Fn::Prop2String::e( setting ) );
        Models::setGlobal( setting, s );
    }
}

void MainWindow::loadColormaps( bool resetSettings )
{
    QSettings settings;
    if ( settings.contains( "colormaps" ) && !resetSettings )
    {
        qDebug( ) << "restore colormaps";
        QByteArray ar = settings.value( "colormaps" ).toByteArray();
        QDataStream in( &ar, QIODevice::ReadOnly );
        int countColormaps;
        in >> countColormaps;
        for ( int i = 0; i < countColormaps; ++i )
        {
            QString name;
            std::vector<ColormapPair> values;
            in >> name;
            int cmapSize;
            in >> cmapSize;
            for ( int k = 0; k < cmapSize; ++k )
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
        qDebug( ) << "create colormaps";
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
    QString fn = Models::getGlobal(  Fn::Property::G_LAST_PATH ).toString();

    QString filter( "all files (*.*);;niftii (*.nii *.nii.gz);;fib files (*.fib *.vtk *.asc *.json);;surfaces (*.vtk *.asc)" );

    fd = new QFileDialog( this, "Open File", fn, filter );
    fd->setFileMode( QFileDialog::ExistingFiles );
    fd->setAcceptMode( QFileDialog::AcceptOpen );

    QStringList fileNames;
    if ( fd->exec() )
    {
        fileNames = fd->selectedFiles();
    }
    delete fd;

    for ( int i = 0; i < fileNames.size(); ++i )
    {
        load( fileNames[i] );
    }
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>( sender() );
    if ( action )
    {
        load( action->data().toString() );
    }
}

void MainWindow::openRecentScene()
{
    QAction *action = qobject_cast<QAction *>( sender() );
    if ( action )
    {
        load( action->data().toString() );
    }
}

void MainWindow::loadRoi()
{
    QString fn = Models::getGlobal(  Fn::Property::G_LAST_PATH ).toString();

    QString filter( "all files (*.*);;niftii (*.nii *.nii.gz)" );

    fd = new QFileDialog( this, "Open File", fn, filter );
    fd->setFileMode( QFileDialog::ExistingFiles );
    fd->setAcceptMode( QFileDialog::AcceptOpen );

    QStringList fileNames;
    if ( fd->exec() )
    {
        fileNames = fd->selectedFiles();
    }
    delete fd;

    for ( int i = 0; i < fileNames.size(); ++i )
    {
        loadRoi( fileNames[i] );
    }
}

bool MainWindow::loadRoi( QString fn )
{
    if ( !fn.isEmpty() )
    {
        if ( fn.endsWith( ".nii.gz" ) || fn.endsWith( ".nii" ) || fn.endsWith( ".hdr" ) || fn.endsWith( ".ima" ) || fn.endsWith( ".img" ) )
        {
            Loader loader( selectedDataset() );
            loader.setFilename( QDir( fn ) );
            if ( loader.load() )
            {
                for ( int k = 0; k < loader.getNumDatasets(); ++k )
                {
                    DatasetScalar* dss = static_cast<DatasetScalar*>( loader.getDataset( k ) );
                    std::vector<float>* data = dss->getData();
                    std::vector<float> out( data->size(), 0.0 );

                    for ( unsigned int i = 0; i < data->size(); ++i )
                    {
                        out[i] = data->at( i );
                    }

                    ROIArea* roiOut = new ROIArea( out, dss->properties() );
                    Models::addROIArea( roiOut );
                }
                QFileInfo fi( fn );
                QDir dir = fi.absoluteDir();
                QString lastPath = dir.absolutePath();
                Models::g()->setData( Models::g()->index( (int) Fn::Property::G_LAST_PATH, 0 ), lastPath );
                setCurrentFile( fn );

                return true;
            }
        }
    }
    return false;
}

bool MainWindow::load( QString fileName )
{
    mainGLWidget->makeCurrent();
    if ( !fileName.isEmpty() )
    {
        if ( fileName.endsWith( "scn" ) || fileName.endsWith( "xml" ) )
        {
            loadScene( fileName );
            return true;
        }
        else if ( fileName.endsWith( "bgscript" ) )
        {
            m_scriptWidget->loadScript( fileName, true );
            return true;
        }
        else
        {
            Loader loader( selectedDataset() );
            loader.setFilename( QDir( fileName ) );
            if ( loader.load() )
            {
                for ( int k = 0; k < loader.getNumDatasets(); ++k )
                {
                    Models::d()->setData( Models::d()->index( Models::d()->rowCount(), (int) Fn::Property::D_NEW_DATASET ),
                            VPtr<Dataset>::asQVariant( loader.getDataset( k ) ), Qt::DisplayRole );
                }
                QFileInfo fi( fileName );
                QDir dir = fi.absoluteDir();
                QString lastPath = dir.absolutePath();
                Models::g()->setData( Models::g()->index( (int) Fn::Property::G_LAST_PATH, 0 ), lastPath );

                setCurrentFile( fileName );

                GLFunctions::reloadShaders();
                return true;
            }
        }
    }
    return false;
}

bool MainWindow::load( QString fileName, QList<QVariant> state )
{
    mainGLWidget->makeCurrent();
    if ( !fileName.isEmpty() )
    {
        if ( fileName.endsWith( "scn" ) )
        {
            loadScene( fileName );
            return true;
        }
        else if ( fileName.endsWith( "bgscript" ) )
        {
            m_scriptWidget->loadScript( fileName, true );
            return true;
        }
        else
        {
            Loader loader( selectedDataset() );
            loader.setFilename( QDir( fileName ) );
            if ( loader.load() )
            {
                for ( int k = 0; k < loader.getNumDatasets(); ++k )
                {
                    loader.getDataset( k )->properties().setState( state );
                    Models::d()->setData( Models::d()->index( Models::d()->rowCount(), (int) Fn::Property::D_NEW_DATASET ),
                            VPtr<Dataset>::asQVariant( loader.getDataset( k ) ), Qt::DisplayRole );
                }
                QFileInfo fi( fileName );
                QDir dir = fi.absoluteDir();
                QString lastPath = dir.absolutePath();
                Models::g()->setData( Models::g()->index( (int) Fn::Property::G_LAST_PATH, 0 ), lastPath );

                setCurrentFile( fileName );

                GLFunctions::reloadShaders();
                return true;
            }
        }
    }
    return false;
}

Dataset* MainWindow::selectedDataset()
{
    int selectedIndex = m_datasetWidget->getSelected();
    if ( selectedIndex > -1 )
    {
        return VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( selectedIndex, (int) Fn::Property::D_DATASET_POINTER ) ) );
    }
    else
    {
        return NULL;
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
    {
        files.removeLast();
    }

    settings.setValue( "recentFileList", files );

    foreach ( QWidget *widget, QApplication::topLevelWidgets() )
    {
        MainWindow *mainWin = qobject_cast< MainWindow * >( widget );
        if ( mainWin )
        {
            mainWin->updateRecentFileActions();
        }
    }
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value( "recentFileList" ).toStringList();

    int numRecentFiles = qMin( files.size(), (int) MaxRecentFiles );

    for ( int i = 0; i < numRecentFiles; ++i )
    {
        //QString text = tr( "&%1 %2" ).arg( i + 1 ).arg( strippedName( files[i] ) );
        QString text = tr( "&%1 %2" ).arg( i + 1 ).arg( files[i] );
        recentFileActs[i]->setText( text );
        recentFileActs[i]->setData( files[i] );
        recentFileActs[i]->setVisible( true );
    }
    for ( int j = numRecentFiles; j < MaxRecentFiles; ++j )
        recentFileActs[j]->setVisible( false );

    separatorAct->setVisible( numRecentFiles > 0 );
}

void MainWindow::setCurrentScene( const QString &fileName )
{
    curFile = fileName;
    setWindowFilePath( curFile );

    QSettings settings;
    QStringList files = settings.value( "recentSceneList" ).toStringList();
    files.removeAll( fileName );
    files.prepend( fileName );
    while ( files.size() > MaxRecentFiles )
    {
        files.removeLast();
    }

    settings.setValue( "recentSceneList", files );

    foreach ( QWidget *widget, QApplication::topLevelWidgets() )
    {
        MainWindow *mainWin = qobject_cast< MainWindow * >( widget );
        if ( mainWin )
        {
            mainWin->updateRecentSceneActions();
        }
    }
}

void MainWindow::updateRecentSceneActions()
{
    QSettings settings;
    QStringList files = settings.value( "recentSceneList" ).toStringList();

    int numRecentScenes = qMin( files.size(), (int) MaxRecentFiles );

    for ( int i = 0; i < numRecentScenes; ++i )
    {
        //QString text = tr( "&%1 %2" ).arg( i + 1 ).arg( strippedName( files[i] ) );
        QString text = tr( "&%1 %2" ).arg( i + 1 ).arg( files[i] );
        recentSceneActs[i]->setText( text );
        recentSceneActs[i]->setData( files[i] );
        recentSceneActs[i]->setVisible( true );
    }
    for ( int j = numRecentScenes; j < MaxRecentFiles; ++j )
        recentSceneActs[j]->setVisible( false );

    separatorAct->setVisible( numRecentScenes > 0 );
}

QString MainWindow::strippedName( const QString &fullFileName )
{
    return QFileInfo( fullFileName ).fileName();
}

void MainWindow::save()
{
    int selected = m_datasetWidget->getSelected();
    if ( selected != -1 )
    {
        Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( selected, (int) Fn::Property::D_DATASET_POINTER ) ) );
        save( ds );
    }
}

bool MainWindow::save( Dataset* ds )
{
    int type = ds->properties().get( Fn::Property::D_TYPE ).toInt();
    if ( type == (int) Fn::DatasetType::PLANE || type == (int) Fn::DatasetType::GUIDE || type == (int) Fn::DatasetType::LABEL )
    {
        return true;
    }

    QString fn = Models::getGlobal( Fn::Property::G_LAST_PATH ).toString();

    QString name = fn + "/" + ds->properties().get( Fn::Property::D_NAME ).toString().replace( " ", "" );
    name.replace( "." + ds->getDefaultSuffix(), "" );
    name += ".";
    name += ds->getDefaultSuffix();

    fd = new QFileDialog( this, "Save File", name, ds->getSaveFilter() );
    fd->setFileMode( QFileDialog::AnyFile );
    fd->setAcceptMode( QFileDialog::AcceptSave );
    fd->setDefaultSuffix( ds->getDefaultSuffix() );

    connect( fd, SIGNAL( filterSelected( QString ) ), this, SLOT( saveFilterChanged( QString ) ) );

    if ( fd->exec() )
    {
        QString fileName = fd->selectedFiles().first();
        // TODO: Qt5
        qDebug( ) << fileName << fd->selectedNameFilter();

        if ( !fileName.isEmpty() )
        {
            QDir dir;
            ds->properties().set( Fn::Property::D_FILENAME, fileName );
            ds->properties().set( Fn::Property::D_NAME, QDir( fileName ).path().split( "/" ).last() );
            saveDataset( ds, fd->selectedNameFilter() );

            QFileInfo fi( fileName );
            dir = fi.absoluteDir();
            QString lastPath = dir.absolutePath();
            Models::g()->setData( Models::g()->index( (int) Fn::Property::G_LAST_PATH, 0 ), lastPath );
            delete fd;
            return true;
        }
    }
    delete fd;
    return false;
}

void MainWindow::saveFilterChanged( QString filterString )
{
    //filter strings contain (*.xxx *.xxx *.xxx) with suffixes
    //this sets the first suffix in the first () as default suffix:
    int i = filterString.indexOf( "(*." );
    QString suffix = filterString.remove( 0, i + 3 );
    suffix = suffix.left( suffix.indexOf( " " ) );
    suffix = suffix.left( suffix.indexOf( ")" ) );
    if ( suffix != "*" )
    {
        qDebug( ) << "set default suffix: " << suffix;
        fd->setDefaultSuffix( suffix );
    }
}

void MainWindow::saveDataset( Dataset* ds, QString filter )
{
    int type = ds->properties().get( Fn::Property::D_TYPE ).toInt();
    if ( type == (int) Fn::DatasetType::PLANE || type == (int) Fn::DatasetType::GUIDE || type == (int) Fn::DatasetType::LABEL )
    {
        return;
    }

    QString fileName = ds->properties().get( Fn::Property::D_FILENAME ).toString();
    Writer writer( ds, QFileInfo( fileName ), filter );
    writer.save();
}

void MainWindow::saveRoi( ROIArea* roi )
{
    QString fileName = roi->properties()->get( Fn::Property::D_FILENAME ).toString();
    RoiWriter writer( roi, QFileInfo( fileName ) );
    writer.save();
}

void MainWindow::exportColormaps()
{
    QString fn = Models::getGlobal( Fn::Property::G_LAST_PATH ).toString();
    QString fileName = QFileDialog::getSaveFileName( this, "Export colormaps", fn );

    if ( !fileName.endsWith( ".xml" ) )
    {
        fileName += ".xml";
    }

    StateWriter writer;
    writer.saveColormaps( fileName );
}

void MainWindow::importColormaps()
{
    QString fn = Models::getGlobal( Fn::Property::G_LAST_PATH ).toString();

    QString filter( "braingl colormaps (*.xml);;all files (*.*)" );

    QStringList fileNames = QFileDialog::getOpenFileNames( this, "Open File", fn, filter );
    for ( int i = 0; i < fileNames.size(); ++i )
    {
        StateReader reader;
        reader.load( fileNames[i] );
    }
    m_colormapEditWidget->update();

    Models::g()->submit();
}

//TODO
void MainWindow::saveScene()
{
    QString fn = Models::getGlobal( Fn::Property::G_LAST_PATH ).toString();
    QString fileName = QFileDialog::getSaveFileName( this, "Save Scene", fn );
    if ( !fileName.endsWith( ".xml" ) )
    {
        fileName += ".xml";
    }
    saveScene( fileName, false );
}

void MainWindow::packAndGo()
{
    QString fn = Models::getGlobal( Fn::Property::G_LAST_PATH ).toString();
    QString fileName = QFileDialog::getSaveFileName( this, "Save Scene", fn );
    if ( !fileName.endsWith( ".xml" ) )
    {
        fileName += ".xml";
    }
    saveScene( fileName, true );
}

void MainWindow::saveScene( QString fileName, bool packAndGo )
{
    QFileInfo fi( fileName );
    QString newScenePath = fi.path();

    int countDatasets = Models::d()->rowCount();
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  save datasets to file if not formerly done so
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    for ( int i = 0; i < countDatasets; ++i )
    {
        Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int) Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
        QString fn = ds->properties().get( Fn::Property::D_FILENAME ).toString();

        QDir dir;
        if ( packAndGo || !dir.exists( fn ) )
        {
            // create tmp name
            ds->properties().set( Fn::Property::D_FILENAME,
                    newScenePath + QDir::separator() + QString::number( QDateTime::currentMSecsSinceEpoch() ) + "." + ds->getDefaultSuffix() );
            saveDataset( ds, "(*." + ds->getDefaultSuffix() + ")" );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  save rois to file
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    int numBranches = Models::r()->rowCount( QModelIndex() );
    QList<QVariant> roiStates;

    for ( int i = 0; i < numBranches; ++i )
    {
        int leafCount = Models::getBranchSize( i );

        QList<QVariant> branch;
        for ( int k = 0; k < leafCount + 1; ++k )
        {
            ROI* roi = Models::getRoi( i, k );

            if ( roi->properties()->get( Fn::Property::D_SHAPE ) == 10 )
            {
                QString fn = roi->properties()->get( Fn::Property::D_FILENAME ).toString();
                QDir dir;
                if ( packAndGo || !dir.exists( fn ) )
                {
                    roi->properties()->set( Fn::Property::D_FILENAME,
                            newScenePath + QDir::separator() + QString::number( QDateTime::currentMSecsSinceEpoch() ) + ".nii.gz" );
                    ROIArea* roi2save = dynamic_cast<ROIArea*>( roi );
                    saveRoi( roi2save );
                }
            }
        }
    }

    StateWriter writer;
    writer.saveScene( fileName, packAndGo, mainGLWidget->getCamera()->getState(), mainGLWidget->getArcBall()->getState(),
            mainGLWidget2->getCamera()->getState(), mainGLWidget2->getArcBall()->getState() );
}

void MainWindow::loadScene( QString fileName )
{
    if ( fileName.endsWith( ".scn" ) )
    {
        qWarning( ) << "*****************************************************************************************";
        qWarning( ) << "***                                                                                   ***";
        qWarning( ) << "***          Warning! Use of .scn is deprecated. Please re-save your scenes!          ***";
        qWarning( ) << "***                                                                                   ***";
        qWarning( ) << "*****************************************************************************************";
        loadSceneDeprecated( fileName );
        return;
    }

    StateReader reader;
    reader.load( fileName );

    QMap<QString, QMap<QString, QVariant> > cameras = reader.getCameras();
    if ( cameras.contains( "camera_maingl" ) )
    {
        mainGLWidget->getCamera()->setState( cameras["camera_maingl"] );
    }
    if ( cameras.contains( "arcball_maingl" ) )
    {
        mainGLWidget->getArcBall()->setState( cameras["arcball_maingl"] );
    }

    if ( cameras.contains( "camera_maingl2" ) )
    {
        mainGLWidget2->getCamera()->setState( cameras["camera_maingl2"] );
    }
    if ( cameras.contains( "arcball_maingl2" ) )
    {
        mainGLWidget2->getArcBall()->setState( cameras["arcball_maingl2"] );
    }

    setCurrentScene( fileName );
    GLFunctions::reloadShaders();
    Models::g()->submit();
}

void MainWindow::loadSceneDeprecated( QString fileName )
{
    QSettings settings( fileName, QSettings::IniFormat );

    QString versionString = "0.0.0";
    if ( settings.contains( "version" ) )
    {
        versionString = settings.value( "version" ).toString();
    }

    QList<QVariant> files = settings.value( "fileNames" ).toList();

    for ( int i = 0; i < files.size(); ++i )
    {
        if ( settings.contains( "file_" + QString::number( i ) + "_state" ) )
        {
            QList<QVariant> state = settings.value( "file_" + QString::number( i ) + "_state" ).toList();
            load( files[i].toString(), state );
        }
    }

    GLFunctions::reloadShaders();

    int numBranches = Models::r()->rowCount( QModelIndex() );

    QList<QVariant> rois = settings.value( "roiStates" ).toList();
    for ( int i = 0; i < rois.size(); ++i )
    {
        QList<QVariant> branch = rois[i].toList();

        QList<QVariant> roiState = branch[0].toList();
        GLFunctions::roi = new ROIBox();
        GLFunctions::roi->properties()->setState( roiState );

        Models::r()->insertRows( 0, 0, QModelIndex() );

        if ( branch.size() > 1 )
        {
            for ( int k = 1; k < branch.size(); ++k )
            {
                QList<QVariant> roiState = branch[k].toList();
                GLFunctions::roi = new ROIBox();
                GLFunctions::roi->properties()->setState( roiState );
                Models::r()->insertRows( 0, 0, Models::createRoiIndex( numBranches + i, 0, 0 ) );

                for ( int l = 0; l < roiState.size() / 2; ++l )
                {
                    if ( (Fn::Property) ( roiState[l * 2].toInt() ) == Fn::Property::D_COLOR )
                    {
                        GLFunctions::roi->properties()->set( (Fn::Property) ( roiState[l * 2].toInt() ), roiState[l * 2 + 1] );
                    }
                }
            }
        }
    }

    if ( versionString == "0.7.0" )
    {
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
    }
    else if ( versionString == "0.8.1" )
    {
        if ( settings.contains( "globalState" ) )
        {
            dynamic_cast<GlobalPropertyModel*>( Models::g() )->setState( settings.value( "globalState" ).toList() );
        }
    }

    if ( settings.contains( "camera_maingl" ) )
    {
        mainGLWidget->getCamera()->setState( settings.value( "camera_maingl" ).toList() );
    }
    if ( settings.contains( "arcball_maingl" ) )
    {
        mainGLWidget->getArcBall()->setState( settings.value( "arcball_maingl" ).toList() );
    }

    if ( settings.contains( "camera_maingl2" ) )
    {
        mainGLWidget2->getCamera()->setState( settings.value( "camera_maingl2" ).toList() );
    }
    if ( settings.contains( "arcball_maingl2" ) )
    {
        mainGLWidget2->getArcBall()->setState( settings.value( "arcball_maingl2" ).toList() );
    }

    Models::g()->submit();
}


void MainWindow::about()
{
    QString hg = HGTIP;
    hg.remove( ";" );
    hg.remove( "changeset:" );
    hg.replace( " ", "" );

    QString message( "<b>brainGL development version " + hg +
#ifdef __DEBUG__
            " (debug)"
#elif __RELEASE__
            " (release)"
#endif
            "<br><br> </b>"
                    "Exploring and visualizing anatomical and functional connectivity in the brain.<br><br>"
                    "For questions please consult the <a href=\"https://code.google.com/p/braingl/wiki/Main\"><b>documentation</b></a>.<br><br>"
                    "Please report bugs and feature requests <a href=\"https://code.google.com/p/braingl/issues/list\"><b>here</b></a>.<br><br>" );

    QMessageBox::about( this, tr( "About brainGL" ), message );
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
    connect( openAct, SIGNAL( triggered() ), this, SLOT( open() ) );

    saveAct = new QAction( QIcon( ":/icons/save.png" ), tr( "&Save..." ), this );
    saveAct->setShortcuts( QKeySequence::Save );
    saveAct->setStatusTip( tr( "Save the current form letter" ) );
    connect( saveAct, SIGNAL( triggered() ), this, SLOT( save() ) );

    quitAct = new QAction( tr( "&Quit" ), this );
    quitAct->setShortcuts( QKeySequence::Quit );
    quitAct->setStatusTip( tr( "Quit the application" ) );
    connect( quitAct, SIGNAL( triggered() ), this, SLOT( close() ) );

    loadRoiAct = new QAction( tr( "Load ROI" ), this );
    loadRoiAct->setStatusTip( tr( "Load a ROI from a Nifti file " ) );
    connect( loadRoiAct, SIGNAL( triggered() ), this, SLOT( loadRoi() ) );

    saveSceneAct = new QAction( tr( "Save Scene" ), this );
    saveSceneAct->setStatusTip( tr( "Save the current scene" ) );
    connect( saveSceneAct, SIGNAL( triggered() ), this, SLOT( saveScene() ) );

    packAndGoAct = new QAction( tr( "Pack and Go" ), this );
    packAndGoAct->setStatusTip( tr( "Copy all loaded datasets into selected directory and save scene" ) );
    connect( packAndGoAct, SIGNAL( triggered() ), this, SLOT( packAndGo() ) );

    exportColormapsAct = new QAction( tr( "Export Colormaps" ), this );
    exportColormapsAct->setStatusTip( tr( "Save current colormaps to a file" ) );
    connect( exportColormapsAct, SIGNAL( triggered() ), this, SLOT( exportColormaps() ) );

    importColormapsAct = new QAction( tr( "Import Colormaps" ), this );
    importColormapsAct->setStatusTip( tr( "Loads current colormaps from a file" ) );
    connect( importColormapsAct, SIGNAL( triggered() ), this, SLOT( importColormaps() ) );

    screenshotAct = new QAction( QIcon( ":/icons/camera.png" ), tr( "Screenshot" ), this );
    screenshotAct->setStatusTip( tr( "Sreenshot" ) );
    screenshotAct->setShortcut( QKeySequence( Qt::CTRL + Qt::Key_P ) );
    connect( screenshotAct, SIGNAL( triggered() ), this, SLOT( screenshot() ) );

    resetSettingsAct = new QAction( tr( "Reset Settings" ), this );
    resetSettingsAct->setStatusTip( tr( "Reset Settings" ) );
    connect( resetSettingsAct, SIGNAL( triggered() ), this, SLOT( resetSettings() ) );

    aboutAct = new QAction( tr( "&About" ), this );
    aboutAct->setStatusTip( tr( "Show the application's About box" ) );
    connect( aboutAct, SIGNAL( triggered() ), this, SLOT( about() ) );

    newMainGLAct = new QAction( tr( "New 3D view" ), this );
    newMainGLAct->setStatusTip( tr( "New 3D view" ) );
    connect( newMainGLAct, SIGNAL( triggered() ), this, SLOT( newMainGL() ) );

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
    standardViewAxialAct->setStatusTip( tr( "Set view to axial." ) );
    connect( standardViewAxialAct, SIGNAL( triggered() ), this, SLOT( slotStandardAxialView() ) );

    standardViewCoronalAct = new QAction( QIcon( ":/icons/coronal.png" ), tr( "Coronal View" ), this );
    standardViewCoronalAct->setStatusTip( tr( "Set view to coronal." ) );
    connect( standardViewCoronalAct, SIGNAL( triggered() ), this, SLOT( slotStandardCoronalView() ) );

    standardViewSagittalAct = new QAction( QIcon( ":/icons/sagittal.png" ), tr( "Sagittal view" ), this );
    standardViewSagittalAct->setStatusTip( tr( "Set view to sagittal." ) );
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

    dilbertAct = new QAction( QIcon( ":/icons/box.png" ), tr( "Dilbert" ), this );
    dilbertAct->setStatusTip( tr( "Dilbert" ) );
    connect( dilbertAct, SIGNAL( triggered() ), this, SLOT( slotDilbert() ) );

    for ( int i = 0; i < MaxRecentFiles; ++i )
    {
        recentFileActs[i] = new QAction( this );
        recentFileActs[i]->setVisible( false );
        connect( recentFileActs[i], SIGNAL( triggered() ), this, SLOT( openRecentFile() ) );
    }

    for ( int i = 0; i < MaxRecentFiles; ++i )
    {
        recentSceneActs[i] = new QAction( this );
        recentSceneActs[i]->setVisible( false );
        connect( recentSceneActs[i], SIGNAL( triggered() ), this, SLOT( openRecentScene() ) );
    }

    continousRenderingAct = new QAction( QIcon( ":/icons/continous.png" ), tr( "Continous Rendering" ), this );
    continousRenderingAct->setStatusTip( tr( "continous rendering, warning this might slow down your computer" ) );
    continousRenderingAct->setCheckable( true );
    continousRenderingAct->setChecked( false );
    connect( continousRenderingAct, SIGNAL( toggled( bool ) ), this, SLOT( continousRendering() ) );

    newPlaneAct = new QAction( QIcon( ":/icons/plane.png" ), tr( "new Plane" ), this );
    newPlaneAct->setStatusTip( tr( "create a new plane" ) );
    newPlaneAct->setCheckable( false );
    newPlaneAct->setChecked( false );
    connect( newPlaneAct, SIGNAL( triggered() ), this, SLOT( newPlane() ) );

    newGuideAct = new QAction( QIcon( ":/icons/crosshair.png" ), tr( "new Guide" ), this );
    newGuideAct->setStatusTip( tr( "create a new guides" ) );
    newGuideAct->setCheckable( false );
    newGuideAct->setChecked( false );
    connect( newGuideAct, SIGNAL( triggered() ), this, SLOT( newGuide() ) );

    newLabelAct = new QAction( QIcon( ":/icons/label.png" ), tr( "new Label" ), this );
    newLabelAct->setStatusTip( tr( "create a new label" ) );
    newLabelAct->setCheckable( false );
    newLabelAct->setChecked( false );
    connect( newLabelAct, SIGNAL( triggered() ), this, SLOT( newLabel() ) );
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu( tr( "&File" ) );
    fileMenu->addAction( openAct );
    fileMenu->addAction( saveAct );
    separatorAct = fileMenu->addSeparator();

    fileMenu->addAction( loadRoiAct );
    separatorAct = fileMenu->addSeparator();

    fileMenu->addAction( saveSceneAct );
    fileMenu->addAction( packAndGoAct );
    separatorAct = fileMenu->addSeparator();

    fileMenu->addAction( exportColormapsAct );
    fileMenu->addAction( importColormapsAct );
    separatorAct = fileMenu->addSeparator();

    recentFilesMenu = fileMenu->addMenu( "Recent files" );

    for ( int i = 0; i < MaxRecentFiles; ++i )
    {
        recentFilesMenu->addAction( recentFileActs[i] );
    }

    recentScenesMenu = fileMenu->addMenu( "Recent scenes" );

    for ( int i = 0; i < MaxRecentFiles; ++i )
    {
        recentScenesMenu->addAction( recentSceneActs[i] );
    }

    fileMenu->addSeparator();
    fileMenu->addAction( quitAct );
    updateRecentFileActions();
    updateRecentSceneActions();

    viewMenu = menuBar()->addMenu( tr( "&Widgets" ) );
    viewMenu->addAction( lockDockTitlesAct );
    viewMenu->addSeparator();

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu( tr( "&Help" ) );
    helpMenu->addAction( screenshotAct );
    helpMenu->addAction( resetSettingsAct );
    helpMenu->addAction( dilbertAct );

    helpMenu->addAction( aboutAct );
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar( tr( "File" ) );
    fileToolBar->addAction( newAct );
    fileToolBar->addAction( openAct );
    fileToolBar->addAction( saveAct );
    fileToolBar->addAction( screenshotAct );
    fileToolBar->addAction( newPlaneAct );
    fileToolBar->addAction( newGuideAct );
    fileToolBar->addAction( newLabelAct );
    //fileToolBar->addAction( continousRenderingAct );
    //fileToolBar->addAction( printAct );
    if ( m_debug )
    {
        fileToolBar->addAction( toggleShaderEditAct );
    }
    fileToolBar->setObjectName( "fileToolbar" );

    editToolBar = addToolBar( tr( "Edit" ) );
    editToolBar->addAction( showAxialAct );
    editToolBar->addAction( showCoronalAct );
    editToolBar->addAction( showSagittalAct );
    editToolBar->addAction( standardViewAxialAct );
    editToolBar->addAction( standardViewCoronalAct );
    editToolBar->addAction( standardViewSagittalAct );
    editToolBar->addAction( newSelectionBoxAct );
    editToolBar->setObjectName( "editToolbar" );

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
    FNDockWidget* dockDSW = new FNDockWidget( QString( "Dataset List" ), m_datasetWidget, this );
    addDockWidget( Qt::LeftDockWidgetArea, dockDSW );
    viewMenu->addAction( dockDSW->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockDSW, SLOT( toggleTitleWidget() ) );

    connect( m_datasetWidget, SIGNAL( moveSelectedItemUp( int ) ), Models::d(), SLOT( moveItemUp( int ) ) );
    connect( m_datasetWidget, SIGNAL( moveSelectedItemDown( int ) ), Models::d(), SLOT( moveItemDown( int ) ) );
    connect( m_datasetWidget, SIGNAL( deleteSelectedItem( int ) ), Models::d(), SLOT( deleteItem( int ) ) );

    m_colormapEditWidget = new ColormapEditWidget( this );
    FNDockWidget* dockCE = new FNDockWidget( QString( "colormap edit" ), m_colormapEditWidget, this );
    addDockWidget( Qt::LeftDockWidgetArea, dockCE );
    viewMenu->addAction( dockCE->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockCE, SLOT( toggleTitleWidget() ) );
    dockCE->hide();

    m_roiWidget = new ROIWidget( this );
    Models::setRoiWidget( m_roiWidget );
    FNDockWidget* dockSBW = new FNDockWidget( QString( "ROIs" ), m_roiWidget, this );
    addDockWidget( Qt::RightDockWidgetArea, dockSBW );
    viewMenu->addAction( dockSBW->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockSBW, SLOT( toggleTitleWidget() ) );
    connect( newSelectionBoxAct, SIGNAL( triggered() ), m_roiWidget, SLOT( addBox() ) );

    DatasetPropertyWidget* dsProperties = new DatasetPropertyWidget( "maingl", this );
    m_dockDSP = new FNDockWidget( QString( "dataset properties" ), dsProperties, this );
    addDockWidget( Qt::LeftDockWidgetArea, m_dockDSP );
    dsProperties->setSelectionModel( m_datasetWidget->selectionModel() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), m_dockDSP, SLOT( toggleTitleWidget() ) );
    connect( m_colormapEditWidget, SIGNAL( signalUpdate() ), dsProperties, SLOT( update() ) );
    connect( m_datasetWidget->selectionModel(), SIGNAL( selectionChanged( QItemSelection, QItemSelection ) ), this,
            SLOT( slotDatasetSelectionChanged() ) );

    DatasetPropertyWidget* dsProperties2 = new DatasetPropertyWidget( "maingl2", this );
    FNDockWidget* dockDSP2 = new FNDockWidget( QString( "dataset properties 2" ), dsProperties2, this );
    addDockWidget( Qt::LeftDockWidgetArea, dockDSP2 );
    dsProperties2->setSelectionModel( m_datasetWidget->selectionModel() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockDSP2, SLOT( toggleTitleWidget() ) );
    connect( m_colormapEditWidget, SIGNAL( signalUpdate() ), dsProperties2, SLOT( update() ) );
    dockDSP2->hide();

    ROIPropertyWidget* roiProperties = new ROIPropertyWidget( this );
    FNDockWidget* dockRP = new FNDockWidget( QString( "roi properties" ), roiProperties, this );
    addDockWidget( Qt::RightDockWidgetArea, dockRP );
    roiProperties->setModel( Models::r() );
    roiProperties->setSelectionModel( m_roiWidget->selectionModel() );
    viewMenu->addAction( dockRP->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockRP, SLOT( toggleTitleWidget() ) );

    GlobalPropertyWidget* globalProperties = new GlobalPropertyWidget( this );
    FNDockWidget* dockGP = new FNDockWidget( QString( "Global Properties" ), globalProperties, this );
    addDockWidget( Qt::LeftDockWidgetArea, dockGP );
    globalProperties->setModel( Models::g() );
    viewMenu->addAction( dockGP->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockGP, SLOT( toggleTitleWidget() ) );

    DatasetInfoView *dsInfo = new DatasetInfoView( this );
    FNDockWidget* dockDSI = new FNDockWidget( QString( "dataset info" ), dsInfo, this );
    addDockWidget( Qt::BottomDockWidgetArea, dockDSI );
    dsInfo->setModel( Models::d() );
    dsInfo->setSelectionModel( m_datasetWidget->selectionModel() );
    viewMenu->addAction( dockDSI->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockDSI, SLOT( toggleTitleWidget() ) );
    dockDSI->hide();

    tabifyDockWidget( dockGP, m_dockDSP );
    tabifyDockWidget( dockGP, dockCE );
    tabifyDockWidget( dockGP, dockDSP2 );

    // GL Widgets
    mainGLWidget = new GLWidget( "maingl", m_roiWidget->selectionModel() );
    FNDockWidget* dockMainGL = new FNDockWidget( QString( "main gl" ), mainGLWidget, this );
    m_centralWidget->addDockWidget( Qt::LeftDockWidgetArea, dockMainGL );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockMainGL, SLOT( toggleTitleWidget() ) );
    connect( dockMainGL, SIGNAL( visibilityChanged( bool ) ), mainGLWidget, SLOT( visibilityChanged( bool ) ) );
    mainGLWidget->update();
    m_scriptWidget = new ScriptWidget( mainGLWidget, this );
    FNDockWidget* dockSW = new FNDockWidget( "script", m_scriptWidget, this );
    m_centralWidget->addDockWidget( Qt::LeftDockWidgetArea, dockSW );
    viewMenu->addAction( dockSW->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockSW, SLOT( toggleTitleWidget() ) );
    connect( m_scriptWidget, SIGNAL( screenshot() ), this, SLOT( screenshot() ) );
    //connect( mainGLWidget, SIGNAL( signalCameraChanged() ), m_scriptWidget, SLOT( slotCameraChanged() ) );
    connect( mainGLWidget, SIGNAL( signalCopyCameraToScript( int ) ), m_scriptWidget, SLOT( slotCopyCamera( int ) ) );
    m_centralWidget->tabifiedDockWidgets( dockSW );
    //dockSW->hide();

    mainGLWidget2 = new GLWidget( "maingl2", m_roiWidget->selectionModel(), this, mainGLWidget );
    FNDockWidget* dockMainGL2 = new FNDockWidget( QString( "main gl 2" ), mainGLWidget2, this );
    m_centralWidget->addDockWidget( Qt::LeftDockWidgetArea, dockMainGL2 );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockMainGL2, SLOT( toggleTitleWidget() ) );
    connect( dockMainGL2, SIGNAL( visibilityChanged( bool ) ), mainGLWidget2, SLOT( visibilityChanged( bool ) ) );

    DockNavGLWidget* nav1 = new DockNavGLWidget( QString( "axial" ), 2, this, mainGLWidget );
    FNDockWidget* dockNav1 = new FNDockWidget( QString( "axial" ), nav1, this );
    m_centralWidget->addDockWidget( Qt::RightDockWidgetArea, dockNav1 );
    viewMenu->addAction( dockNav1->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockNav1, SLOT( toggleTitleWidget() ) );
    connect( dockNav1, SIGNAL( visibilityChanged( bool ) ), nav1, SLOT( setWidgetVisible( bool) ) );

    DockNavGLWidget* nav2 = new DockNavGLWidget( QString( "sagittal" ), 0, this, mainGLWidget );
    FNDockWidget* dockNav2 = new FNDockWidget( QString( "sagittal" ), nav2, this );
    m_centralWidget->addDockWidget( Qt::RightDockWidgetArea, dockNav2 );
    viewMenu->addAction( dockNav2->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockNav2, SLOT( toggleTitleWidget() ) );
    connect( dockNav2, SIGNAL( visibilityChanged( bool ) ), nav2, SLOT( setWidgetVisible( bool) ) );

    DockNavGLWidget* nav3 = new DockNavGLWidget( QString( "coronal" ), 1, this, mainGLWidget );
    FNDockWidget* dockNav3 = new FNDockWidget( QString( "coronal" ), nav3, this );
    m_centralWidget->addDockWidget( Qt::RightDockWidgetArea, dockNav3 );
    viewMenu->addAction( dockNav3->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockNav3, SLOT( toggleTitleWidget() ) );
    connect( dockNav3, SIGNAL( visibilityChanged( bool ) ), nav3, SLOT( setWidgetVisible( bool ) ) );

//    CombinedNavGLWidget* nav4 = new CombinedNavGLWidget( QString( "combined" ), this, mainGLWidget );
//    FNDockWidget* dockNav4 = new FNDockWidget( QString("Combined Nav"), nav4, this );
//    m_centralWidget->addDockWidget( Qt::LeftDockWidgetArea, dockNav4 );
//    viewMenu->addAction( dockNav4->toggleViewAction() );
//    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockNav4, SLOT( toggleTitleWidget() ) );
//    connect( dockNav4, SIGNAL( visibilityChanged( bool ) ), nav4, SLOT( setWidgetVisible( bool ) ) );

    HierarchicalTreeGLWidget* htw = new HierarchicalTreeGLWidget( QString( "tree" ), this, mainGLWidget );
    FNDockWidget* dockHTW = new FNDockWidget( QString( "tree" ), htw, this );
    m_centralWidget->addDockWidget( Qt::RightDockWidgetArea, dockHTW );
    viewMenu->addAction( dockHTW->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockHTW, SLOT( toggleTitleWidget() ) );
    connect( dockHTW, SIGNAL( visibilityChanged( bool ) ), htw, SLOT( setWidgetVisible( bool ) ) );

    HierarchicalTreeGLWidget* htw2 = new HierarchicalTreeGLWidget( QString( "tree2" ), this, mainGLWidget );
    FNDockWidget* dockHTW2 = new FNDockWidget( QString( "tree2" ), htw2, this );
    m_centralWidget->addDockWidget( Qt::RightDockWidgetArea, dockHTW2 );
    viewMenu->addAction( dockHTW2->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockHTW2, SLOT( toggleTitleWidget() ) );
    connect( dockHTW2, SIGNAL( visibilityChanged( bool ) ), htw2, SLOT( setWidgetVisible( bool ) ) );

    dockNav1->hide();
    dockNav2->hide();
    dockNav3->hide();
//    dockNav4->hide();
    dockMainGL2->hide();
    dockHTW->hide();
    dockHTW2->hide();

    SingleSHWidget* sshw = new SingleSHWidget( QString( "single sh" ), this, mainGLWidget );
    FNDockWidget* dockSSHW = new FNDockWidget( QString( "single sh" ), sshw, this );
    m_centralWidget->addDockWidget( Qt::LeftDockWidgetArea, dockSSHW );
    viewMenu->addAction( dockSSHW->toggleViewAction() );
    connect( lockDockTitlesAct, SIGNAL( triggered() ), dockSSHW, SLOT( toggleTitleWidget() ) );
    connect( dockSSHW, SIGNAL( visibilityChanged( bool ) ), sshw, SLOT( visibilityChanged( bool ) ) );
    dockSSHW->hide();

    viewMenu->addSeparator();
    viewMenu->addAction( dockMainGL->toggleViewAction() );
    viewMenu->addAction( m_dockDSP->toggleViewAction() );
    viewMenu->addAction( dockMainGL2->toggleViewAction() );
    viewMenu->addAction( dockDSP2->toggleViewAction() );

    //m_centralWidget->tabifyDockWidget( dockSSHW, dockNav4 );
//    m_centralWidget->tabifyDockWidget( dockNav4, dockMainGL );
    m_centralWidget->tabifyDockWidget( dockMainGL2, dockMainGL );
    m_centralWidget->tabifyDockWidget( dockSW, dockMainGL );
    //m_centralWidget->tabifyDockWidget( dockFS, dockMainGL );

    connect( mainGLWidget, SIGNAL( signalKeyPressed( int, Qt::KeyboardModifiers ) ), m_datasetWidget,
            SLOT( slotKeyPressed( int, Qt::KeyboardModifiers ) ) );
    connect( mainGLWidget, SIGNAL( signalKeyPressed( int, Qt::KeyboardModifiers ) ), m_scriptWidget,
            SLOT( slotKeyPressed( int, Qt::KeyboardModifiers ) ) );
}

void MainWindow::slotToggleAxialSlice()
{
    Models::g()->setData( Models::g()->index( (int) Fn::Property::G_SHOW_AXIAL, 0 ), showAxialAct->isChecked() );
    Models::g()->submit();
}

void MainWindow::slotToggleCoronalSlice()
{
    Models::g()->setData( Models::g()->index( (int) Fn::Property::G_SHOW_CORONAL, 0 ), showCoronalAct->isChecked() );
    Models::g()->submit();
}

void MainWindow::slotToggleSagittalSlice()
{
    Models::g()->setData( Models::g()->index( (int) Fn::Property::G_SHOW_SAGITTAL, 0 ), showSagittalAct->isChecked() );
    Models::g()->submit();
}

void MainWindow::slotStandardAxialView()
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

void MainWindow::slotStandardCoronalView()
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

void MainWindow::slotStandardSagittalView()
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
    FNDockWidget* dockSEW = new FNDockWidget( QString( "shader edit" ), m_shaderEditWidget, this );
    viewMenu->addAction( dockSEW->toggleViewAction() );
    m_centralWidget->addDockWidget( Qt::RightDockWidgetArea, dockSEW );
    tabifiedDockWidgets( dockSEW );
}

void MainWindow::slotRenderCrosshairs( bool value )
{
    Models::g()->setData( Models::g()->index( (int) Fn::Property::G_RENDER_CROSSHAIRS, 0 ), value );
}

void MainWindow::screenshot( bool exitAfter )
{
    QString path = Models::getGlobal(  Fn::Property::G_SCREENSHOT_PATH ).toString();
    if ( !path.endsWith( QDir::separator() ) )
    {
        path += QDir::separator();
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
        mainGLWidget->screenshot( path + prefix + numberString + QString( ".png" ), exitAfter );
    }
    if ( Models::getGlobal( Fn::Property::G_SCREENSHOT_DO_MAINGL2 ).toBool() )
    {
        mainGLWidget2->screenshot( path + prefix2 + numberString + QString( ".png" ), exitAfter );
    }
    Models::g()->submit();
}

void MainWindow::resetSettings()
{
    QMessageBox msgBox;
    msgBox.setText( "Reset settings." );
    msgBox.setInformativeText( "This will shut down brainGL. Continue?" );
    msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::Cancel );
    msgBox.setDefaultButton( QMessageBox::Yes );
    int ret = msgBox.exec();
    switch ( ret )
    {
        case QMessageBox::Yes:
        {
            QSettings settings;
            settings.clear();
            settings.sync();
            exit( 0 );
            break;
        }
        case QMessageBox::Cancel:
            break;
    }
}

void MainWindow::slotDilbert()
{
    QWidget* widget = new QWidget();
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setContentsMargins( 1, 1, 1, 1 );
    vLayout->setSpacing( 1 );
    QWebView* wv = new QWebView( this );
    wv->setUrl( QUrl( "http://www.dilbert.com/fast/" ) );
    vLayout->addWidget( wv );
    widget->setLayout( vLayout );
    widget->show();
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
    FNDockWidget* dockDSP2 = new FNDockWidget( QString( "dataset properties " + QString::number( MainWindow::countMainGL ) ), dsProperties2, this );
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

void MainWindow::runScript()
{
    m_scriptWidget->run( true );
}

void MainWindow::slotDatasetSelectionChanged()
{
    m_dockDSP->show();
    m_dockDSP->raise();
}

void MainWindow::newPlane()
{
    DatasetPlane* plane = new DatasetPlane();
    Models::addDataset( plane );
}

void MainWindow::newGuide()
{
    DatasetGuides* g = new DatasetGuides();
    Models::addDataset( g );
}

void MainWindow::newLabel()
{
    DatasetLabel* g = new DatasetLabel();
    Models::addDataset( g );
}
