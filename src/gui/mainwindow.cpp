#include <QtGui/QtGui>

#include "../data/datastore.h"
#include "../data/enums.h"

#include "docks/datasetlistwidget.h"
#include "docks/datasetinfowidget.h"
#include "docks/datasetpropertywidget.h"
#include "docks/globalpropertywidget.h"
#include "docks/docknavglwidget.h"
#include "docks/dockcombinednavglwidget.h"
#include "docks/docksingleshwidget.h"

#include "widgets/navglwidget.h"
#include "widgets/glwidget.h"
#include "widgets/combinednavglwidget.h"
#include "widgets/toolbar.h"
#include "widgets/statusbar.h"
#include "widgets/shadereditwidget.h"

#include "mainwindow.h"

MainWindow::MainWindow( DataStore* dataStore, bool debug ) :
	QMainWindow(),
    m_dataStore( dataStore ),
    m_debug( debug )
{
	m_centralTabWidget = new QTabWidget( this );
	setCentralWidget( m_centralTabWidget );
	m_centralTabWidget->setTabsClosable( true );
	connect( m_centralTabWidget, SIGNAL( tabCloseRequested ( int ) ), this, SLOT( closeTab( int ) ) );

    mainGLWidget = new GLWidget( m_dataStore );
    m_centralTabWidget->addTab( mainGLWidget, "main gl" );

    createActions();
    createMenus();
    createToolBars();
    createDockWindows();

    // this needs to be done after the view is created
    m_toolsToolBar->setSelectionModel( m_datasetWidget->selectionModel() );

    createStatusBar();

    setWindowTitle( tr( "Fibernavigator 2" ) );

    setUnifiedTitleAndToolBarOnMac( true );

    QSettings settings;
    restoreGeometry( settings.value( "mainWindowGeometry" ).toByteArray() );

	restoreState( settings.value( "mainWindowState" ).toByteArray() );

	if ( settings.contains( "lastPath" ) )
	{
	    QString lastPath = settings.value( "lastPath" ).toString();
	    QModelIndex mi = m_dataStore->index( 0, FNGLOBAL_LAST_PATH );
	    m_dataStore->setData( mi, lastPath, Qt::UserRole );
	}

}

void MainWindow::closeEvent( QCloseEvent *event )
{
	QSettings settings;
	settings.setValue( "mainWindowGeometry", saveGeometry() );
	settings.setValue( "mainWindowState", saveState() );
	QModelIndex mi = m_dataStore->index( 0, FNGLOBAL_LAST_PATH );
	settings.setValue( "lastPath", m_dataStore->data( mi, Qt::UserRole ).toString() );
}

void MainWindow::print()
{
}

void MainWindow::open()
{
    QModelIndex mi = m_dataStore->index( 0, FNGLOBAL_LAST_PATH );
    QString fn = m_dataStore->data( mi, Qt::UserRole ).toString();

    QString fileName = QFileDialog::getOpenFileName( this, "Open File", fn );
    if ( !fileName.isEmpty() )
    {
    	m_dataStore->load( fileName );

    	QFileInfo fi( fileName );
    	QDir dir = fi.absoluteDir();
    	QString lastPath = dir.absolutePath();
        m_dataStore->setData( mi, lastPath, Qt::UserRole );
    }
}

void MainWindow::save()
{
    QModelIndex mi = m_dataStore->index( 0, FNGLOBAL_LAST_PATH );
    QString fn = m_dataStore->data( mi, Qt::UserRole ).toString();
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
                    m_dataStore->save( m_datasetWidget->getSelected(), fileName );
                    break;
                case QMessageBox::Cancel :
                    break;
            }
        }
        else
        {
            m_dataStore->save( m_datasetWidget->getSelected(), fileName );
        }

        QFileInfo fi( fileName );
        dir = fi.absoluteDir();
        QString lastPath = dir.absolutePath();
        m_dataStore->setData( mi, lastPath, Qt::UserRole );
    }
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

    addTabCombined = new QAction( tr( "Add Combined Slice Tab" ), this );
    addTabCombined->setStatusTip( tr( "Add a new tab to the central tab widget with all 3 slices." ) );
    connect( addTabCombined, SIGNAL(triggered()), this, SLOT( slotAddTabCombined() ) );

    addTabSagittal = new QAction( tr( "Add Sagittal Slice Tab" ), this );
    addTabSagittal->setStatusTip( tr( "Add a new tab to the central tab widget showing the sagittal slice." ) );
    connect( addTabSagittal, SIGNAL(triggered()), this, SLOT( slotAddTabSagittal() ) );

    addTabCoronal = new QAction( tr( "Add Coronal Slice Tab" ), this );
    addTabCoronal->setStatusTip( tr( "Add a new tab to the central tab widget showing the coronal slice." ) );
    connect( addTabCoronal, SIGNAL(triggered()), this, SLOT( slotAddTabCoronal() ) );

    addTabAxial = new QAction( tr( "Add Axial Slice Tab" ), this );
    addTabAxial->setStatusTip( tr( "Add a new tab to the central tab widget showing the axial slice." ) );
    connect( addTabAxial, SIGNAL(triggered()), this, SLOT( slotAddTabAxial() ) );

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

    tabMenu = menuBar()->addMenu( tr( "&Tabs" ) );
    tabMenu->addAction( addTabCombined );
    tabMenu->addAction( addTabSagittal );
    tabMenu->addAction( addTabCoronal );
    tabMenu->addAction( addTabAxial );

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
    editToolBar->setObjectName( "editToolbar");

    m_toolsToolBar = new ToolBar( tr( "Tools" ) );
    m_toolsToolBar->setModel( m_dataStore );
    addToolBar( m_toolsToolBar );
}

void MainWindow::createStatusBar()
{
    StatusBar* sbar = new StatusBar( this );
    sbar->setModel( m_dataStore );
    sbar->setSelectionModel( m_datasetWidget->selectionModel() );
    this->setStatusBar( sbar );
}

void MainWindow::createDockWindows()
{
	m_datasetWidget = new DatasetListWidget( this );
	m_datasetWidget->setModel( m_dataStore );
	addDockWidget( Qt::LeftDockWidgetArea, m_datasetWidget );
	viewMenu->addAction( m_datasetWidget->toggleViewAction() );

	connect( m_datasetWidget, SIGNAL( moveSelectedItemUp( int ) ), m_dataStore, SLOT( moveItemUp( int ) ) );
	connect( m_datasetWidget, SIGNAL( moveSelectedItemDown( int ) ), m_dataStore, SLOT( moveItemDown( int ) ) );
	connect( m_datasetWidget, SIGNAL( deleteSelectedItem( int ) ), m_dataStore, SLOT( deleteItem( int ) ) );

	DatasetPropertyWidget* dsProperties = new DatasetPropertyWidget( QString("dataset properties"), this );
    addDockWidget( Qt::LeftDockWidgetArea, dsProperties );
    dsProperties->setModel( m_dataStore );
    dsProperties->setSelectionModel( m_datasetWidget->selectionModel() );
    viewMenu->addAction( dsProperties->toggleViewAction() );

    GlobalPropertyWidget* globalProperties = new GlobalPropertyWidget( QString("global properties"), this );
    addDockWidget( Qt::LeftDockWidgetArea, globalProperties );
    globalProperties->setModel( m_dataStore );
    viewMenu->addAction( globalProperties->toggleViewAction() );

	DatasetInfoWidget *dsInfo = new DatasetInfoWidget( tr( "Dataset Info Table" ), this );
	addDockWidget( Qt::BottomDockWidgetArea, dsInfo );
	dsInfo->setModel( m_dataStore );
    dsInfo->setSelectionModel( m_datasetWidget->selectionModel() );
    viewMenu->addAction( dsInfo->toggleViewAction() );

    DockNavGLWidget* nav1 = new DockNavGLWidget( m_dataStore, QString("axial"), 2, this, mainGLWidget );
    addDockWidget( Qt::RightDockWidgetArea, nav1 );
    viewMenu->addAction( nav1->toggleViewAction() );

    DockNavGLWidget* nav2 = new DockNavGLWidget( m_dataStore, QString( "sagittal" ), 0, this, mainGLWidget );
    addDockWidget( Qt::RightDockWidgetArea, nav2 );
    viewMenu->addAction( nav2->toggleViewAction() );

    DockNavGLWidget* nav3 = new DockNavGLWidget( m_dataStore, QString( "coronal" ), 1, this, mainGLWidget );
    addDockWidget( Qt::RightDockWidgetArea, nav3 );
    viewMenu->addAction( nav3->toggleViewAction() );

    DockCombinedNavGLWidget* nav4 = new DockCombinedNavGLWidget( m_dataStore, QString( "combined" ), this, mainGLWidget );
    addDockWidget( Qt::RightDockWidgetArea, nav4 );
    viewMenu->addAction( nav4->toggleViewAction() );

    DockSingleSHWidget* sshw = new DockSingleSHWidget( m_dataStore, QString( "single sh" ), this, mainGLWidget );
    addDockWidget( Qt::RightDockWidgetArea, sshw );
    viewMenu->addAction( sshw->toggleViewAction() );
}

void MainWindow::closeTab( int index )
{
    if ( index > 0 )
    {
        m_centralTabWidget->removeTab( index );
    }
}

void MainWindow::slotAddTabCombined()
{
    CombinedNavGLWidget* combNav = new CombinedNavGLWidget( m_dataStore, QString( "combined" ), this, mainGLWidget );
    m_centralTabWidget->addTab( combNav, "slices" );
    m_centralTabWidget->setCurrentWidget( combNav );
}

void MainWindow::slotAddTabSagittal()
{
    NavGLWidget* glWidget = new NavGLWidget( m_dataStore, tr("sagittal"), 0, this, mainGLWidget );
    m_centralTabWidget->addTab( glWidget, "sagittal" );
    m_centralTabWidget->setCurrentWidget( glWidget );
}

void MainWindow::slotAddTabCoronal()
{
    NavGLWidget* glWidget = new NavGLWidget( m_dataStore, tr("coronal"), 1, this, mainGLWidget );
    m_centralTabWidget->addTab( glWidget, "coronal" );
    m_centralTabWidget->setCurrentWidget( glWidget );
}

void MainWindow::slotAddTabAxial()
{
    NavGLWidget* glWidget = new NavGLWidget( m_dataStore, tr("axial"), 2, this, mainGLWidget );
    m_centralTabWidget->addTab( glWidget, "axial" );
    m_centralTabWidget->setCurrentWidget( glWidget );
}

void MainWindow::slotToggleAxialSlice()
{
    QModelIndex mi = m_dataStore->index( 0, FNGLOBAL_SHOW_AXIAL );
    m_dataStore->setData( mi, showAxialAct->isChecked(), Qt::UserRole );
}

void MainWindow::slotToggleCoronalSlice()
{
    QModelIndex mi = m_dataStore->index( 0, FNGLOBAL_SHOW_CORONAL );
    m_dataStore->setData( mi, showCoronalAct->isChecked(), Qt::UserRole );
}

void MainWindow::slotToggleSagittalSlice()
{
    QModelIndex mi = m_dataStore->index( 0, FNGLOBAL_SHOW_SAGITTAL );
    m_dataStore->setData( mi, showSagittalAct->isChecked(), Qt::UserRole );
}

void  MainWindow::slotStandardAxialView()
{
    QModelIndex mi = m_dataStore->index( 0, FNGLOBAL_VIEW );
    m_dataStore->setData( mi, FN_AXIAL, Qt::UserRole );
    mainGLWidget->setView( FN_AXIAL );
}

void  MainWindow::slotStandardCoronalView()
{
    QModelIndex mi = m_dataStore->index( 0, FNGLOBAL_VIEW );
    m_dataStore->setData( mi, FN_CORONAL, Qt::UserRole );
    mainGLWidget->setView( FN_CORONAL );
}

void  MainWindow::slotStandardSagittalView()
{
    QModelIndex mi = m_dataStore->index( 0, FNGLOBAL_VIEW );
    m_dataStore->setData( mi, FN_SAGITTAL, Qt::UserRole );
    mainGLWidget->setView( FN_SAGITTAL );
}

void MainWindow::slotToggleShaderEdit()
{
    m_shaderEditWidget = new ShaderEditWidget( this );
    m_centralTabWidget->addTab( m_shaderEditWidget, "shader edit" );
    m_centralTabWidget->setCurrentWidget( m_shaderEditWidget );
}
