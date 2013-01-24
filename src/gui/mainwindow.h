#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QAction;
class QListView;
class DatasetListWidget;
class QTabWidget;
class QTableView;
class ShaderEditWidget;
class QMenu;

class DataStore;
class GLWidget;
class ToolBar;

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    MainWindow( DataStore* dataStore, bool debug = false );

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

    void slotToggleDockTitles();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();

    DataStore* m_dataStore;

    bool m_debug;
    bool m_showDockTitles;

    GLWidget* mainGLWidget;
    DatasetListWidget* m_datasetWidget;
    QTabWidget* m_centralTabWidget;
    ShaderEditWidget* m_shaderEditWidget;

    QMenu* fileMenu;
    QMenu* editMenu;
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
    QAction* undoAct;
    QAction* aboutAct;
    QAction* aboutQtAct;
    QAction* quitAct;
    QAction* showAxialAct;
    QAction* showCoronalAct;
    QAction* showSagittalAct;
    QAction* toggleDockTitlesAct;

    QAction* toggleShaderEditAct;

    QAction* addTabCombined;
    QAction* addTabSagittal;
    QAction* addTabCoronal;
    QAction* addTabAxial;

    QAction* standardViewAxialAct;
    QAction* standardViewCoronalAct;
    QAction* standardViewSagittalAct;
};

#endif
