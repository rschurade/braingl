#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QAction;
class QListView;
class QMenu;
class QTabWidget;
class QTableView;

class DatasetListWidget;
class ShaderEditWidget;

class DataStore;
class GlobalPropertyModel;
class SelectionBoxModel;
class GLWidget;
class ToolBar;

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    MainWindow( DataStore* dataStore, GlobalPropertyModel* globalProps, SelectionBoxModel* selBoxModel,  bool debug = false );

    void closeEvent( QCloseEvent *event );

private slots:
    void open();
    void save();
    void print();
    void undo();
    void about();
    void closeTab( int index );
    void slotAddTabCombined();
    void slotAddTabSagittal();
    void slotAddTabCoronal();
    void slotAddTabAxial();

    void slotToggleAxialSlice();
    void slotToggleCoronalSlice();
    void slotToggleSagittalSlice();

    void slotStandardAxialView();
    void slotStandardCoronalView();
    void slotStandardSagittalView();
    void slotToggleShaderEdit();

    void slotToggleDockTitles( bool value );
    void slotRenderCrosshairs( bool value );

    void slotNewSelectionBox();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();

    DataStore* m_dataStore;
    GlobalPropertyModel* m_globalProps;
    SelectionBoxModel* m_selBoxModel;

    bool m_debug;

    GLWidget* mainGLWidget;
    DatasetListWidget* m_datasetWidget;
    QTabWidget* m_centralTabWidget;
    ShaderEditWidget* m_shaderEditWidget;

    QMenu* fileMenu;
    QMenu* optionMenu;
    QMenu* viewMenu;
    QMenu* tabMenu;
    QMenu* helpMenu;

    QToolBar* fileToolBar;
    QToolBar* editToolBar;
    ToolBar* m_toolsToolBar;

    QAction* newLetterAct;
    QAction* openAct;
    QAction* saveAct;
    QAction* printAct;
    QAction* quitAct;

    QAction* lockDockTitlesAct;
    QAction* renderCrosshairsAct;

    QAction* showAxialAct;
    QAction* showCoronalAct;
    QAction* showSagittalAct;

    QAction* aboutAct;
    QAction* aboutQtAct;

    QAction* toggleShaderEditAct;

    QAction* addTabCombined;
    QAction* addTabSagittal;
    QAction* addTabCoronal;
    QAction* addTabAxial;

    QAction* standardViewAxialAct;
    QAction* standardViewCoronalAct;
    QAction* standardViewSagittalAct;

    QAction* newSelectionBoxAct;
};

#endif
