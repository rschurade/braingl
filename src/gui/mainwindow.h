#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class QAction;
class QListView;
class DatasetViewWidget;
class QTableView;
class QMenu;

class DataStore;
class GLWidget;

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

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();

    GLWidget *mainGLWidget;

    DatasetViewWidget *datasetView;
    //QListView *datasetView;
    QTableView *datasetView2;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newLetterAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *printAct;
    QAction *undoAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *quitAct;

    DataStore* m_dataStore;
};

#endif
