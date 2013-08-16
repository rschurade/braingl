/*
 * scriptwidget.cpp
 *
 *  Created on: 11.08.2013
 *      Author: Ralph
 */

#include "scriptwidget.h"
#include "glwidget.h"
#include "../gl/camera.h"
#include "../gl/arcball.h"

#include "controls/checkboxid.h"
#include "controls/comboboxid.h"
#include "controls/lineeditid.h"
#include "controls/checkboxwithlabel.h"
#include "controls/sliderwitheditint.h"
#include "controls/pushbuttonwithid.h"

#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/vptr.h"
#include "../../data/datasets/dataset.h"
#include "../../data/properties/propertygroup.h"

#include <QDebug>

ScriptWidget::ScriptWidget( GLWidget* glWidget, QWidget* parent ) :
    m_glWidget( glWidget ),
    m_widgetToEnsureVisible ( 0 ),
    m_contRunning( 0 ),
    m_screenshotEach( 0 ),
    m_delay( 0 ),
    m_scrollArea( 0 ),
    m_runButton( 0 ),

    m_lastInsertedLine( 0 ),
    m_runScript( false ),
    m_currentCommandLine( 0 ),

    m_lastGlobal( (int)Fn::Property::G_SAGITTAL ),
    m_lastDataset( 0 ),
    m_lastProperty( (int)Fn::Property::D_ACTIVE ),

    m_loopCount( 1 ),
    m_totalLoops( 1 ),
    m_loopBegin( 0 ),
    m_inBlock( false ),
    m_inLoop( false ),
    m_render( false ),


    m_currentZoom( 1.0f ),
    m_targetZoom( 1.0f ),
    m_currentMoveX( 0.0f ),
    m_currentMoveY( 0.0f ),
    m_targetMoveX( 0.0f ),
    m_targetMoveY( 0.0f )
{
    QList<QVariant> line;
    line.push_back( true );
    line.push_back( (int)ScriptCommand::NONE );
    m_script.push_back( line );

    initLayout();
}

ScriptWidget::~ScriptWidget()
{
}

void ScriptWidget::rebuild()
{
    m_scrollArea->takeWidget();
    //delete m_scriptPanel;
    m_scriptPanel = buildScriptLayout();
    m_scrollArea->setWidget( m_scriptPanel );
    this->repaint();
}

void ScriptWidget::initLayout()
{
    m_widgetToEnsureVisible = 0;

    m_scriptPanel = buildScriptLayout();
    m_scrollArea = new QScrollArea( this );
    m_scrollArea->setWidget( m_scriptPanel );

    QScrollBar* scrollbar = m_scrollArea->verticalScrollBar();
    connect( scrollbar, SIGNAL( rangeChanged( int, int ) ), this, SLOT( moveScrollBarToBottom( int, int ) ) );

    QVBoxLayout* layout2 = new QVBoxLayout();
    QHBoxLayout* buttons1 = new QHBoxLayout();

    QPushButton* resetButton = new QPushButton( "Reset", this );
    buttons1->addWidget( resetButton );
    connect( resetButton, SIGNAL( clicked() ), this, SLOT( resetScript() ) );

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

    m_contRunning = new CheckboxWithLabel( "continous", -1, this );
    buttons2->addWidget( m_contRunning );

    m_screenshotEach = new CheckboxWithLabel( "screenshot each step", -1, this );
    buttons2->addWidget( m_screenshotEach );

    m_delay = new SliderWithEditInt( "delay", 0, this );
    m_delay->setMin( 1 );
    m_delay->setMax( 1000 );
    m_delay->setValue( 25 );

    buttons2->addStretch();
    buttons2->addStretch();
    buttons2->addStretch();
    buttons1->addStretch();
    buttons1->addWidget( m_delay );

    layout2->addLayout( buttons2 );
    layout2->addLayout( buttons1 );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_scrollArea );
    layout->addLayout( layout2 );

    this->setLayout( layout );
    this->setMinimumSize( 600, 600 );

    if ( m_widgetToEnsureVisible )
    {
        m_scrollArea->ensureWidgetVisible( m_widgetToEnsureVisible );
    }
}

QWidget* ScriptWidget::buildScriptLayout()
{
    QGroupBox *groupBox = new QGroupBox( this );
    QVBoxLayout* scriptLayout = new QVBoxLayout();
    groupBox->setLayout( scriptLayout );

    bool inLoop = false;
    bool inBlock = false;

    for ( int i = 0; i < m_script.size(); ++i )
    {

        QHBoxLayout* layout = new QHBoxLayout();
        QList<QVariant>line = m_script[i];

        CheckBoxID* checkBox = new CheckBoxID( i, this );
        layout->addWidget( checkBox );
        bool lineActive = line[0].toBool();
        checkBox->setChecked( lineActive );
        connect( checkBox, SIGNAL( signalStateChanged( int, int ) ), this, SLOT( slotCheckboxChanged( int, int ) ) );
        connect( this, SIGNAL( checkBoxChanged( int, int ) ), checkBox, SLOT( slotSetChecked2( int, int ) ) );

        if ( i == m_lastInsertedLine )
        {
            m_widgetToEnsureVisible = checkBox;
        }

        PushButtonWithId* insertButton = new PushButtonWithId( "+", i );
        layout->addWidget( insertButton );
        insertButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( insertButton, SIGNAL( signalClicked( int ) ), this, SLOT( insertCommand( int ) ) );

        PushButtonWithId* deleteButton = new PushButtonWithId( "-", i );
        layout->addWidget( deleteButton );
        deleteButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( deleteButton, SIGNAL( signalClicked( int ) ), this, SLOT( deleteCommand( int ) ) );

        PushButtonWithId* upButton = new PushButtonWithId( "^", i );
        layout->addWidget( upButton );
        upButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( upButton, SIGNAL( signalClicked( int ) ), this, SLOT( moveCommandUp( int ) ) );

        PushButtonWithId* downButton = new PushButtonWithId( "v", i );
        layout->addWidget( downButton );
        downButton->setStyleSheet( "QPushButton { font:  bold 12px; max-width: 14px; max-height: 14px; } ");
        connect( downButton, SIGNAL( signalClicked( int ) ), this, SLOT( moveCommandDown( int ) ) );


        QColor lightRed( 255, 168, 168 );
        QColor lightBlue( 168, 168, 255 );
        QColor lightGreen( 168, 255, 168 );
        QColor lightPurple( 255, 158, 255 );
        QColor yellow( 255, 255, 0 );
        QColor magenta( 0, 255, 255 );
        QColor orange( 255, 160, 0 );

        if( inLoop && line[1].toInt() != (int)ScriptCommand::END_LOOP )
        {
            QLabel* label = new QLabel( "loop", this );
            label->setStyleSheet("QLabel { background-color : " + lightRed.name() + "}");
            layout->addWidget( label );
        }
        if( inBlock && line[1].toInt() != (int)ScriptCommand::END_BLOCK )
        {
            QLabel* label = new QLabel( "block", this );

            label->setStyleSheet("QLabel { background-color : " + lightBlue.name() + "}");
            layout->addWidget( label );
        }


        ComboBoxID* select = new ComboBoxID( i * 10, this );
        for ( int j = 0; j < NUM_SCRIPT_COMMANDS; ++j )
        {
            select->insertItem( j, Script2String::s( (ScriptCommand)j ) );
        }

        layout->addWidget( select );
        select->setCurrentIndex( line[1].toInt() );
        connect( select, SIGNAL( currentIndexChanged( int, int, int ) ), this, SLOT( commandChanged( int, int ) ) );
        connect( this, SIGNAL( enable2( bool, int ) ), select, SLOT( setEnabled2( bool, int ) ) );

        switch( (ScriptCommand)( line[1].toInt() ) )
        {
            case ScriptCommand::NONE:
                addStretch( layout, 5 );
                break;
            case ScriptCommand::DELAY:
            {
                select->setStyleSheet("QComboBox { background-color : " + magenta.name() + "}");
                addStretch( layout, 5 );
                break;
            }
            case ScriptCommand::SET_CAMERA:
            {
                select->setStyleSheet("QComboBox { background-color : " + yellow.name() + "}");
                addEdit( layout, i* 10 + 1, 3 );
                QVector3D vec = line[2].value<QVector3D>();
                emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 1 ) );
                vec = line[3].value<QVector3D>();
                emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 2 ) );
                vec = line[4].value<QVector3D>();
                emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 3 ) );
                addStretch( layout, 2 );
                break;
            }
            case ScriptCommand::SET_GLOBAL:
            {
                select->setStyleSheet("QComboBox { background-color : " + lightGreen.name() + "}");
                addGlobalSelect( layout, i, line[2].toInt(), lineActive );
                addEdit( layout, i* 10 + 2, 1 );
                emit( editChanged( line[3].toString(), i * 10 + 2 ) );
                addStretch( layout, 3 );
                break;
            }
            case ScriptCommand::INCREMENT_GLOBAL:
            {
                select->setStyleSheet("QComboBox { background-color : " + lightGreen.name() + "}");
                addGlobalSelect( layout, i, line[2].toInt(), lineActive );
                addEdit( layout, i* 10 + 2, 1 );
                emit( editChanged( line[3].toString(), i * 10 + 2 ) );
                addStretch( layout, 3 );
                break;
            }

            case ScriptCommand::SET_PROPERTY:
            {
                select->setStyleSheet("QComboBox { background-color : " + lightPurple.name() + "}");
                addPropertySelect( layout, i, line[2].toInt(), line[3].toInt(), lineActive );
                addEdit( layout, i* 10 + 2, 2 );
                emit( editChanged( line[3].toString(), i * 10 + 2 ) );
                emit( editChanged( line[4].toString(), i * 10 + 3 ) );
                addStretch( layout, 2 );
                break;
            }
            case ScriptCommand::INCREMENT_PROPERTY:
            {
                select->setStyleSheet("QComboBox { background-color : " + lightPurple.name() + "}");
                addPropertySelect( layout, i, line[2].toInt(), line[3].toInt(), lineActive );
                addEdit( layout, i* 10 + 2, 2 );
                emit( editChanged( line[3].toString(), i * 10 + 2 ) );
                emit( editChanged( line[4].toString(), i * 10 + 3 ) );
                addStretch( layout, 2 );
                break;
            }
            case ScriptCommand::SET_ARCBALL:
            {
                select->setStyleSheet("QComboBox { background-color : " + yellow.name() + "}");
                addEdit( layout, i* 10 + 1, 4 );
                QVector4D vec = line[2].value<QQuaternion>().toVector4D();
                emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ) + ", " + QString::number( vec.w() ), i * 10 + 1 ) );
                emit( editChanged( line[3].toString(), i * 10 + 2 ) );
                emit( editChanged( line[4].toString(), i * 10 + 3 ) );
                emit( editChanged( line[5].toString(), i * 10 + 4 ) );
                addStretch( layout, 1 );
                break;
            }
            case ScriptCommand::BEGIN_LOOP:
            {
                select->setStyleSheet("QComboBox { background-color : " + lightRed.name() + "}");
                addEdit( layout, i* 10 + 1, 1 );
                emit( editChanged( line[2].toString(), i * 10 + 1 ) );
                addStretch( layout, 4 );
                inLoop = true;
                break;
            }
            case ScriptCommand::END_LOOP:
            {
                select->setStyleSheet("QComboBox { background-color : " + lightRed.name() + "}");
                inLoop = false;
                addStretch( layout, 5 );
                break;
            }
            case ScriptCommand::BEGIN_BLOCK:
            {
                select->setStyleSheet("QComboBox { background-color : " + lightBlue.name() + "}");
                inBlock = true;
                addStretch( layout, 5 );
                break;
            }
            case ScriptCommand::END_BLOCK:
            {
                select->setStyleSheet("QComboBox { background-color : " + lightBlue.name() + "}");
                inBlock = false;
                addStretch( layout, 5 );
                break;
            }
            case ScriptCommand::COMMENT:
            {
                select->setStyleSheet("QComboBox { background-color : " + orange.name() + "}");
                addEdit( layout, i* 10 + 1, 1 );
                emit( editChanged( line[2].toString(), i * 10 + 1 ) );
                break;
            }
        }

        select->setEnabled( lineActive );
        emit( enable( lineActive, i * 10 + 1 ) );
        emit( enable( lineActive, i * 10 + 2 ) );
        emit( enable( lineActive, i * 10 + 3 ) );
        emit( enable( lineActive, i * 10 + 4 ) );
        emit( enable( lineActive, i * 10 + 5 ) );

        layout->addStretch();
        scriptLayout->addLayout( layout );
    }
    scriptLayout->addStretch();

    return groupBox;
}

void ScriptWidget::commandChanged( int line, int command )
{
    line /= 10;
    QList<QVariant> commandLine;
    commandLine.push_back( true );
    commandLine.push_back( command );

    QList<QVariant> camera = m_glWidget->getCamera()->getState();

    switch( (ScriptCommand)( command ) )
    {
        case ScriptCommand::NONE:
            break;
        case ScriptCommand::DELAY:
        {
            break;
        }
        case ScriptCommand::SET_CAMERA:
        {
            commandLine.push_back( camera[0] );
            commandLine.push_back( camera[1] );
            commandLine.push_back( camera[2] );
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
            break;
        }
        case ScriptCommand::SET_ARCBALL:
        {
            QQuaternion rot = m_glWidget->getArcBall()->getRotation();
            float zoom = m_glWidget->getArcBall()->getZoom();
            float moveX = m_glWidget->getArcBall()->getMoveX();
            float moveY = m_glWidget->getArcBall()->getMoveY();
            commandLine.push_back( rot );
            commandLine.push_back( zoom );
            commandLine.push_back( moveX );
            commandLine.push_back( moveY );
            break;
        }
        case ScriptCommand::BEGIN_LOOP:
        {
            commandLine.push_back( 1 );
            QList<QVariant> commandLine2;
            commandLine2.push_back( true );
            commandLine2.push_back( (int)ScriptCommand::NONE );
            m_script.insert( line + 1, commandLine2 );
            QList<QVariant> commandLine3;
            commandLine3.push_back( true );
            commandLine3.push_back( (int)ScriptCommand::END_LOOP );
            m_script.insert( line + 2, commandLine3 );
            if ( line + 2 == m_script.size() - 1 )
            {
                QList<QVariant> command4;
                command4.push_back( true );
                command4.push_back( (int)ScriptCommand::NONE );
                m_script.push_back( command4 );
            }
            break;
        }
        case ScriptCommand::END_LOOP:
        {
            break;
        }
        case ScriptCommand::BEGIN_BLOCK:
        {
            QList<QVariant> commandLine2;
            commandLine2.push_back( true );
            commandLine2.push_back( (int)ScriptCommand::NONE );
            m_script.insert( line + 1, commandLine2 );
            QList<QVariant> commandLine3;
            commandLine3.push_back( true );
            commandLine3.push_back( (int)ScriptCommand::END_BLOCK );
            m_script.insert( line + 2, commandLine3 );
            if ( line + 2 == m_script.size() - 1 )
            {
                QList<QVariant> command4;
                command4.push_back( true );
                command4.push_back( (int)ScriptCommand::NONE );
                m_script.push_back( command4 );
            }
            break;
        }
        case ScriptCommand::END_BLOCK:
            break;
        case ScriptCommand::COMMENT:
        {
            commandLine.push_back( "comment" );
            break;
        }
    }
    m_script[line] = commandLine;
    if ( line == m_script.size() - 1 && command != 0 )
    {
        QList<QVariant> command;
        command.push_back( true );
        command.push_back( (int)ScriptCommand::NONE );
        m_script.push_back( command );
    }
    rebuild();
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

void ScriptWidget::addStretch( QHBoxLayout* layout, int count )
{
    for ( int i = 0; i < count; ++i )
    {
        layout->addStretch();
    }
}

void ScriptWidget::addGlobalSelect( QHBoxLayout* layout, int id, int selected, bool active )
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
    connect( this, SIGNAL( enable2( bool, int ) ), select, SLOT( setEnabled2( bool, int ) ) );
    layout->addWidget( select );
    select->setEnabled( active );
}

void ScriptWidget::addPropertySelect( QHBoxLayout* layout, int id, int selected, int dataset, bool active )
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
    connect( this, SIGNAL( enable2( bool, int ) ), select, SLOT( setEnabled2( bool, int ) ) );
    layout->addWidget( select );
    select->setEnabled( active );
}

void ScriptWidget::loadScript( QString fileName )
{
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
    rebuild();
    if ( settings.contains( "delay" ) )
    {
        m_delay->setValue( settings.value( "delay").toInt() );
    }
}

void ScriptWidget::loadScript()
{
    QString fn = Models::g()->data( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ) ).toString();
    QString fileName = QFileDialog::getOpenFileName( this, "Open File", fn );
    loadScript( fileName );
}

void ScriptWidget::saveScript( QString fileName )
{
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

void ScriptWidget::saveScript()
{
    QString fn = Models::g()->data( Models::g()->index( (int)Fn::Property::G_LAST_PATH, 0 ) ).toString();
    QString fileName = QFileDialog::getSaveFileName( this, "Save File", fn );
    saveScript( fileName );
}

void ScriptWidget::run( bool checked )
{
    m_inBlock = false;
    m_loopCount = 1;
    m_totalLoops = 1;
    m_inLoop = false;
    m_render = false;
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

    if ( !line[0].toBool() )
    {
        QTimer::singleShot( 1, this, SLOT( run() ) );
        return;
    }

    int delay = m_delay->getValue();
    int command = line[1].toInt();

    QList<QVariant>camera = m_glWidget->getCamera()->getState();

    switch( (ScriptCommand)( command ) )
    {
        case ScriptCommand::NONE:
        case ScriptCommand::COMMENT:
        {
                QTimer::singleShot( 1, this, SLOT( run() ) );
                return;
        }
        case ScriptCommand::DELAY:
        {
            break;
        }
        case ScriptCommand::SET_CAMERA:
        {
            if ( m_loopCount == m_totalLoops )
            {
                m_currentCamera = camera;
                m_interpolatedCamera.clear();
                m_interpolatedCamera.push_back( line[2] );
                m_interpolatedCamera.push_back( line[3] );
                m_interpolatedCamera.push_back( line[4] );
            }

            QVector3D pos1 = m_currentCamera[0].value<QVector3D>();
            QVector3D pos2 = m_interpolatedCamera[0].value<QVector3D>();

            QVector3D look1 = m_currentCamera[1].value<QVector3D>();
            QVector3D look2 = m_interpolatedCamera[1].value<QVector3D>();

            QVector3D up1 = m_currentCamera[2].value<QVector3D>();
            QVector3D up2 = m_interpolatedCamera[2].value<QVector3D>();

            float div = (float)( m_totalLoops - m_loopCount + 1 ) / (float)m_totalLoops;

            QVector3D pos = div * pos2 + ( 1.0f - div ) * pos1;
            QVector3D look = div * look2 + ( 1.0f - div ) * look1;
            QVector3D up = div * up2 + ( 1.0f - div ) * up1;

            QList<QVariant>newCam;
            newCam.push_back( pos );
            newCam.push_back( look );
            newCam.push_back( up );
            m_glWidget->getCamera()->setState( newCam );
            break;

        }
        case ScriptCommand::SET_GLOBAL:
        {
            Models::g()->setData( Models::g()->index( line[2].toInt(), 0 ), line[3] );
            break;
        }
        case ScriptCommand::INCREMENT_GLOBAL:
        {
            m_lastGlobal = line[2].toInt();
            float stepSize = line[3].toFloat();
            float value = Models::g()->data( Models::g()->index( m_lastGlobal, 0 ) ).toFloat() + stepSize;
            Models::g()->setData( Models::g()->index( m_lastGlobal, 0 ), value );
            break;
        }
        case ScriptCommand::SET_PROPERTY:
        {
            QString v = line[4].toString();
            bool ok;
            if ( v.startsWith( "(float)" ) )
            {
                v = v.right( v.size() - 7 );
                float f = v.toFloat( &ok );
                if ( ok )
                {
                    Models::d()->setData( Models::d()->index( line[3].toInt(), line[2].toInt() ), f, Qt::DisplayRole );
                }
            }
            else
            {
                Models::d()->setData( Models::d()->index( line[3].toInt(), line[2].toInt() ), line[4], Qt::DisplayRole );
            }
            break;
        }
        case ScriptCommand::INCREMENT_PROPERTY:
        {
            m_lastDataset = line[3].toInt();
            m_lastProperty = line[2].toInt();
            float stepSize = line[4].toFloat();
            float value = Models::d()->data( Models::d()->index( m_lastDataset, m_lastProperty ) ).toFloat() + stepSize;
            Models::d()->setData( Models::d()->index( m_lastDataset, m_lastProperty ), value, Qt::DisplayRole );
            break;
        }
        case ScriptCommand::SET_ARCBALL:
        {
            if ( m_loopCount == m_totalLoops )
            {
                m_currentRot = m_glWidget->getArcBall()->getRotation();
                m_targetRot = line[2].value<QQuaternion>();
                m_currentZoom = m_glWidget->getArcBall()->getZoom();
                m_targetZoom = line[3].toFloat();
                m_currentMoveX = m_glWidget->getArcBall()->getMoveX();
                m_targetMoveX = line[4].toFloat();
                m_currentMoveY = m_glWidget->getArcBall()->getMoveY();
                m_targetMoveY = line[5].toFloat();
            }

            float div = (float)( m_totalLoops - m_loopCount + 1 ) / (float)m_totalLoops;

            QQuaternion rot = QQuaternion::slerp( m_currentRot, m_targetRot, div );
            m_glWidget->getArcBall()->setRotation( rot );

            float zoom = ( 1.0f - div ) * m_currentZoom + div * m_targetZoom;
            float moveX = ( 1.0f - div ) * m_currentMoveX + div * m_targetMoveX;
            float moveY = ( 1.0f - div ) * m_currentMoveY + div * m_targetMoveY;

            m_glWidget->getArcBall()->setZoom( zoom );
            m_glWidget->getArcBall()->setMoveX( moveX );
            m_glWidget->getArcBall()->setMoveY( moveY );

            break;
        }
        case ScriptCommand::BEGIN_LOOP:
        {
            m_loopCount = line[2].toInt();
            m_totalLoops = line[2].toInt();
            m_loopBegin = m_currentCommandLine;
            m_inLoop = true;
            break;
        }
        case ScriptCommand::END_LOOP:
        {
            if ( --m_loopCount )
            {
                m_currentCommandLine = m_loopBegin;
                m_render = true;
            }
            else
            {
                m_loopCount = 1;
                m_totalLoops = 1;
                m_inLoop = false;
            }
            break;
        }
        case ScriptCommand::BEGIN_BLOCK:
        {
            m_inBlock = true;
            break;
        }
        case ScriptCommand::END_BLOCK:
        {
            m_inBlock = false;
            break;
        }
    }

    if ( m_inBlock || ( m_inLoop && !m_render ) )
    {
        QTimer::singleShot( 1, this, SLOT( run() ) );
        return;
    }

    if ( m_screenshotEach->checked() )
    {
        emit( screenshot() );
    }
    Models::g()->submit();

    m_render = false;
    QTimer::singleShot( delay, this, SLOT( run() ) );
}

void ScriptWidget::slotEditChanged( QString text, int id )
{
    int row = id / 10;
    int column = ( id % 10 ) + 1;

    int command = m_script[row].at( 1 ).toInt();
    switch( (ScriptCommand)( command ) )
    {
        case ScriptCommand::NONE:
            break;
        case ScriptCommand::DELAY:
        {
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
        case ScriptCommand::SET_GLOBAL:
        {
            if ( column == 3 )
            {
                if ( m_script[row].at( 2 ).toInt() < (int)Fn::Property::G_BACKGROUND_COLOR_MAIN )
                {
                    m_script[row].replace( 3, text );
                }
            }
            break;
        }
        case ScriptCommand::INCREMENT_GLOBAL:
        {
            if ( column == 3 )
            {
                if ( m_script[row].at( 2 ).toInt() < (int)Fn::Property::G_BACKGROUND_COLOR_MAIN )
                {
                    m_script[row].replace( column, text );
                }
            }
            break;
        }
        case ScriptCommand::SET_PROPERTY:
        {
            if ( column == 3 )
            {
                int ds = text.toInt();
                m_lastDataset = ds;
                int countDS = Models::d()->rowCount();
                if ( countDS > ds && ds >= 0 )
                {
                    m_script[row].replace( column, ds );
                    m_lastInsertedLine = row;
                   rebuild();
                }
                else
                {
                    m_script[row].replace( column, 0 );
                }
            }
            if ( column == 4 )
            {
                m_script[row].replace( column, text );
            }
            break;
        }
        case ScriptCommand::INCREMENT_PROPERTY:
        {
            if ( column == 3 )
            {
                int ds = text.toInt();
                m_lastDataset = ds;
                int countDS = Models::d()->rowCount();
                if ( countDS > ds && ds >= 0 )
                {
                    m_script[row].replace( column, text.toInt() );
                    m_lastInsertedLine = row;
                    rebuild();
                }
                else
                {
                    m_script[row].replace( column, 0 );
                }
            }
            if ( column == 4 )
            {
                m_script[row].replace( column, text );
            }
            if ( column == 5 )
            {
                m_script[row].replace( column, text.toInt() );
            }
            break;
        }
        case ScriptCommand::SET_ARCBALL:
        {
            if ( column == 2 )
            {
                QStringList parts = text.split( ",", QString::SkipEmptyParts );
                if( parts.size() == 4 )
                {
                    bool ok = true;
                    bool totalOk = true;
                    float x = parts[0].toFloat( &ok );
                    totalOk &= ok;
                    float y = parts[1].toFloat( &ok );
                    totalOk &= ok;
                    float z = parts[2].toFloat( &ok );
                    totalOk &= ok;
                    float w = parts[2].toFloat( &ok );
                    totalOk &= ok;
                    if ( !totalOk )
                    {
                        m_script[row].replace( column, QQuaternion( 0, 0, 0, 0 ) );
                    }
                    else
                    {
                        m_script[row].replace( column, QQuaternion( w, x, y, z ) );
                    }
                }
            }
            if ( column > 2 )
            {
                m_script[row].replace( column, text );
            }
            break;
        }
        case ScriptCommand::BEGIN_LOOP:
        {
            if ( column == 2 )
            {
                m_script[row].replace( column, text.toInt() );
            }
            break;
        }
        case ScriptCommand::END_LOOP:
        case ScriptCommand::BEGIN_BLOCK:
        case ScriptCommand::END_BLOCK:
            break;
        case ScriptCommand::COMMENT:
        {
            if ( column == 2 )
            {
                m_script[row].replace( column, text );
            }
            break;
        }
    }
}

void ScriptWidget::slotGlobalSelectChanged( int line, int prop, int data )
{
    m_script[line].replace( 2, data );
    m_lastGlobal = data;
}

void ScriptWidget::slotPropertySelectChanged( int line, int prop, int data )
{
    m_script[line].replace( 2, data );
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
    if ( m_script.size() == 1 )
    {
        // do not delete the last command
        return;
    }
    if ( row >= 0 && row < m_script.size() )
    {
        m_script.removeAt( row );
    }
    m_lastInsertedLine = row;
    rebuild();
}

void ScriptWidget::insertCommand( int row )
{
    if ( row >= 0 && row < m_script.size() )
    {
        QList<QVariant> line;
        line.push_back( true );
        line.push_back( (int)ScriptCommand::NONE );
        m_script.insert( row, line );
    }
    m_lastInsertedLine = row;
    rebuild();
}

void ScriptWidget::moveCommandUp( int row )
{
    if ( row == 0 )
    {
        return;
    }
    QList<QVariant> line = m_script.takeAt( row );
    m_script.insert( row - 1, line );
    rebuild();
}

void ScriptWidget::moveCommandDown( int row )
{
    if ( row == m_script.size() - 1 )
    {
        return;
    }
    QList<QVariant> line = m_script.takeAt( row );
    m_script.insert( row + 1, line );
    rebuild();
}

void ScriptWidget::moveScrollBarToBottom( int min, int max )
{
    Q_UNUSED( min );
    if ( m_widgetToEnsureVisible)
    {
        m_scrollArea->ensureWidgetVisible( m_widgetToEnsureVisible );
    }
    else
    {
        m_scrollArea->verticalScrollBar()->setValue( max );
    }
}

void ScriptWidget::resetScript()
{
    m_script.clear();
    QList<QVariant> line;
    line.push_back( true );
    line.push_back( (int)ScriptCommand::NONE );
    m_script.push_back( line );
    rebuild();
}

void ScriptWidget::slotCheckboxChanged( int line, int state )
{
    m_script[line].replace( 0, state );
    if ( m_script[line].at( 1 ).toInt() == (int)ScriptCommand::BEGIN_LOOP )
    {
        do
        {
            m_script[line].replace( 0, state );
            emit( checkBoxChanged( state, line ) );
            emit( enable2( state, line ) );
            emit( enable2( state, line * 10 ) );
            emit( enable( state, line * 10 + 1 ) );
            emit( enable( state, line * 10 + 2 ) );
            emit( enable( state, line * 10 + 3 ) );
            emit( enable( state, line * 10 + 4 ) );
            emit( enable( state, line * 10 + 5 ) );
            ++line;
        }
        while( m_script[line].at( 1 ).toInt() != (int)ScriptCommand::END_LOOP );
    }
    if ( m_script[line].at( 1 ).toInt() == (int)ScriptCommand::BEGIN_BLOCK )
    {
        do
        {
            emit( checkBoxChanged( state, line ) );
            emit( enable2( state, line ) );
            emit( enable2( state, line * 10 ) );
            emit( enable( state, line * 10 + 1 ) );
            emit( enable( state, line * 10 + 2 ) );
            emit( enable( state, line * 10 + 3 ) );
            emit( enable( state, line * 10 + 4 ) );
            emit( enable( state, line * 10 + 5 ) );
            m_script[line].replace( 0, state );
            ++line;
        }
        while( m_script[line].at( 1 ).toInt() != (int)ScriptCommand::END_BLOCK );
    }
    m_script[line].replace( 0, state );
    emit( checkBoxChanged( state, line ) );
    emit( enable2( state, line ) );
    emit( enable2( state, line * 10 ) );
    emit( enable( state, line * 10 + 1 ) );
    emit( enable( state, line * 10 + 2 ) );
    emit( enable( state, line * 10 + 3 ) );
    emit( enable( state, line * 10 + 4 ) );
    emit( enable( state, line * 10 + 5 ) );
}
