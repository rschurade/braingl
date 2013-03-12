/*
 * mainwindow.h
 *
 *  Created on: May 03, 2012
 *      Author: Ralph Schurade
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../data/enums.h"

#include <QtGui/QMainWindow>
#include <QtCore/QSettings>

class QAction;
class QListView;
class QMenu;
class QTabWidget;
class QTableView;

class DatasetListWidget;
class ROIWidget;
class ShaderEditWidget;

class DataStore;
class GlobalPropertyModel;
class ROIModel;
class GLWidget;
class ToolBar;

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    MainWindow( DataStore* dataStore, GlobalPropertyModel* globalProps, ROIModel* roiModel,  bool debug = false );

    void closeEvent( QCloseEvent *event );
    void load( QString fileName );

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();

    void saveSettings();
    void loadSettings();
    void loadSetting( QSettings &settings, Fn::Global setting );
    void loadColormaps();


    void setCurrentFile( const QString &fileName );
    void updateRecentFileActions();
    QString strippedName( const QString &fullFileName );

    DataStore* m_dataStore;
    GlobalPropertyModel* m_globalProps;
    ROIModel* m_roiModel;

    bool m_debug;

    QMainWindow* m_centralWidget;

    GLWidget* mainGLWidget;
    DatasetListWidget* m_datasetWidget;
    ROIWidget* m_roiWidget;
    ShaderEditWidget* m_shaderEditWidget;

    QMenu* fileMenu;
    QMenu* optionMenu;
    QMenu* viewMenu;
    QMenu* tabMenu;
    QMenu* helpMenu;
    QMenu *recentFilesMenu;

    QToolBar* fileToolBar;
    QToolBar* editToolBar;
    ToolBar* m_toolsToolBar;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
    QAction *separatorAct;
    QString curFile;

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

    QAction* screenshotAct;
    QAction* resetSettingsAct;
    QAction* aboutAct;
    QAction* aboutQtAct;

    QAction* toggleShaderEditAct;

    QAction* standardViewAxialAct;
    QAction* standardViewCoronalAct;
    QAction* standardViewSagittalAct;

    QAction* newSelectionBoxAct;

    QAction* dilbertAct;

    static int screenshotNumber;

private slots:
    void open();
    void openRecentFile();
    void save();
    void print();
    void undo();
    void about();
    void screenshot();
    void resetSettings();

    void slotToggleAxialSlice();
    void slotToggleCoronalSlice();
    void slotToggleSagittalSlice();

    void slotStandardAxialView();
    void slotStandardCoronalView();
    void slotStandardSagittalView();
    void slotToggleShaderEdit();

    void slotDilbert();

    void slotRenderCrosshairs( bool value );
};

#endif
