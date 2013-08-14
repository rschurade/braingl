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
    INTERPOLATE_CAMERA,
    SET_GLOBAL,
    INCREMENT_GLOBAL,
    SET_PROPERTY,
    INCREMENT_PROPERTY,
    SET_ARCBALL,
    INTERPOLATE_ARCBALL,
    BEGIN_LOOP,
    END_LOOP,
    BEGIN_BLOCK,
    END_BLOCK,
    COMMENT

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
            case ScriptCommand::INTERPOLATE_CAMERA: return QString( "interpolate camera" ); break;
            case ScriptCommand::SET_GLOBAL: return QString( "set global" ); break;
            case ScriptCommand::INCREMENT_GLOBAL: return QString( "increment global" ); break;
            case ScriptCommand::SET_PROPERTY: return QString( "set property" ); break;
            case ScriptCommand::INCREMENT_PROPERTY: return QString( "increment property" ); break;
            case ScriptCommand::SET_ARCBALL: return QString( "set arcball" ); break;
            case ScriptCommand::INTERPOLATE_ARCBALL: return QString( "interpolate arcball" ); break;
            case ScriptCommand::BEGIN_LOOP: return QString( "begin loop" ); break;
            case ScriptCommand::END_LOOP: return QString( "end loop" ); break;
            case ScriptCommand::BEGIN_BLOCK: return QString( "begin block" ); break;
            case ScriptCommand::END_BLOCK: return QString( "end block" ); break;
            case ScriptCommand::COMMENT: return QString( "comment" ); break;
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
    QVector3D string2Vector3D( QString string );

    void addGlobalSelect( QHBoxLayout* layout, int id, int selected, bool active );
    void addPropertySelect( QHBoxLayout* layout, int id, int selected, int dataset, bool active );
    void addEdit( QHBoxLayout* layout, int startId, int count );
    void addStretch( QHBoxLayout* layout, int count );

    GLWidget* m_glWidget;

    QList<QList<QVariant> >m_script;

    QVBoxLayout* m_scriptLayout;

    QScrollArea* m_scrollArea;
    QPushButton* m_runButton;

    int m_runScript;
    int m_currentCommandLine;

    CheckboxWithLabel* m_contRunning;
    CheckboxWithLabel* m_screenshotEach;
    SliderWithEditInt* m_delay;

    QList<QVariant>m_currentCamera;
    QList<QVariant>m_interpolatedCamera;

    int m_lastGlobal;
    int m_lastDataset;
    int m_lastProperty;

    float m_stepSize;
    int m_targetStep;
    int m_currentStep;
    int m_loopCount;
    int m_loopBegin;
    bool m_inBlock;

    QQuaternion m_currentRot;
    QQuaternion m_targetRot;
    float m_currentZoom;
    float m_targetZoom;
    float m_currentMoveX;
    float m_currentMoveY;
    float m_targetMoveX;
    float m_targetMoveY;

public slots:


private slots:
    void loadScript();
    void saveScript();
    void run( bool checked );
    void run();

    void slotInterpolateCamera();
    void slotInterpolateArcball();

    void copyCamera();

    void commandChanged( int line, int command );
    void slotEditChanged( QString, int );
    void slotGlobalSelectChanged( int line, int prop, int data );
    void slotPropertySelectChanged( int line, int prop, int data );
    void deleteCommand( int row );
    void insertCommand( int row );
    void moveScrollBarToBottom( int min, int max );
    void resetScript();
    void slotCheckboxChanged( int, int );

signals:
    void enable( bool e, int id );
    void enable2( bool e, int id );
    void editChanged( QString text, int id );
    void selectChanged( int index, int id );
    void checkBoxChanged( int state, int id );
    void screenshot();
};

#endif /* SCRIPTWIDGET_H_ */
