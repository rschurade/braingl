#include <QtGui/QtGui>

#include "../data/datastore.h"

#include "glwidget.h"
#include "datasetviewwidget.h"

#include "mainwindow.h"

MainWindow::MainWindow( DataStore* dataStore ) :
    m_dataStore( dataStore )
{
    mainGLWidget = new GLWidget;
    setCentralWidget( mainGLWidget );

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();

    setWindowTitle( tr( "Fibernavigator 2" ) );

    setUnifiedTitleAndToolBarOnMac( true );
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

    editToolBar = addToolBar( tr( "Edit" ) );
    editToolBar->addAction( undoAct );
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage( tr( "Ready" ) );
}

void MainWindow::createDockWindows()
{
	DatasetViewWidget* dock1 = new DatasetViewWidget( this );
	dock1->setModel( m_dataStore );
	addDockWidget( Qt::LeftDockWidgetArea, dock1 );
	viewMenu->addAction( dock1->toggleViewAction() );

	connect( dock1, SIGNAL( moveSelectedItemUp( int ) ), m_dataStore, SLOT( moveItemUp( int ) ) );
	connect( dock1, SIGNAL( moveSelectedItemDown( int ) ), m_dataStore, SLOT( moveItemDown( int ) ) );

	QDockWidget *dock = new QDockWidget( tr( "Dataset View 2" ), this );
    datasetView2 = new QTableView( dock );
    datasetView2->setModel( m_dataStore );
    datasetView2->setSelectionModel( dock1->selectionModel() );
    dock->setWidget( datasetView2 );
    addDockWidget( Qt::LeftDockWidgetArea, dock );
    viewMenu->addAction( dock->toggleViewAction() );
}
