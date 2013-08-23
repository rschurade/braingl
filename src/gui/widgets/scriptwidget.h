/*
 * scriptwidget.h
 *
 *  Created on: 11.08.2013
 *      Author: Ralph
 */

#ifndef SCRIPTWIDGET_H_
#define SCRIPTWIDGET_H_

#include <QtGui>

enum class ScriptCommand : int
{
    // 0 parameters
    NONE,
    DELAY,
    SET_CAMERA,  // #1 position #2 lookat #3 up
    SET_GLOBAL,
    INC_GLOBAL,
    SET_PROPERTY,
    INC_PROPERTY,
    SET_ARCBALL,
    BEGIN_LOOP,
    END_LOOP,
    BEGIN_BLOCK,
    END_BLOCK,
    COMMENT,
    SET_ROI_PROPERTY,
    INC_ROI_PROPERTY
};

#define NUM_SCRIPT_COMMANDS 15

struct Script2String
{
    static QString s( ScriptCommand command )
    {
        switch ( command )
        {
            case ScriptCommand::NONE: return QString( "none" ); break;
            case ScriptCommand::DELAY: return QString( "delay" ); break;
            case ScriptCommand::SET_CAMERA: return QString( "set camera" ); break;
            case ScriptCommand::SET_GLOBAL: return QString( "set global" ); break;
            case ScriptCommand::INC_GLOBAL: return QString( "increment global" ); break;
            case ScriptCommand::SET_PROPERTY: return QString( "set property" ); break;
            case ScriptCommand::INC_PROPERTY: return QString( "increment property" ); break;
            case ScriptCommand::SET_ARCBALL: return QString( "set arcball" ); break;
            case ScriptCommand::BEGIN_LOOP: return QString( "begin loop" ); break;
            case ScriptCommand::END_LOOP: return QString( "end loop" ); break;
            case ScriptCommand::BEGIN_BLOCK: return QString( "begin block" ); break;
            case ScriptCommand::END_BLOCK: return QString( "end block" ); break;
            case ScriptCommand::COMMENT: return QString( "comment" ); break;
            case ScriptCommand::SET_ROI_PROPERTY: return QString( "set roi property" ); break;
            case ScriptCommand::INC_ROI_PROPERTY: return QString( "increment roi property" ); break;
        }
        return QString( "command doesn't exist" );
    }
};

class GLWidget;
class CheckboxWithLabel;
class SliderWithEditInt;

class ScriptWidget : public QWidget
{
    Q_OBJECT

public:
    ScriptWidget( GLWidget* glWidget, QWidget* parent );
    virtual ~ScriptWidget();

private:
    void initLayout();
    QWidget* buildScriptLayout();
    QHBoxLayout* addWidgetLine( int i, QList<QVariant> &line, bool &inLoop, bool &inBlock );

    QVector3D string2Vector3D( QString string );

    void addGlobalSelect( QHBoxLayout* layout, int id, int selected, bool active );
    void addPropertySelect( QHBoxLayout* layout, int id, int selected, int dataset, bool active );
    void addRoiPropertySelect( QHBoxLayout* layout, int id, int selected, int branch, int row, bool active );
    void addEdit( QHBoxLayout* layout, int startId, int count );
    void addStretch( QHBoxLayout* layout, int count );

    bool lastIsNone();

    GLWidget* m_glWidget;
    QWidget* m_widgetToEnsureVisible;
    CheckboxWithLabel* m_contRunning;
    CheckboxWithLabel* m_screenshotEach;
    CheckboxWithLabel* m_copyCamera;
    CheckboxWithLabel* m_buildRange;
    SliderWithEditInt* m_delay;
    SliderWithEditInt* m_beginSlider;
    SliderWithEditInt* m_endSlider;

    QList<QList<QVariant> >m_script;

    QScrollArea* m_scrollArea;
    QWidget* m_scriptPanel;
    QPushButton* m_runButton;
    QPushButton* m_pauseButton;

    int m_lastInsertedLine;
    int m_runScript;
    int m_currentCommandLine;

    QList<QVariant>m_currentCamera;
    QList<QVariant>m_interpolatedCamera;
    QQuaternion m_currentRot;
    QQuaternion m_targetRot;

    int m_lastGlobal;
    int m_lastDataset;
    int m_lastProperty;
    int m_lastRoiBranch;
    int m_lastRoiRow;

    int m_loopCount;
    int m_totalLoops;
    int m_loopBegin;
    bool m_inBlock;
    bool m_inLoop;
    bool m_render;
    bool m_paused;


    float m_currentZoom;
    float m_targetZoom;
    float m_currentMoveX;
    float m_currentMoveY;
    float m_targetMoveX;
    float m_targetMoveY;

    float m_currentValue;
    float m_targetValue;
    QList< QList<QVariant> >m_loopList;

public slots:
    void loadScript( QString fileName, bool append );
    void saveScript( QString fileName );
    void slotCameraChanged();
    void slotCopyCamera( int mode );

private slots:
    void rebuild();
    void loadScript();
    void appendScript();
    void saveScript();
    void run( bool checked );
    void run();
    void pause( bool checked );

    void commandChanged( int line, int command );
    void slotEditChanged( QString, int );
    void slotGlobalSelectChanged( int line, int prop, int data );
    void slotPropertySelectChanged( int line, int prop, int data );
    void deleteCommand( int row );
    void insertCommand( int row );
    void moveCommandUp( int row );
    void moveCommandDown( int row );
    void moveScrollBarToBottom( int min, int max );
    void resetScript();
    void slotCheckboxChanged( int, int );
    void slotKeyPressed( int, Qt::KeyboardModifiers mods );
    void slotRangeChanged();
    void slotCopyCameraChanged();



signals:
    void enable( bool e, int id );
    void enable2( bool e, int id );
    void editChanged( QString text, int id );
    void selectChanged( int index, int id );
    void checkBoxChanged( int state, int id );
    void screenshot();
    void hideIndicator( bool state, int id );
};

#endif /* SCRIPTWIDGET_H_ */
