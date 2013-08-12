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
    INCREMENT_PROPERTY

};

#define NUM_SCRIPT_COMMANDS 8

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

        }
        return QString( "command doesn't exist" );
    }
};

class GLWidget;
class CheckboxWithLabel;
class SliderWithEditInt2;

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

    void addGlobalSelect( QHBoxLayout* layout, int id, int selected );
    void addPropertySelect( QHBoxLayout* layout, int id, int selected, int dataset );
    void addEdit( QHBoxLayout* layout, int startId, int count );

    GLWidget* m_glWidget;

    QList<QList<QVariant> >m_script;

    QVBoxLayout* m_scriptLayout;

    QPushButton* m_runButton;

    int m_runScript;
    int m_currentCommandLine;

    CheckboxWithLabel* m_contRunning;
    CheckboxWithLabel* m_screenshotEach;
    SliderWithEditInt2* m_delay;

    QList<QVariant>m_currentCamera;
    QList<QVariant>m_interpolatedCamera;
    int m_interpolateSteps;
    int m_currentInterpolateStep;

    int m_lastGlobal;
    float m_stepSize;
    int m_targetStep;
    int m_currentStep;

    int m_currentDelay;
    int m_targetDelay;

    int m_lastDataset;
    int m_lastProperty;

public slots:


private slots:
    void loadScript();
    void saveScript();
    void run( bool checked );
    void run();

    void interpolateCamera();
    void slotDelay();
    void slotIncrementGlobal();
    void slotIncrementProperty();

    void copyCamera();

    void commandChanged( int line, int command );
    void slotEditChanged( QString, int );
    void slotGlobalSelectChanged( int line, int prop, int data );
    void slotPropertySelectChanged( int line, int prop, int data );
    void deleteCommand( int row );
    void insertCommand( int row );

signals:
    void enable( bool e, int id );
    void editChanged( QString text, int id );
    void selectChanged( int index, int id );
    void screenshot();
};

#endif /* SCRIPTWIDGET_H_ */
