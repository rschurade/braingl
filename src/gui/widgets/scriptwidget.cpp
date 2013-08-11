/*
 * scriptwidget.cpp
 *
 *  Created on: 11.08.2013
 *      Author: Ralph
 */

#include "scriptwidget.h"
#include "glwidget.h"
#include "../gl/camera.h"

#include "controls/comboboxid.h"
#include "controls/lineeditid.h"
#include "controls/checkboxwithlabel.h"
#include "controls/sliderwitheditint2.h"
#include "controls/pushbuttonwithid.h"

#include "../../data/enums.h"
#include "../../data/models.h"

#include <QDebug>

ScriptWidget::ScriptWidget( GLWidget* glWidget, QWidget* parent ) :
    m_glWidget( glWidget ),
    m_runScript( false ),
    m_currentCommandLine( 0 )
{
    QList<QVariant> command;
    command.push_back( (int)ScriptCommand::NONE );
    m_script.push_back( command );

    initLayout();
}

ScriptWidget::~ScriptWidget()
{
}

void ScriptWidget::initLayout()
{
    QWidget* widget1 = buildScriptLayout();
    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidget( widget1 );

    QVBoxLayout* layout2 = new QVBoxLayout();
    QHBoxLayout* buttons1 = new QHBoxLayout();

    QPushButton* lbutton = new QPushButton( "Load", this );
    buttons1->addWidget( lbutton );
    connect( lbutton, SIGNAL( clicked() ), this, SLOT( loadScript() ) );

    QPushButton* sbutton = new QPushButton( "Save", this );
    buttons1->addWidget( sbutton );
    connect( sbutton, SIGNAL( clicked() ), this, SLOT( saveScript() ) );

    QPushButton* rbutton = new QPushButton( "Run", this );
    rbutton->setCheckable( true );
    buttons1->addWidget( rbutton );
    connect( rbutton, SIGNAL( toggled( bool ) ), this, SLOT( run( bool ) ) );

    buttons1->addStretch();


    QHBoxLayout* buttons2 = new QHBoxLayout();
//
//    QPushButton* addbutton = new QPushButton( "Add", this );
//    buttons2->addWidget( addbutton );
//    connect( addbutton, SIGNAL( clicked() ), this, SLOT( addCommand() ) );
//
//    QPushButton* cambutton = new QPushButton( "Copy Camera", this );
//    buttons2->addWidget( cambutton );
//    connect( cambutton, SIGNAL( clicked() ), this, SLOT( copyCamera() ) );
//
//
    m_contRunning = new CheckboxWithLabel( "continous", -1, this );
    buttons2->addWidget( m_contRunning );

    m_screenshotEach = new CheckboxWithLabel( "screenshot each step", -1, this );
    buttons2->addWidget( m_screenshotEach );

    m_delay = new SliderWithEditInt2( this );
    m_delay->setMin( 0 );
    m_delay->setMax( 1000 );
    m_delay->setValue( 25 );

    buttons2->addStretch();
    buttons2->addWidget( m_delay );



    layout2->addLayout( buttons2 );
    layout2->addLayout( buttons1 );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( scrollArea );
    layout->addLayout( layout2 );

    this->setLayout( layout );
    this->setMinimumSize( 600, 600 );
}

QWidget* ScriptWidget::buildScriptLayout()
{
    QGroupBox *groupBox = new QGroupBox( this );
    m_scriptLayout = new QVBoxLayout();
    groupBox->setLayout( m_scriptLayout );

    for ( int i = 0; i < m_script.size(); ++i )
    {

        QHBoxLayout* layout = new QHBoxLayout();
        QList<QVariant>command = m_script[i];

        PushButtonWithId* deleteButton = new PushButtonWithId( "-", i );
        layout->addWidget( deleteButton );
        deleteButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( deleteButton, SIGNAL( signalClicked( int ) ), this, SLOT( deleteCommand( int ) ) );

        ComboBoxID* select = new ComboBoxID( i * 10, this );
        for ( int j = 0; j < NUM_SCRIPT_COMMANDS; ++j )
        {
            select->insertItem( j, Script2String::s( (ScriptCommand)j ) );
        }

        layout->addWidget( select );
        select->setCurrentIndex( command[0].toInt() );
        connect( select, SIGNAL( currentIndexChanged( int, int) ), this, SLOT( commandChanged( int, int ) ) );

        for ( int k = 1; k < 6; ++k )
        {
            LineEditID* edit = new LineEditID( i * 10 + k, this );
            connect( this, SIGNAL( enable( bool, int ) ), edit, SLOT( setEnabled2( bool, int ) ) );
            connect( this, SIGNAL( editChanged( QString, int ) ), edit, SLOT( setText2( QString, int ) ) );
            connect( edit, SIGNAL( textEdited2( QString, int ) ), this, SLOT( slotEditChanged( QString, int ) ) );
            layout->addWidget( edit );
        }



        switch( (ScriptCommand)( command[0].toInt() ) )
        {
            case ScriptCommand::NONE:
                emit( enable( false, i * 10 + 1 ) );
                emit( enable( false, i * 10 + 2 ) );
                emit( enable( false, i * 10 + 3 ) );
                emit( enable( false, i * 10 + 4 ) );
                emit( enable( false, i * 10 + 5 ) );

                break;
            case ScriptCommand::DELAY:
            {
                emit( editChanged( command[1].toString(), i * 10 + 1 ) );
                emit( enable( false, i * 10 + 2 ) );
                emit( enable( false, i * 10 + 3 ) );
                emit( enable( false, i * 10 + 4 ) );
                emit( enable( false, i * 10 + 5 ) );
                break;
            }
            case ScriptCommand::SET_CAMERA:
            {
                QVector3D vec = command[1].value<QVector3D>();
                emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 1 ) );
                vec = command[2].value<QVector3D>();
                emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 2 ) );
                vec = command[3].value<QVector3D>();
                emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 3 ) );
                emit( enable( false, i * 10 + 4 ) );
                emit( enable( false, i * 10 + 5 ) );
                break;
            }
            case ScriptCommand::INTERPOLATE_CAMERA_TO:
            {
                QVector3D vec = command[1].value<QVector3D>();
                emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 1 ) );
                vec = command[2].value<QVector3D>();
                emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 2 ) );
                vec = command[3].value<QVector3D>();
                emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 3 ) );
                int steps = command[4].toInt();
                emit( editChanged( QString::number( steps ), i * 10 + 4 ) );
                emit( enable( false, i * 10 + 5 ) );
                break;
            }
        }

        layout->addStretch();
        m_scriptLayout->addLayout( layout );
    }
    m_scriptLayout->addStretch();

    return groupBox;
}

void ScriptWidget::loadScript()
{
    QString fn = Models::g()->data( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ) ).toString();
    QString fileName = QFileDialog::getOpenFileName( this, "Open File", fn );
    QSettings settings( fileName, QSettings::IniFormat );

    if ( settings.contains( "appName" ) && settings.contains( "fileType" ) )
    {
        if( ( settings.value( "appName").toString() != "braingl" ) || ( settings.value( "fileType").toString() != "script" ) )
        {
            qDebug() << "*** ERROR *** No braingl script file.";
            return;
        }
    }
    else
    {
        qDebug() << "*** ERROR *** No braingl script file.";
        return;
    }

    int size = 0;
    if ( settings.contains( "size" ) )
    {
        size = settings.value( "size" ).toInt();
    }
    else
    {
        qDebug() << "*** ERROR *** No size entry in script file.";
    }
    m_script.clear();
    for ( int i = 0; i < size; ++i )
    {
        if( settings.contains( QString::number( i ) ) )
        {
            QList<QVariant> commandLine = settings.value( QString::number( i ) ).toList();
            m_script.push_back( commandLine );
        }
        else
        {
            qDebug() << "*** ERROR *** script file corrupt.";
        }
    }
    delete this->layout();
    initLayout();
}

void ScriptWidget::saveScript()
{
    QString fn = Models::g()->data( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ) ).toString();
    QString fileName = QFileDialog::getSaveFileName( this, "Save File", fn );
    QSettings settings( fileName, QSettings::IniFormat );
    settings.setValue( "appName", "braingl" );
    settings.setValue( "version", "0.7.0" );
    settings.setValue( "fileType", "script" );
    settings.setValue( "size", m_script.size() );
    for ( int i = 0; i < m_script.size(); ++i )
    {
        settings.setValue( QString::number( i ), m_script[i] );
    }
}

void ScriptWidget::run( bool checked )
{
    if ( checked )
    {
        qDebug() << "start script";
        m_currentCommandLine = 0;
        m_runScript = true;
        run();
    }
    else
    {
        qDebug() << "stop script";
        m_runScript = false;
    }

}

void ScriptWidget::run()
{
    if ( !m_runScript )
    {
        return;
    }

    if ( m_currentCommandLine >= m_script.size() )
    {
        if( m_contRunning->checked() )
        {
            m_currentCommandLine = 0;
        }
        else
        {
            return;
        }
    }

    QList<QVariant> line = m_script[m_currentCommandLine++];

    int delay = m_delay->getValue();
    int command = line[0].toInt();

    QList<QVariant>camera = m_glWidget->getCamera()->getState();

    //qDebug() << Script2String::s( (ScriptCommand)command );

    switch( (ScriptCommand)( command ) )
    {
        case ScriptCommand::NONE:
            break;
        case ScriptCommand::DELAY:
        {
            delay += line[1].toInt();
            break;
        }
        case ScriptCommand::SET_CAMERA:
        {
            camera[0] = line[1];
            camera[1] = line[2];
            camera[2] = line[3];
            m_glWidget->getCamera()->setState( camera );
            break;
        }
        case ScriptCommand::INTERPOLATE_CAMERA_TO:
        {
            m_currentCamera = camera;
            m_interpolatedCamera.clear();
            m_interpolatedCamera.push_back( line[1] );
            m_interpolatedCamera.push_back( line[2] );
            m_interpolatedCamera.push_back( line[3] );
            m_interpolateSteps = line[4].toInt();
            m_currentInterpolateStep = 0;
            interpolateCamera();
            return;
            break;
        }
    }
    Models::g()->submit();

    QTimer::singleShot( delay, this, SLOT( run() ) );
}

void ScriptWidget::interpolateCamera()
{
    if ( !m_runScript )
    {
        return;
    }

    int delay = m_delay->getValue();
    if ( ++m_currentInterpolateStep > m_interpolateSteps )
    {
        QTimer::singleShot( delay, this, SLOT( run() ) );
        return;
    }

    QVector3D pos1 = m_currentCamera[0].value<QVector3D>();
    QVector3D pos2 = m_interpolatedCamera[0].value<QVector3D>();

    QVector3D look1 = m_currentCamera[1].value<QVector3D>();
    QVector3D look2 = m_interpolatedCamera[1].value<QVector3D>();

    QVector3D up1 = m_currentCamera[2].value<QVector3D>();
    QVector3D up2 = m_interpolatedCamera[2].value<QVector3D>();

    float div = (float)m_currentInterpolateStep / (float)m_interpolateSteps;

    QVector3D pos = div * pos2 + ( 1.0f - div ) * pos1;
    QVector3D look = div * look2 + ( 1.0f - div ) * look1;
    QVector3D up = div * up2 + ( 1.0f - div ) * up1;

    QList<QVariant>newCam;
    newCam.push_back( pos );
    newCam.push_back( look );
    newCam.push_back( up );
    m_glWidget->getCamera()->setState( newCam );
    Models::g()->submit();

    QTimer::singleShot( delay, this, SLOT( interpolateCamera() ) );
}

void ScriptWidget::commandChanged( int line, int command )
{
    line /= 10;
    QList<QVariant> commandLine;
    commandLine.push_back( command );

    QList<QVariant> camera = m_glWidget->getCamera()->getState();

    emit( enable( false, line * 10 + 1 ) );
    emit( enable( false, line * 10 + 2 ) );
    emit( enable( false, line * 10 + 3 ) );
    emit( enable( false, line * 10 + 4 ) );
    emit( enable( false, line * 10 + 5 ) );

    emit( editChanged( "", line * 10 + 1 ) );
    emit( editChanged( "", line * 10 + 2 ) );
    emit( editChanged( "", line * 10 + 3 ) );
    emit( editChanged( "", line * 10 + 4 ) );
    emit( editChanged( "", line * 10 + 5 ) );

    switch( (ScriptCommand)( command ) )
    {
        case ScriptCommand::NONE:
            break;
        case ScriptCommand::DELAY:
        {
            commandLine.push_back( 25 );
            emit( enable( true, line * 10 + 1 ) );
            emit( editChanged( QString::number( 25 ), line * 10 + 1 ) );
            break;
        }
        case ScriptCommand::SET_CAMERA:
        {
            commandLine.push_back( camera[0] );
            commandLine.push_back( camera[1] );
            commandLine.push_back( camera[2] );
            emit( enable( true, line * 10 + 1 ) );
            emit( enable( true, line * 10 + 2 ) );
            emit( enable( true, line * 10 + 3 ) );
            QVector3D vec = camera[0].value<QVector3D>();
            emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), line * 10 + 1 ) );
            vec = camera[1].value<QVector3D>();
            emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), line * 10 + 2 ) );
            vec = camera[2].value<QVector3D>();
            emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), line * 10 + 3 ) );
            break;
        }
        case ScriptCommand::INTERPOLATE_CAMERA_TO:
        {
            commandLine.push_back( camera[0] );
            commandLine.push_back( camera[1] );
            commandLine.push_back( camera[2] );
            emit( enable( true, line * 10 + 1 ) );
            emit( enable( true, line * 10 + 2 ) );
            emit( enable( true, line * 10 + 3 ) );
            emit( enable( true, line * 10 + 4 ) );
            QVector3D vec = camera[0].value<QVector3D>();
            emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), line * 10 + 1 ) );
            vec = camera[1].value<QVector3D>();
            emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), line * 10 + 2 ) );
            vec = camera[2].value<QVector3D>();
            emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), line * 10 + 3 ) );
            int steps = 10;
            commandLine.push_back( steps );
            emit( editChanged( QString::number( steps ), line * 10 + 4 ) );
            break;
        }
    }
    m_script[line] = commandLine;
    if ( line == m_script.size() - 1 && command != 0 )
    {
        addCommand();
    }
}

void ScriptWidget::addCommand()
{
    QList<QVariant> command;
    command.push_back( (int)ScriptCommand::NONE );
    m_script.push_back( command );
    delete this->layout();
    initLayout();
}

void ScriptWidget::copyCamera()
{

}

void ScriptWidget::slotEditChanged( QString text, int id )
{
    int row = id / 10;
    int column = id % 10;
    qDebug() << "row:" << row << "column:" << column;

    int command = m_script[row].at( 0 ).toInt();
    switch( (ScriptCommand)( command ) )
    {
        case ScriptCommand::NONE:
            break;
        case ScriptCommand::DELAY:
        {
            if ( column == 1 )
            {
                m_script[row].replace( column, text.toInt() );
            }
            break;
        }
        case ScriptCommand::SET_CAMERA:
        {
            if ( column > 0 && column < 4 )
            {
                m_script[row].replace( column, string2Vector3D( text ) );
            }
            break;
        }
        case ScriptCommand::INTERPOLATE_CAMERA_TO:
        {
            if ( column > 0 && column < 4 )
            {
                m_script[row].replace( column, string2Vector3D( text ) );
            }
            if ( column == 4 )
            {
                m_script[row].replace( column, text.toInt() );
            }
            break;
        }
    }
}

QVector3D ScriptWidget::string2Vector3D( QString string )
{
    QStringList parts = string.split( ",", QString::SkipEmptyParts );
    if( parts.size() != 3 )
    {
        return QVector3D( 0, 0,0 );
    }
    bool ok = true;
    float x = parts[0].toFloat( &ok );
    if ( !ok )
    {
        return QVector3D( 0, 0,0 );
    }
    float y = parts[1].toFloat( &ok );
    if ( !ok )
    {
        return QVector3D( 0, 0,0 );
    }
    float z = parts[2].toFloat( &ok );
    if ( !ok )
    {
        return QVector3D( 0, 0,0 );
    }

    QVector3D out( x, y, z );
    return out;
}

void ScriptWidget::deleteCommand( int row )
{
    if ( row >= 0 && row < m_script.size() )
    {
        m_script.removeAt( row );
    }
    delete this->layout();
    initLayout();
}
