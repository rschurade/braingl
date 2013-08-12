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
#include "../../data/vptr.h"
#include "../../data/datasets/dataset.h"
#include "../../data/properties/propertygroup.h"

#include <QDebug>

ScriptWidget::ScriptWidget( GLWidget* glWidget, QWidget* parent ) :
    m_glWidget( glWidget ),
    m_runScript( false ),
    m_currentCommandLine( 0 ),
    m_lastGlobal( (int)Fn::Property::G_SAGITTAL ),
    m_lastDataset( 0 ),
    m_lastProperty( (int)Fn::Property::D_ACTIVE )
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

    m_runButton = new QPushButton( "Run", this );
    m_runButton->setCheckable( true );
    buttons1->addWidget( m_runButton );
    connect( m_runButton, SIGNAL( toggled( bool ) ), this, SLOT( run( bool ) ) );

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

        PushButtonWithId* insertButton = new PushButtonWithId( "+", i );
        layout->addWidget( insertButton );
        insertButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( insertButton, SIGNAL( signalClicked( int ) ), this, SLOT( insertCommand( int ) ) );

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
        connect( select, SIGNAL( currentIndexChanged( int, int, int ) ), this, SLOT( commandChanged( int, int ) ) );

        switch( (ScriptCommand)( command[0].toInt() ) )
        {
            case ScriptCommand::NONE:
                addEdit( layout, i* 10 + 1, 5 );
                emit( enable( false, i * 10 + 1 ) );
                emit( enable( false, i * 10 + 2 ) );
                emit( enable( false, i * 10 + 3 ) );
                emit( enable( false, i * 10 + 4 ) );
                emit( enable( false, i * 10 + 5 ) );

                break;
            case ScriptCommand::DELAY:
            {
                addEdit( layout, i* 10 + 1, 5 );
                emit( editChanged( command[1].toString(), i * 10 + 1 ) );
                emit( enable( false, i * 10 + 2 ) );
                emit( enable( false, i * 10 + 3 ) );
                emit( enable( false, i * 10 + 4 ) );
                emit( enable( false, i * 10 + 5 ) );
                break;
            }
            case ScriptCommand::SET_CAMERA:
            {
                addEdit( layout, i* 10 + 1, 5 );
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
            case ScriptCommand::INTERPOLATE_CAMERA:
            {
                addEdit( layout, i* 10 + 1, 5 );
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
            case ScriptCommand::SET_GLOBAL:
            {
                addGlobalSelect( layout, i, command[1].toInt() );
                addEdit( layout, i* 10 + 2, 4 );
                emit( editChanged( command[2].toString(), i * 10 + 2 ) );
                emit( enable( false, i * 10 + 3 ) );
                emit( enable( false, i * 10 + 4 ) );
                emit( enable( false, i * 10 + 5 ) );
                break;
            }
            case ScriptCommand::INCREMENT_GLOBAL:
            {
                addGlobalSelect( layout, i, command[1].toInt() );
                addEdit( layout, i* 10 + 2, 4 );
                emit( editChanged( command[2].toString(), i * 10 + 2 ) );
                emit( editChanged( command[3].toString(), i * 10 + 3 ) );
                emit( enable( false, i * 10 + 4 ) );
                emit( enable( false, i * 10 + 5 ) );
                break;
            }

            case ScriptCommand::SET_PROPERTY:
            {
                addPropertySelect( layout, i, command[1].toInt(), command[2].toInt() );
                addEdit( layout, i* 10 + 2, 4 );
                emit( editChanged( command[2].toString(), i * 10 + 2 ) );
                emit( editChanged( command[3].toString(), i * 10 + 3 ) );
                emit( enable( false, i * 10 + 4 ) );
                emit( enable( false, i * 10 + 5 ) );
                break;
            }
            case ScriptCommand::INCREMENT_PROPERTY:
            {
                addPropertySelect( layout, i, command[1].toInt(), command[2].toInt() );
                addEdit( layout, i* 10 + 2, 4 );
                emit( editChanged( command[2].toString(), i * 10 + 2 ) );
                emit( editChanged( command[3].toString(), i * 10 + 3 ) );
                emit( editChanged( command[4].toString(), i * 10 + 4 ) );
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

void ScriptWidget::addEdit( QHBoxLayout* layout, int startId, int count  )
{
    for ( int k = 0; k < count; ++k )
    {
        LineEditID* edit = new LineEditID( startId + k, this );
        connect( this, SIGNAL( enable( bool, int ) ), edit, SLOT( setEnabled2( bool, int ) ) );
        connect( this, SIGNAL( editChanged( QString, int ) ), edit, SLOT( setText2( QString, int ) ) );
        connect( edit, SIGNAL( textEdited2( QString, int ) ), this, SLOT( slotEditChanged( QString, int ) ) );
        layout->addWidget( edit );
    }
}

void ScriptWidget::addGlobalSelect( QHBoxLayout* layout, int id, int selected )
{
    ComboBoxID* select = new ComboBoxID( id, this );
    for ( int j = (int)Fn::Property::G_FIRST + 1; j < (int)Fn::Property::G_LAST; ++j )
    {
        if ( Fn::Prop2String::s( (Fn::Property)j ) != "" )
        {
            select->insertItem( j, Fn::Prop2String::s( (Fn::Property)j ), j );
        }
    }
    int index = 0;
    for ( int j = (int)Fn::Property::G_FIRST + 1; j < (int)Fn::Property::G_LAST; ++j )
    {
        if ( selected == j )
        {
            select->setCurrentIndex( index );
        }
        if ( Fn::Prop2String::s( (Fn::Property)j ) != "" )
        {
            ++index;
        }
    }
    connect( select, SIGNAL( currentIndexChanged( int, int, int ) ), this, SLOT( slotGlobalSelectChanged( int, int, int ) ) );
    layout->addWidget( select );
}

void ScriptWidget::addPropertySelect( QHBoxLayout* layout, int id, int selected, int dataset )
{
    ComboBoxID* select = new ComboBoxID( id, this );

    if ( Models::d()->rowCount() > dataset && dataset >= 0 )
    {
        Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( dataset, (int) Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
        PropertyGroup* props = ds->properties();
        int toSelect = 0;

        for ( int i = 0; i < props->size(); ++i )
        {
            QPair<Fn::Property, Property*> prop = props->getNthPropertyPair( i );
            if ( prop.second->getPropertyTab() != "none" || prop.first == Fn::Property::D_ACTIVE )
            {
                select->insertItem( i, Fn::Prop2String::s( prop.first ), (int)prop.first );
                if ( selected == (int)prop.first )
                {
                    toSelect = select->count() - 1;
                }
            }
        }
        select->setCurrentIndex( toSelect );
    }
    connect( select, SIGNAL( currentIndexChanged( int, int, int ) ), this, SLOT( slotPropertySelectChanged( int, int, int ) ) );
    layout->addWidget( select );
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
    this->repaint();
    initLayout();
    if ( settings.contains( "delay" ) )
    {
        m_delay->setValue( settings.value( "delay").toInt() );
    }
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
    settings.setValue( "delay", m_delay->getValue() );
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
            m_runButton->setChecked( false );
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
            m_targetDelay = line[1].toInt();
            m_currentDelay = 0;
            slotDelay();
            return;
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
        case ScriptCommand::INTERPOLATE_CAMERA:
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
        case ScriptCommand::SET_GLOBAL:
        {
            Models::g()->setData( Models::g()->index( line[1].toInt(), 0 ), line[2] );
            break;
        }
        case ScriptCommand::INCREMENT_GLOBAL:
        {
            m_lastGlobal = line[1].toInt();
            m_stepSize = line[2].toInt();
            m_targetStep = line[3].toInt();
            m_currentStep = 0;
            slotIncrementGlobal();
            return;
            break;
        }
        case ScriptCommand::SET_PROPERTY:
        {
            QString v = line[3].toString();
            bool ok;
            if ( v.startsWith( "(float)" ) )
            {
                v = v.right( v.size() - 7 );
                float f = v.toFloat( &ok );
                if ( ok )
                {
                    Models::d()->setData( Models::d()->index( line[2].toInt(), line[1].toInt() ), f, Qt::DisplayRole );
                }
            }
            else
            {
                Models::d()->setData( Models::d()->index( line[2].toInt(), line[1].toInt() ), line[3], Qt::DisplayRole );
            }
            break;
        }
        case ScriptCommand::INCREMENT_PROPERTY:
        {
            m_lastDataset = line[2].toInt();
            m_lastProperty = line[1].toInt();
            m_stepSize = line[3].toFloat();
            m_targetStep = line[4].toInt();
            m_currentStep = 0;
            slotIncrementProperty();
            return;
            break;
        }
    }
    Models::g()->submit();

    if ( m_screenshotEach->checked() )
    {
        emit( screenshot() );
    }

    QTimer::singleShot( delay, this, SLOT( run() ) );
}

void ScriptWidget::slotIncrementGlobal()
{
    if ( !m_runScript )
    {
        return;
    }

    int delay = m_delay->getValue();
    if ( ++m_currentStep > m_targetStep )
    {
        QTimer::singleShot( delay, this, SLOT( run() ) );
        return;
    }
    int value = Models::g()->data( Models::g()->index( m_lastGlobal, 0 ) ).toInt();
    value += m_stepSize;
    Models::g()->setData( Models::g()->index( m_lastGlobal, 0 ), value );

    Models::g()->submit();

    if ( m_screenshotEach->checked() )
    {
        emit( screenshot() );
    }

    QTimer::singleShot( delay, this, SLOT( slotIncrementGlobal() ) );

}

void ScriptWidget::slotIncrementProperty()
{
    if ( !m_runScript )
    {
        return;
    }

    int delay = m_delay->getValue();
    if ( ++m_currentStep > m_targetStep )
    {
        QTimer::singleShot( delay, this, SLOT( run() ) );
        return;
    }
    float value = Models::d()->data( Models::d()->index( m_lastDataset, m_lastProperty ) ).toFloat();
    value += m_stepSize;

    Models::d()->setData( Models::d()->index( m_lastDataset, m_lastProperty ), value, Qt::DisplayRole );

    Models::d()->submit();

    if ( m_screenshotEach->checked() )
    {
        emit( screenshot() );
    }

    QTimer::singleShot( delay, this, SLOT( slotIncrementProperty() ) );

}

void ScriptWidget::slotDelay()
{
    if ( !m_runScript )
    {
        return;
    }

    int delay = m_delay->getValue();
    if ( m_currentDelay > m_targetDelay )
    {
        QTimer::singleShot( delay, this, SLOT( run() ) );
        return;
    }
    m_currentDelay += delay;

    Models::g()->submit();

    if ( m_screenshotEach->checked() )
    {
        emit( screenshot() );
    }

    QTimer::singleShot( delay, this, SLOT( slotDelay() ) );
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

    if ( m_screenshotEach->checked() )
    {
        emit( screenshot() );
    }

    QTimer::singleShot( delay, this, SLOT( interpolateCamera() ) );
}

void ScriptWidget::commandChanged( int line, int command )
{
    line /= 10;
    QList<QVariant> commandLine;
    commandLine.push_back( command );

    QList<QVariant> camera = m_glWidget->getCamera()->getState();

    switch( (ScriptCommand)( command ) )
    {
        case ScriptCommand::NONE:
            break;
        case ScriptCommand::DELAY:
        {
            commandLine.push_back( 25 );
            break;
        }
        case ScriptCommand::SET_CAMERA:
        {
            commandLine.push_back( camera[0] );
            commandLine.push_back( camera[1] );
            commandLine.push_back( camera[2] );
            break;
        }
        case ScriptCommand::INTERPOLATE_CAMERA:
        {
            commandLine.push_back( camera[0] );
            commandLine.push_back( camera[1] );
            commandLine.push_back( camera[2] );
            commandLine.push_back( 25 );
            break;
        }
        case ScriptCommand::SET_GLOBAL:
        {
            commandLine.push_back( m_lastGlobal );
            commandLine.push_back( Models::g()->data( Models::g()->index( m_lastGlobal, 0 ) ).toInt() );
            break;
        }
        case ScriptCommand::INCREMENT_GLOBAL:
        {
            commandLine.push_back( m_lastGlobal );
            commandLine.push_back( 1 );
            commandLine.push_back( 1 );
            break;
        }
        case ScriptCommand::SET_PROPERTY:
        {
            commandLine.push_back( (int)Fn::Property::D_ACTIVE );
            commandLine.push_back( m_lastDataset );
            commandLine.push_back( 1 );
            break;
        }
        case ScriptCommand::INCREMENT_PROPERTY:
        {
            commandLine.push_back( (int)Fn::Property::D_ACTIVE );
            commandLine.push_back( m_lastDataset );
            commandLine.push_back( 1 );
            commandLine.push_back( 1 );
            break;
        }
    }
    m_script[line] = commandLine;
    if ( line == m_script.size() - 1 && command != 0 )
    {
        QList<QVariant> command;
        command.push_back( (int)ScriptCommand::NONE );
        m_script.push_back( command );
    }

    delete this->layout();
    this->repaint();
    initLayout();
}

void ScriptWidget::copyCamera()
{

}

void ScriptWidget::slotEditChanged( QString text, int id )
{
    int row = id / 10;
    int column = id % 10;

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
        case ScriptCommand::INTERPOLATE_CAMERA:
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
        case ScriptCommand::SET_GLOBAL:
        {
            if ( column == 2 )
            {
                if ( m_script[row].at( 1 ).toInt() < (int)Fn::Property::G_BACKGROUND_COLOR_MAIN )
                {
                    m_script[row].replace( 2, text.toInt() );
                }
            }
            break;
        }
        case ScriptCommand::INCREMENT_GLOBAL:
        {
            if ( column == 2 || column == 3 )
            {
                if ( m_script[row].at( 1 ).toInt() < (int)Fn::Property::G_BACKGROUND_COLOR_MAIN )
                {
                    m_script[row].replace( column, text.toInt() );
                }
            }
            break;
        }
        case ScriptCommand::SET_PROPERTY:
        {
            if ( column == 2 )
            {
                int ds = text.toInt();
                int countDS = Models::d()->rowCount();
                if ( countDS > ds && ds >= 0 )
                {
                    m_script[row].replace( column, ds );
                }
                else
                {
                    m_script[row].replace( column, 0 );
                }
            }
            if ( column == 3 )
            {
                m_script[row].replace( column, text );
            }
            break;
        }
        case ScriptCommand::INCREMENT_PROPERTY:
        {
            if ( column == 2 )
            {
                int ds = text.toInt();
                int countDS = Models::d()->rowCount();
                if ( countDS > ds && ds >= 0 )
                {
                    m_script[row].replace( column, text.toInt() );
                }
                else
                {
                    m_script[row].replace( column, 0 );
                }
            }
            if ( column == 3 )
            {
                m_script[row].replace( column, text );
            }
            if ( column == 4 )
            {
                m_script[row].replace( column, text.toInt() );
            }
            break;
        }
    }
}

void ScriptWidget::slotGlobalSelectChanged( int line, int prop, int data )
{
    m_script[line].replace( 1, data );
    m_lastGlobal = data;
}

void ScriptWidget::slotPropertySelectChanged( int line, int prop, int data )
{
    m_script[line].replace( 1, data );
    m_lastProperty = data;
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
    this->repaint();
    initLayout();
}

void ScriptWidget::insertCommand( int row )
{
    if ( row >= 0 && row < m_script.size() )
    {
        QList<QVariant> command;
        command.push_back( (int)ScriptCommand::NONE );
        m_script.insert( row, command );
    }
    delete this->layout();
    this->repaint();
    initLayout();
}
