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
    // 1 parameters
    DELAY,
    // 2 parameters
    // 3 parameters
    SET_CAMERA,  // #1 position #2 lookat #3 up
    // 4 parameters
    INTERPOLATE_CAMERA_TO
    // 5 parameters
};

#define NUM_SCRIPT_COMMANDS 4

struct Script2String
{
    static QString s( ScriptCommand command )
    {
        switch ( command )
        {
            // 0 parameters
            case ScriptCommand::NONE: return QString( "none" ); break;
            // 1 parameters
            case ScriptCommand::DELAY: return QString( "delay" ); break;
            // 2 parameters
            // 3 parameters
            case ScriptCommand::SET_CAMERA: return QString( "set camera" ); break;
            // 4 parameters
            case ScriptCommand::INTERPOLATE_CAMERA_TO: return QString( "interpolate camera to" ); break;
            // 5 parameters
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

    GLWidget* m_glWidget;

    QList<QList<QVariant> >m_script;

    QVBoxLayout* m_scriptLayout;

    int m_runScript;
    int m_currentCommandLine;

    CheckboxWithLabel* m_contRunning;
    CheckboxWithLabel* m_screenshotEach;
    SliderWithEditInt2* m_delay;

    QList<QVariant>m_currentCamera;
    QList<QVariant>m_interpolatedCamera;
    int m_interpolateSteps;
    int m_currentInterpolateStep;

public slots:


private slots:
    void loadScript();
    void saveScript();
    void run( bool checked );
    void run();

    void interpolateCamera();

    void addCommand();
    void copyCamera();

    void commandChanged( int line, int command );
    void slotEditChanged( QString, int );
    void deleteCommand( int row );

signals:
    void enable( bool e, int id );
    void editChanged( QString text, int id );
};

#endif /* SCRIPTWIDGET_H_ */
