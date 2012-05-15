#include <QtGui/QtGui>

#include "../data/datastore.h"

#include "glwidget.h"
#include "datasetlistwidget.h"
#include "datasetinfowidget.h"
#include "datasetpropertywidget.h"
#include "docknavglwidget.h"
#include "combinednavglwidget.h"
#include "dockcombinednavglwidget.h"
#include "mainwindow.h"

MainWindow::MainWindow( DataStore* dataStore ) :
	QMainWindow(),
    m_dataStore( dataStore )
{
	QSettings settings;
	restoreGeometry( settings.value( "mainWindowGeometry" ).toByteArray() );

	m_centralTabWidget = new QTabWidget( this );
	setCentralWidget( m_centralTabWidget );

    mainGLWidget = new GLWidget( m_dataStore );
    m_centralTabWidget->addTab( mainGLWidget, "main gl" );

    CombinedNavGLWidget* combNav = new CombinedNavGLWidget( m_dataStore, QString( "combined" ), this, mainGLWidget );
    m_centralTabWidget->addTab( combNav, "slices" );

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();

    setWindowTitle( tr( "Fibernavigator 2" ) );

    setUnifiedTitleAndToolBarOnMac( true );

	restoreState( settings.value( "mainWindowState" ).toByteArray() );

	m_dataStore->updateGlobals();
}

void MainWindow::closeEvent( QCloseEvent *event )
{
	QSettings settings;
	settings.setValue( "mainWindowGeometry", saveGeometry() );
	settings.setValue( "mainWindowState", saveState() );
}

void MainWindow::print()
{
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName( this );
    if ( !fileName.isEmpty() )
    {
    	m_dataStore->load( fileName );
    }
}

void MainWindow::save()
{
}

void MainWindow::undo()
{
}


void MainWindow::about()
{
    QMessageBox::about( this, tr( "About Fibernavigator 2" ),
            tr( "The <b>Fibernavigator 2</b> will be the replacement "
                    "of Fibernavigator and so much more." ) );
}

void MainWindow::createActions()
{
    openAct = new QAction( QIcon( ":/icons/open.png" ), tr( "&Open..." ),
            this );
    openAct->setShortcuts( QKeySequence::Open );
    openAct->setStatusTip( tr( "Load Dataset" ) );
    connect( openAct, SIGNAL(triggered()), this, SLOT(open()) );

    saveAct = new QAction( QIcon( ":/icons/save.png" ), tr( "&Save..." ),
            this );
    saveAct->setShortcuts( QKeySequence::Save );
    saveAct->setStatusTip( tr( "Save the current form letter" ) );
    connect( saveAct, SIGNAL(triggered()), this, SLOT(save()) );

    printAct = new QAction( QIcon( ":/icons/print.png" ), tr( "&Print..." ),
            this );
    printAct->setShortcuts( QKeySequence::Print );
    printAct->setStatusTip( tr( "Print the current form letter" ) );
    connect( printAct, SIGNAL(triggered()), this, SLOT(print()) );

    undoAct = new QAction( QIcon( ":/icons/undo.png" ), tr( "&Undo" ), this );
    undoAct->setShortcuts( QKeySequence::Undo );
    undoAct->setStatusTip( tr( "Undo the last editing action" ) );
    connect( undoAct, SIGNAL(triggered()), this, SLOT(undo()) );

    quitAct = new QAction( tr( "&Quit" ), this );
    quitAct->setShortcuts( QKeySequence::Quit );
    quitAct->setStatusTip( tr( "Quit the application" ) );
    connect( quitAct, SIGNAL(triggered()), this, SLOT(close()) );

    aboutAct = new QAction( tr( "&About" ), this );
    aboutAct->setStatusTip( tr( "Show the application's About box" ) );
    connect( aboutAct, SIGNAL(triggered()), this, SLOT(about()) );

    aboutQtAct = new QAction( tr( "About &Qt" ), this );
    aboutQtAct->setStatusTip( tr( "Show the Qt library's About box" ) );
    connect( aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()) );
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu( tr( "&File" ) );
    fileMenu->addAction( openAct );
    fileMenu->addAction( saveAct );
    fileMenu->addAction( printAct );
    fileMenu->addSeparator();
    fileMenu->addAction( quitAct );

    editMenu = menuBar()->addMenu( tr( "&Edit" ) );
    editMenu->addAction( undoAct );

    viewMenu = menuBar()->addMenu( tr( "&View" ) );

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu( tr( "&Help" ) );
    helpMenu->addAction( aboutAct );
    helpMenu->addAction( aboutQtAct );
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar( tr( "File" ) );
    fileToolBar->addAction( openAct );
    fileToolBar->addAction( saveAct );
    fileToolBar->addAction( printAct );
    fileToolBar->setObjectName( "fileToolbar");

    editToolBar = addToolBar( tr( "Edit" ) );
    editToolBar->addAction( undoAct );
    editToolBar->setObjectName( "editToolbar");
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage( tr( "Ready" ) );
}

void MainWindow::createDockWindows()
{
	DatasetListWidget* dsList = new DatasetListWidget( this );
	dsList->setModel( m_dataStore );
	addDockWidget( Qt::LeftDockWidgetArea, dsList );
	viewMenu->addAction( dsList->toggleViewAction() );

	connect( dsList, SIGNAL( moveSelectedItemUp( int ) ), m_dataStore, SLOT( moveItemUp( int ) ) );
	connect( dsList, SIGNAL( moveSelectedItemDown( int ) ), m_dataStore, SLOT( moveItemDown( int ) ) );
	connect( dsList, SIGNAL( deleteSelectedItem( int ) ), m_dataStore, SLOT( deleteItem( int ) ) );

	DatasetPropertyWidget* dsProperties = new DatasetPropertyWidget( QString("properties"), this );
    addDockWidget( Qt::LeftDockWidgetArea, dsProperties );
    dsProperties->setModel( m_dataStore );
    dsProperties->setSelectionModel( dsList->selectionModel() );
    viewMenu->addAction( dsProperties->toggleViewAction() );


	DatasetInfoWidget *dsInfo = new DatasetInfoWidget( tr( "Dataset Info Table" ), this );
	addDockWidget( Qt::BottomDockWidgetArea, dsInfo );
	dsInfo->setModel( m_dataStore );
    dsInfo->setSelectionModel( dsList->selectionModel() );
    viewMenu->addAction( dsInfo->toggleViewAction() );

    DockNavGLWidget* nav1 = new DockNavGLWidget( m_dataStore, QString("axial"), this, mainGLWidget );
    addDockWidget( Qt::RightDockWidgetArea, nav1 );
    viewMenu->addAction( nav1->toggleViewAction() );

    DockNavGLWidget* nav2 = new DockNavGLWidget( m_dataStore, QString( "sagittal" ), this, mainGLWidget );
    addDockWidget( Qt::RightDockWidgetArea, nav2 );
    viewMenu->addAction( nav2->toggleViewAction() );

    DockNavGLWidget* nav3 = new DockNavGLWidget( m_dataStore, QString( "coronal" ), this, mainGLWidget );
    addDockWidget( Qt::RightDockWidgetArea, nav3 );
    viewMenu->addAction( nav3->toggleViewAction() );

    DockCombinedNavGLWidget* nav4 = new DockCombinedNavGLWidget( m_dataStore, QString( "combined" ), this, mainGLWidget );
    addDockWidget( Qt::RightDockWidgetArea, nav4 );
    viewMenu->addAction( nav4->toggleViewAction() );
}
