#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QAction;
class QListView;
class DatasetListWidget;
class QTabWidget;
class QTableView;
class QMenu;

class DataStore;
class GLWidget;
class ToolBar;

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    MainWindow( DataStore* dataStore );

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

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();

    DataStore* m_dataStore;

    GLWidget* mainGLWidget;

    DatasetListWidget* m_datasetView;

    QTabWidget* m_centralTabWidget;

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

    QAction* addTabCombined;
    QAction* addTabSagittal;
    QAction* addTabCoronal;
    QAction* addTabAxial;
};

#endif
