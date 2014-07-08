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

#include "controls/comboboxid.h"
#include "controls/lineeditid.h"
#include "controls/labelid.h"
#include "controls/checkbox.h"
#include "controls/sliderwitheditint.h"
#include "controls/pushbuttonwithid.h"

#include "../../algos/fmath.h"

#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/vptr.h"
#include "../../data/roi.h"
#include "../../data/roibox.h"
#include "../../data/datasets/dataset.h"
#include "../../data/properties/propertygroup.h"

#include <QDebug>

ScriptWidget::ScriptWidget( GLWidget* glWidget, QWidget* parent ) :
    m_glWidget( glWidget ),
    m_widgetToEnsureVisible ( 0 ),
    m_contRunning( 0 ),
    m_screenshotEach( 0 ),
    m_delay( 0 ),
    m_beginSlider( 0 ),
    m_endSlider( 0 ),
    m_scrollArea( 0 ),
    m_runButton( 0 ),

    m_lastInsertedLine( 0 ),
    m_runScript( false ),
    m_currentCommandLine( 0 ),

    m_lastGlobal( (int)Fn::Property::G_SAGITTAL ),
    m_lastDataset( 0 ),
    m_lastProperty( (int)Fn::Property::D_ACTIVE ),
    m_lastRoiBranch( 0 ),
    m_lastRoiRow( 0 ),

    m_loopCount( 1 ),
    m_totalLoops( 1 ),
    m_loopBegin( 0 ),
    m_inBlock( false ),
    m_inLoop( false ),
    m_render( false ),
    m_paused( false )
{
    m_commandLookUp = std::vector<int>( { 11, 10, 1, 2, 11, 3, 11, 0, 5, 6, 7, 8, 9, 4, 11 } );

    QList<QVariant> line;
    line.push_back( true );
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
    m_scriptPanel = m_scrollArea->takeWidget();
    delete m_scriptPanel->layout();
    // delete of old widget currently deactivated because of crash, possible connected to signals
    // i have no idea how to fix that
    //delete m_scriptPanel;

    m_scriptPanel = buildScriptLayout();
    m_scrollArea->setWidget( m_scriptPanel );

    moveScrollBarToBottom( 0, 0 );

    this->repaint();
}

void ScriptWidget::initLayout()
{
    m_widgetToEnsureVisible = 0;

    // need to initialize these two slider here
    m_beginSlider = new SliderWithEditInt( "begin", 0, this );
    m_beginSlider->setMin( 0 );
    m_beginSlider->setMax( qMax( 0, m_script.size() - 2 ) );
    m_beginSlider->setValue( 0 );

    m_endSlider = new SliderWithEditInt( "end", 0, this );
    m_endSlider->setMin( 0 );
    m_endSlider->setMax( qMax( 0, m_script.size() - 1 ) );
    m_endSlider->setValue( m_script.size() - 1  );

    m_buildRange = new CheckBox( "show only begin-end", -1, this );
    m_buildRange->setChecked( false );

    m_scriptPanel = buildScriptLayout();
    m_scrollArea = new QScrollArea( this );
    m_scrollArea->setWidget( m_scriptPanel );


    connect( m_beginSlider, SIGNAL( valueChanged( int, int ) ), this, SLOT( slotRangeChanged() ) );
    connect( m_endSlider, SIGNAL( valueChanged( int, int ) ), this, SLOT( slotRangeChanged() ) );

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

    QPushButton* appendButton = new QPushButton( "Append", this );
    buttons1->addWidget( appendButton );
    connect( appendButton, SIGNAL( clicked() ), this, SLOT( appendScript() ) );

    QPushButton* sbutton = new QPushButton( "Save", this );
    buttons1->addWidget( sbutton );
    connect( sbutton, SIGNAL( clicked() ), this, SLOT( saveScript() ) );

    m_runButton = new QPushButton( "Run", this );
    m_runButton->setCheckable( true );
    buttons1->addWidget( m_runButton );
    connect( m_runButton, SIGNAL( toggled( bool ) ), this, SLOT( run( bool ) ) );

    m_pauseButton = new QPushButton( "Pause", this );
    m_pauseButton->setCheckable( true );
    buttons1->addWidget( m_pauseButton );
    connect( m_pauseButton, SIGNAL( toggled( bool ) ), this, SLOT( pause( bool ) ) );

    buttons1->addStretch();

    QHBoxLayout* buttons2 = new QHBoxLayout();

    m_contRunning = new CheckBox( "continous", -1, this );
    m_contRunning->uncheck();
    buttons2->addWidget( m_contRunning );

    m_screenshotEach = new CheckBox( "screenshot each step", -1, this );
    m_screenshotEach->uncheck();
    buttons2->addWidget( m_screenshotEach );

    buttons2->addWidget( m_buildRange );
    connect( m_buildRange, SIGNAL( stateChanged( int, int ) ), this, SLOT( rebuild() ) );

    m_delay = new SliderWithEditInt( "delay", 0, this );
    m_delay->setMin( 1 );
    m_delay->setMax( 1000 );
    m_delay->setValue( 25 );

    m_loopSize = new SliderWithEditInt( "loop size", 0, this );
    m_loopSize->setMin( 1 );
    m_loopSize->setMax( 1000 );
    m_loopSize->setValue( 25 );


    buttons2->addStretch();
    buttons2->addStretch();
    buttons2->addStretch();
    buttons2->addWidget( m_loopSize );
    buttons1->addWidget( m_beginSlider );
    buttons1->addWidget( m_endSlider );
    buttons1->addWidget( m_delay );

    layout2->addLayout( buttons2 );
    layout2->addLayout( buttons1 );

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_scrollArea );
    layout->addLayout( layout2 );

    this->setLayout( layout );
    this->setMinimumSize( 300, 300 );

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

    int begin = m_beginSlider->getValue();
    int end = m_endSlider->getValue();

    for ( int i = 0; i < m_script.size(); ++i )
    {
        if ( m_buildRange->checked() && ( i < begin || i > end  ) )
        {
            continue;
        }

        QList<QVariant>line = m_script[i];

        scriptLayout->addLayout( addWidgetLine( i, line, inLoop, inBlock ) );
    }
    scriptLayout->addStretch();

    return groupBox;
}

ComboBoxID* ScriptWidget::createCommandSelect( int id )
{
    ComboBoxID* select = new ComboBoxID( id, this );
    int i = 0;
    select->insertItem( i++, Script2String::s( ScriptCommand::SET_ARCBALL ),      (int)ScriptCommand::SET_ARCBALL );
    select->insertItem( i++, Script2String::s( ScriptCommand::SET_CAMERA ),       (int)ScriptCommand::SET_CAMERA );
    select->insertItem( i++, Script2String::s( ScriptCommand::SET_GLOBAL ),       (int)ScriptCommand::SET_GLOBAL );
    select->insertItem( i++, Script2String::s( ScriptCommand::SET_PROPERTY ),     (int)ScriptCommand::SET_PROPERTY );
    select->insertItem( i++, Script2String::s( ScriptCommand::SET_ROI_PROPERTY ), (int)ScriptCommand::SET_ROI_PROPERTY );
    select->insertItem( i++, Script2String::s( ScriptCommand::BEGIN_LOOP ),       (int)ScriptCommand::BEGIN_LOOP );
    select->insertItem( i++, Script2String::s( ScriptCommand::END_LOOP ),         (int)ScriptCommand::END_LOOP );
    select->insertItem( i++, Script2String::s( ScriptCommand::BEGIN_BLOCK ),      (int)ScriptCommand::BEGIN_BLOCK );
    select->insertItem( i++, Script2String::s( ScriptCommand::END_BLOCK ),        (int)ScriptCommand::END_BLOCK );
    select->insertItem( i++, Script2String::s( ScriptCommand::COMMENT ),          (int)ScriptCommand::COMMENT );
    select->insertItem( i++, Script2String::s( ScriptCommand::DELAY ),            (int)ScriptCommand::DELAY );
    select->insertItem( i++, Script2String::s( ScriptCommand::NONE ),             (int)ScriptCommand::NONE );

    return select;
}

QHBoxLayout* ScriptWidget::addWidgetLine( int i, QList<QVariant> &line, bool &inLoop, bool &inBlock )
{
    QHBoxLayout* layout = new QHBoxLayout();

    QLabel* label = new QLabel( QString::number( i ), this );
    layout->addWidget( label );

    CheckBox* checkBox = new CheckBox( i, this );
    layout->addWidget( checkBox );
    bool lineActive = line[0].toBool();
    checkBox->setChecked( lineActive );
    connect( checkBox, SIGNAL( stateChanged( int, int ) ), this, SLOT( slotCheckboxChanged( int, int ) ) );
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

    CheckBox* checkBox2 = new CheckBox( i, this );
    layout->addWidget( checkBox2 );
    bool screenshotActive = line[1].toBool();
    checkBox2->setChecked( screenshotActive );
    connect( checkBox2, SIGNAL( stateChanged( int, int ) ), this, SLOT( slotCheckboxChanged2( int, int ) ) );

    if( ( line[2].toInt() == (int)ScriptCommand::BEGIN_LOOP ) || ( line[2].toInt() == (int)ScriptCommand::BEGIN_BLOCK ) )
    {
        checkBox2->setDisabled( true );
    }

    QColor lightRed( 255, 168, 168 );
    QColor lightBlue( 168, 168, 255 );
    QColor lightGreen( 168, 255, 168 );
    QColor lightPurple( 255, 158, 255 );
    QColor yellow( 255, 255, 0 );
    QColor magenta( 0, 255, 255 );
    QColor orange( 255, 160, 0 );

    if( inLoop && line[2].toInt() != (int)ScriptCommand::END_LOOP )
    {
        checkBox2->setDisabled( true );
        QLabel* label = new QLabel( "loop", this );
        label->setStyleSheet("QLabel { background-color : " + lightRed.name() + "}");
        layout->addWidget( label );
    }
    if( inBlock && line[2].toInt() != (int)ScriptCommand::END_BLOCK )
    {
        checkBox2->setDisabled( true );
        QLabel* label = new QLabel( "block", this );
        label->setStyleSheet("QLabel { background-color : " + lightBlue.name() + "}");
        layout->addWidget( label );
    }

    ComboBoxID* select = createCommandSelect( i * 10 );

    layout->addWidget( select );
    select->setCurrentIndex( m_commandLookUp[ line[2].toInt() ] );
    connect( select, SIGNAL( currentIndexChanged( int, int, int ) ), this, SLOT( commandChanged( int, int, int ) ) );
    connect( this, SIGNAL( enable2( bool, int ) ), select, SLOT( setEnabled2( bool, int ) ) );

    switch( (ScriptCommand)( line[2].toInt() ) )
    {
        case ScriptCommand::NONE:
            addStretch( layout, 5 );
            break;
        case ScriptCommand::DELAY:
        {
            select->setStyleSheet("QComboBox { background-color : " + orange.name() + "}");
            addStretch( layout, 5 );
            break;
        }
        case ScriptCommand::SET_CAMERA:
        {
            select->setStyleSheet("QComboBox { background-color : " + yellow.name() + "}");
            addEdit( layout, i* 10 + 1, 3 );
            QVector3D vec = line[3].value<QVector3D>();
            emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 1 ) );
            vec = line[4].value<QVector3D>();
            emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 2 ) );
            vec = line[5].value<QVector3D>();
            emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ), i * 10 + 3 ) );
            addStretch( layout, 2 );
            break;
        }
        case ScriptCommand::SET_GLOBAL:
        {
            select->setStyleSheet("QComboBox { background-color : " + lightGreen.name() + "}");
            addGlobalSelect( layout, i, line[3].toInt(), lineActive );

            addEdit( layout, i* 10 + 2, 1 );
            if ( line[3].toInt() < (int)Fn::Property::G_BACKGROUND_COLOR_MAIN )
            {
                emit( editChanged( line[4].toString(), i * 10 + 2 ) );
            }
            else
            {
                QColor vec = line[4].value<QColor>();
                emit( editChanged( QString::number( vec.red() ) + ", " + QString::number( vec.green() ) + ", " + QString::number( vec.blue() ), i * 10 + 2 ) );
            }
            addStretch( layout, 3 );
            break;
        }
        case ScriptCommand::INC_GLOBAL:
        {
            select->setStyleSheet("QComboBox { background-color : " + lightGreen.name() + "}");
            addGlobalSelect( layout, i, line[3].toInt(), lineActive );
            addEdit( layout, i* 10 + 2, 1 );
            emit( editChanged( line[4].toString(), i * 10 + 2 ) );
            addStretch( layout, 3 );
            break;
        }

        case ScriptCommand::SET_PROPERTY:
        {
            select->setStyleSheet("QComboBox { background-color : " + lightPurple.name() + "}");
            addPropertySelect( layout, i, line[3].toInt(), line[4].toInt(), lineActive );
            addEdit( layout, i* 10 + 2, 2 );
            emit( editChanged( line[4].toString(), i * 10 + 2 ) );
            emit( editChanged( line[5].toString(), i * 10 + 3 ) );
            addStretch( layout, 2 );
            break;
        }
        case ScriptCommand::INC_PROPERTY:
        {
            select->setStyleSheet("QComboBox { background-color : " + lightPurple.name() + "}");
            addPropertySelect( layout, i, line[3].toInt(), line[4].toInt(), lineActive );
            addEdit( layout, i* 10 + 2, 2 );
            emit( editChanged( line[4].toString(), i * 10 + 2 ) );
            emit( editChanged( line[5].toString(), i * 10 + 3 ) );
            addStretch( layout, 2 );
            break;
        }
        case ScriptCommand::SET_ARCBALL:
        {
            select->setStyleSheet("QComboBox { background-color : " + yellow.name() + "}");
            addEdit( layout, i* 10 + 1, 4 );
            QVector4D vec = line[3].value<QQuaternion>().toVector4D();
            emit( editChanged( QString::number( vec.x() ) + ", " + QString::number( vec.y() ) + ", " + QString::number( vec.z() ) + ", " + QString::number( vec.w() ), i * 10 + 1 ) );
            emit( editChanged( line[4].toString(), i * 10 + 2 ) );
            emit( editChanged( line[5].toString(), i * 10 + 3 ) );
            emit( editChanged( line[6].toString(), i * 10 + 4 ) );
            addStretch( layout, 1 );
            break;
        }
        case ScriptCommand::BEGIN_LOOP:
        {
            select->setStyleSheet("QComboBox { background-color : " + lightRed.name() + "}");
            addEdit( layout, i* 10 + 1, 1 );
            emit( editChanged( line[3].toString(), i * 10 + 1 ) );
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
            emit( editChanged( line[3].toString(), i * 10 + 1 ) );
            break;
        }
        case ScriptCommand::SET_ROI_PROPERTY:
        {
            select->setStyleSheet("QComboBox { background-color : " + magenta.name() + "}");
            addRoiPropertySelect( layout, i, line[3].toInt(), line[4].toInt(), line[5].toInt(), lineActive );
            addEdit( layout, i* 10 + 2, 3 );
            emit( editChanged( line[4].toString(), i * 10 + 2 ) );
            emit( editChanged( line[5].toString(), i * 10 + 3 ) );
            emit( editChanged( line[6].toString(), i * 10 + 4 ) );
            addStretch( layout, 1 );
            break;
        }
        case ScriptCommand::INC_ROI_PROPERTY:
        {
            select->setStyleSheet("QComboBox { background-color : " + magenta.name() + "}");
            addRoiPropertySelect( layout, i, line[3].toInt(), line[4].toInt(), line[5].toInt(), lineActive );
            addEdit( layout, i* 10 + 2, 3 );
            emit( editChanged( line[4].toString(), i * 10 + 2 ) );
            emit( editChanged( line[5].toString(), i * 10 + 3 ) );
            emit( editChanged( line[6].toString(), i * 10 + 4 ) );
            addStretch( layout, 1 );
            break;
        }
    }

    LabelID* indicator = new LabelID( i, "<---", this );
    indicator->setHidden( true );
    connect( this, SIGNAL( hideIndicator( bool, int ) ), indicator, SLOT( slotSetHidden( bool, int ) ) );
    layout->addWidget( indicator );

    select->setEnabled( lineActive );
    emit( enable( lineActive, i * 10 + 1 ) );
    emit( enable( lineActive, i * 10 + 2 ) );
    emit( enable( lineActive, i * 10 + 3 ) );
    emit( enable( lineActive, i * 10 + 4 ) );
    emit( enable( lineActive, i * 10 + 5 ) );

    layout->addStretch();

    return layout;
}

void ScriptWidget::commandChanged( int line, int index, int command )
{
    line /= 10;
    QList<QVariant> commandLine;
    commandLine.push_back( true );
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
            commandLine.push_back( Models::getGlobal( m_lastGlobal ) );
            break;
        }
        case ScriptCommand::INC_GLOBAL:
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
        case ScriptCommand::INC_PROPERTY:
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
            commandLine.push_back( m_loopSize->getValue() );
            QList<QVariant> commandLine2;
            commandLine2.push_back( true );
            commandLine2.push_back( true );
            commandLine2.push_back( (int)ScriptCommand::NONE );
            m_script.insert( line + 1, commandLine2 );
            QList<QVariant> commandLine3;
            commandLine3.push_back( true );
            commandLine3.push_back( true );
            commandLine3.push_back( (int)ScriptCommand::END_LOOP );
            m_script.insert( line + 2, commandLine3 );
            if ( line + 2 == m_script.size() - 1 )
            {
                QList<QVariant> command4;
                command4.push_back( true );
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
            commandLine2.push_back( true );
            commandLine2.push_back( (int)ScriptCommand::NONE );
            m_script.insert( line + 1, commandLine2 );
            QList<QVariant> commandLine3;
            commandLine3.push_back( true );
            commandLine3.push_back( true );
            commandLine3.push_back( (int)ScriptCommand::END_BLOCK );
            m_script.insert( line + 2, commandLine3 );
            if ( line + 2 == m_script.size() - 1 )
            {
                QList<QVariant> command4;
                command4.push_back( true );
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
        case ScriptCommand::SET_ROI_PROPERTY:
        {
            commandLine.push_back( (int)Fn::Property::D_ACTIVE );
            commandLine.push_back( m_lastRoiBranch );
            commandLine.push_back( m_lastRoiRow );
            commandLine.push_back( 1 );
            break;
        }
        case ScriptCommand::INC_ROI_PROPERTY:
        {
            commandLine.push_back( (int)Fn::Property::D_ACTIVE );
            commandLine.push_back( m_lastRoiBranch );
            commandLine.push_back( m_lastRoiRow );
            commandLine.push_back( 1 );
            break;
        }
    }
    m_script[line] = commandLine;
    if ( line == m_script.size() - 1 && command != 0 )
    {
        QList<QVariant> command;
        command.push_back( true );
        command.push_back( true );
        command.push_back( (int)ScriptCommand::NONE );
        m_script.push_back( command );
    }
    m_beginSlider->setMax( qMax( 0, m_script.size() - 2 ) );
    m_endSlider->setMax( qMax( 0, m_script.size() - 1 ) );
    m_endSlider->setValue( qMax( 0, m_script.size() - 1 ) );
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
        PropertyGroup props = ds->properties();
        int toSelect = 0;

        for ( int i = 0; i < props.size(); ++i )
        {
            QPair<Fn::Property, Property*> prop = props.getNthPropertyPair( i );
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

void ScriptWidget::addRoiPropertySelect( QHBoxLayout* layout, int id, int selected, int branch, int row, bool active )
{
    ComboBoxID* select = new ComboBoxID( id, this );

    ROI* roi = Models::getRoi( branch, row );
    if ( dynamic_cast<ROIBox*>( roi) )
    {
        PropertyGroup* props = roi->properties();
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

void ScriptWidget::loadScript( QString fileName, bool append )
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

    bool oldScript = false;
    if ( settings.value( "version" ).toString() == "0.7.0" )
    {
        oldScript = true;
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

    if ( !append )
    {
        m_script.clear();
    }
    else
    {
        if ( lastIsNone() )
        {
            m_script.takeLast();
        }
    }

    for ( int i = 0; i < size; ++i )
    {
        if( settings.contains( QString::number( i ) ) )
        {
            QList<QVariant> commandLine = settings.value( QString::number( i ) ).toList();
            if ( oldScript )
            {
                commandLine.insert( 1, true );
            }
            m_script.push_back( commandLine );
        }
        else
        {
            qDebug() << "*** ERROR *** script file corrupt.";
        }
    }
    m_beginSlider->setMax( qMax( 0, m_script.size() - 2 ) );
    m_endSlider->setMax( qMax( 0, m_script.size() - 1 ) );
    m_endSlider->setValue( qMax( 0, m_script.size() - 1 ) );
    rebuild();
    if ( settings.contains( "delay" ) )
    {
        m_delay->setValue( settings.value( "delay").toInt() );
    }
    if ( settings.contains( "continuous" ) )
    {
        m_contRunning->setChecked( settings.value( "continuous").toBool() );
    }
}

void ScriptWidget::loadScript()
{
    QString fn = Models::getGlobal( (int)Fn::Property::G_LAST_PATH ).toString();
    QString fileName = QFileDialog::getOpenFileName( this, "Open File", fn );
    loadScript( fileName, false );
}

void ScriptWidget::appendScript()
{
    QString fn = Models::getGlobal( (int)Fn::Property::G_LAST_PATH ).toString();
    QString fileName = QFileDialog::getOpenFileName( this, "Open File", fn );
    loadScript( fileName, true );
}

void ScriptWidget::saveScript( QString fileName )
{
    if ( m_script.size() == 0 )
    {
        qDebug() << "script size is zero, no file saved ";
        return;
    }
    QSettings settings( fileName, QSettings::IniFormat );
    settings.clear();
    settings.setValue( "appName", "braingl" );
    settings.setValue( "version", "0.7.1" );
    settings.setValue( "fileType", "script" );
    int begin = m_beginSlider->getValue();
    int end = m_endSlider->getValue();
    settings.setValue( "size", end - begin + 1 );
    settings.setValue( "delay", m_delay->getValue() );
    settings.setValue( "continuous", m_contRunning->checked() );
    int lineIndex = 0;
    for ( int i = begin; i <= end; ++i )
    {
        settings.setValue( QString::number( lineIndex++ ), m_script[i] );
    }
}

void ScriptWidget::saveScript()
{
    QString fn = Models::getGlobal( (int)Fn::Property::G_LAST_PATH ).toString();
    QString fileName = QFileDialog::getSaveFileName( this, "Save File", fn );
    if ( !fileName.endsWith( ".bgscript" ) )
    {
        fileName += ".bgscript";
    }
    saveScript( fileName );
}

void ScriptWidget::pause( bool checked )
{
    m_paused = checked;
    if ( m_paused )
    {
        qDebug() << "pause script";
    }
    else
    {
        qDebug() << "continue script";
        run();
    }
}

void ScriptWidget::run( bool checked )
{
    m_inBlock = false;
    m_loopCount = 1;
    m_totalLoops = 1;
    m_inLoop = false;
    m_render = false;
    m_paused = m_pauseButton->isChecked();

    if ( checked )
    {
        for ( int i = 0; i < m_script.size(); ++i )
        {
            emit( hideIndicator( true, i ) );
        }
        qDebug() << "start script";
        m_currentCommandLine = m_beginSlider->getValue();
        m_runScript = true;
        run();
    }
    else
    {
        qDebug() << "stop script";
        m_runScript = false;
        m_pauseButton->setChecked( false );
    }
}

void ScriptWidget::run()
{
    if ( !m_runScript || m_paused )
    {
        return;
    }

    emit( hideIndicator( true, m_currentCommandLine - 1 ) );
    if ( m_currentCommandLine > m_endSlider->getValue() )
    {
        if( m_contRunning->checked() )
        {
            m_currentCommandLine = m_beginSlider->getValue();
        }
        else
        {
            m_runButton->setChecked( false );
            return;
        }
    }

    emit( hideIndicator( false, m_currentCommandLine ) );
    QList<QVariant> line = m_script[m_currentCommandLine++];

    if ( !line[0].toBool() )
    {
        QTimer::singleShot( 1, this, SLOT( run() ) );
        return;
    }

    int delay = m_delay->getValue();
    int command = line[2].toInt();

    QList<QVariant>camera = m_glWidget->getCamera()->getState();


    switch( (ScriptCommand)( command ) )
    {
        case ScriptCommand::NONE:
        case ScriptCommand::COMMENT:
        {
                emit( hideIndicator( true, m_currentCommandLine - 1 ) );
                QTimer::singleShot( 1, this, SLOT( run() ) );
                return;
        }
        case ScriptCommand::DELAY:
        {
            break;
        }
        case ScriptCommand::SET_CAMERA:
        {
            if ( m_inLoop && m_loopCount == m_totalLoops )
            {
                QList<QVariant>loopLine;
                loopLine.push_back( "c" );
                loopLine.push_back( camera[0] );
                loopLine.push_back( camera[1] );
                loopLine.push_back( camera[2] );
                loopLine.push_back( line[3] );
                loopLine.push_back( line[4] );
                loopLine.push_back( line[5] );
                m_loopList.push_back( loopLine );

            }
            else
            {
                QList<QVariant>newCam;
                newCam.push_back( line[3].value<QVector3D>() );
                newCam.push_back( line[4].value<QVector3D>() );
                newCam.push_back( line[5].value<QVector3D>() );
                m_glWidget->getCamera()->setState( newCam );
            }
            break;

        }
        case ScriptCommand::SET_GLOBAL:
        {
            if ( m_inLoop && m_loopCount == m_totalLoops )
            {
                int lastGlobal = line[3].toInt();
                QVariant currentValue = Models::getGlobal( lastGlobal );
                QVariant targetValue = line[4];
                QList<QVariant>loopLine;
                loopLine.push_back( "g" );
                loopLine.push_back( lastGlobal );
                loopLine.push_back( currentValue );
                loopLine.push_back( targetValue );
                m_loopList.push_back( loopLine );
            }
            else if ( !m_inLoop )
            {
                Models::setGlobal( line[3].toInt(), line[4] );
            }
            break;
        }
        case ScriptCommand::INC_GLOBAL:
        {
            m_lastGlobal = line[3].toInt();
            float stepSize = line[4].toFloat();
            float value = Models::getGlobal( m_lastGlobal ).toFloat() + stepSize;
            Models::setGlobal( m_lastGlobal, value );
            break;
        }
        case ScriptCommand::SET_PROPERTY:
        {
            if ( m_inLoop && m_loopCount == m_totalLoops )
            {
                int lastDataset = line[4].toInt();
                int lastProperty = line[3].toInt();
                QVariant currentValue = Models::d()->data( Models::d()->index( lastDataset, lastProperty ) );
                QVariant targetValue = line[5];
                QList<QVariant>loopLine;
                loopLine.push_back( "d" );
                loopLine.push_back( lastDataset );
                loopLine.push_back( lastProperty );
                loopLine.push_back( currentValue );
                loopLine.push_back( targetValue );
                m_loopList.push_back( loopLine );
            }
            else if ( !m_inLoop )
            {
                Models::d()->setData( Models::d()->index( line[4].toInt(), line[3].toInt() ), line[5], Qt::DisplayRole );
            }
            break;
        }
        case ScriptCommand::INC_PROPERTY:
        {
            m_lastDataset = line[4].toInt();
            m_lastProperty = line[3].toInt();
            float stepSize = line[5].toFloat();
            float value = Models::d()->data( Models::d()->index( m_lastDataset, m_lastProperty ) ).toFloat() + stepSize;
            Models::d()->setData( Models::d()->index( m_lastDataset, m_lastProperty ), value, Qt::DisplayRole );
            break;
        }
        case ScriptCommand::SET_ARCBALL:
        {
            if ( m_inLoop && m_loopCount == m_totalLoops )
            {
                QList<QVariant>loopLine;
                loopLine.push_back( "a" );
                loopLine.push_back( m_glWidget->getArcBall()->getRotation() );
                loopLine.push_back( m_glWidget->getArcBall()->getZoom() );
                loopLine.push_back( m_glWidget->getArcBall()->getMoveX() );
                loopLine.push_back( m_glWidget->getArcBall()->getMoveY() );
                loopLine.push_back( line[3] );
                loopLine.push_back( line[4] );
                loopLine.push_back( line[5] );
                loopLine.push_back( line[6] );
                m_loopList.push_back( loopLine );
            }
            else if ( !m_inLoop )
            {
                m_glWidget->getArcBall()->setRotation( line[3].value<QQuaternion>() );
                m_glWidget->getArcBall()->setZoom( line[4].toFloat() );
                m_glWidget->getArcBall()->setMoveX( line[5].toFloat() );
                m_glWidget->getArcBall()->setMoveY( line[6].toFloat() );
            }
            break;
        }
        case ScriptCommand::BEGIN_LOOP:
        {
            m_loopList.clear();
            m_loopCount = line[3].toInt();
            m_totalLoops = line[3].toInt();
            m_loopBegin = m_currentCommandLine;
            m_inLoop = true;
            break;
        }
        case ScriptCommand::END_LOOP:
        {
            --m_loopCount;
            float div = (float)( m_totalLoops - m_loopCount ) / (float)m_totalLoops;
            if ( m_loopCount + 1 )
            {
                m_currentCommandLine = m_loopBegin;

                for ( int i = 0; i < m_loopList.size(); ++i )
                {
                    QList<QVariant>loopLine = m_loopList[i];
                    if ( loopLine[0] == "g" )
                    {
                        Models::setGlobal( loopLine[1].toInt(), FMath::interpolateQVariant( loopLine[2], loopLine[3], div ) );
                    }
                    if ( loopLine[0] == "d" )
                    {
                        //qDebug() << loopLine[3] <<  loopLine[4] << div << FMath::interpolateQVariant( loopLine[3], loopLine[4], div ).toInt();
                        Models::d()->setData( Models::d()->index( loopLine[1].toInt(), loopLine[2].toInt() ), FMath::interpolateQVariant( loopLine[3], loopLine[4], div ), Qt::DisplayRole );
                    }
                    if ( loopLine[0] == "r" )
                    {
                        Models::setROIProp( loopLine[1].toInt(), loopLine[2].toInt(), loopLine[3].toInt(), FMath::interpolateQVariant( loopLine[4], loopLine[5], div ) );
                    }
                    if ( loopLine[0] == "c" )
                    {
                        QList<QVariant>newCam;
                        newCam.push_back( FMath::interpolateQVariant( loopLine[1], loopLine[4], div ) );
                        newCam.push_back( FMath::interpolateQVariant( loopLine[2], loopLine[5], div ) );
                        newCam.push_back( FMath::interpolateQVariant( loopLine[3], loopLine[6], div ) );
                        m_glWidget->getCamera()->setState( newCam );
                    }
                    if ( loopLine[0] == "a" )
                    {
                        QQuaternion currentRot = loopLine[1].value<QQuaternion>();
                        QQuaternion targetRot = loopLine[5].value<QQuaternion>();
                        QQuaternion rot = QQuaternion::slerp( currentRot, targetRot, div );
                        m_glWidget->getArcBall()->setRotation( rot );

                        m_glWidget->getArcBall()->setZoom( FMath::interpolateQVariant( loopLine[2], loopLine[6], div ).toFloat() );
                        m_glWidget->getArcBall()->setMoveX( FMath::interpolateQVariant( loopLine[3], loopLine[7], div ).toFloat() );
                        m_glWidget->getArcBall()->setMoveY( FMath::interpolateQVariant( loopLine[4], loopLine[8], div ).toFloat() );

                    }
                }
                m_render = true;
            }
            else
            {
                m_loopCount = 1;
                m_totalLoops = 1;
                m_inLoop = false;
                emit( hideIndicator( true, m_currentCommandLine - 1 ) );
                QTimer::singleShot( 1, this, SLOT( run() ) );
                return;
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
        case ScriptCommand::SET_ROI_PROPERTY:
        {
            ROI* roi = Models::getRoi( line[4].toInt(), line[5].toInt() );


            if ( m_inLoop && m_loopCount == m_totalLoops )
            {
                if ( dynamic_cast<ROIBox*>( roi) )
                {
                    int branch = line[4].toInt();
                    int row = line[5].toInt();
                    int lastProperty = line[3].toInt();
                    PropertyGroup* props = roi->properties();
                    float currentValue = props->get( (Fn::Property)line[3].toInt() ).toFloat() ;
                    float targetValue = line[6].toFloat();
                    QList<QVariant>loopLine;
                    loopLine.push_back( "r" );
                    loopLine.push_back( branch );
                    loopLine.push_back( row );
                    loopLine.push_back( lastProperty );
                    loopLine.push_back( currentValue );
                    loopLine.push_back( targetValue );
                    m_loopList.push_back( loopLine );
                }
            }
            else if ( !m_inLoop )
            {
                if ( dynamic_cast<ROIBox*>( roi) )
                {
                    Models::r()->setData( Models::createRoiIndex( line[4].toInt(), line[5].toInt(), line[3].toInt() ), line[6], Qt::DisplayRole );
                }
            }


            break;
        }
        case ScriptCommand::INC_ROI_PROPERTY:
        {
            ROI* roi = Models::getRoi( line[4].toInt(), line[5].toInt() );
            if ( dynamic_cast<ROIBox*>( roi) )
            {
                PropertyGroup* props = roi->properties();
                float value = props->get( (Fn::Property)line[3].toInt() ).toFloat() + line[6].toFloat();
                Models::r()->setData( Models::createRoiIndex( line[4].toInt(), line[5].toInt(), line[3].toInt() ), value, Qt::DisplayRole );
            }
            break;
        }
    }

    if ( m_inBlock || ( m_inLoop && !m_render ) )
    {
        emit( hideIndicator( true, m_currentCommandLine - 1 ) );
        QTimer::singleShot( 1, this, SLOT( run() ) );
        return;
    }

    if ( m_screenshotEach->checked() && line[1].toBool() )
    {
        emit( screenshot() );
    }
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    Models::g()->submit();
    qint64 endTime = QDateTime::currentMSecsSinceEpoch();
    int timeSpent = endTime - startTime;
    delay = qMax( 1, delay - timeSpent );

    m_render = false;

    QTimer::singleShot( delay, this, SLOT( run() ) );
}

void ScriptWidget::slotEditChanged( QString text, int id )
{
    int row = id / 10;
    int column = ( id % 10 ) + 2;

    int command = m_script[row].at( 2 ).toInt();
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
            m_script[row].replace( column, string2Vector3D( text ) );
            break;
        }
        case ScriptCommand::SET_GLOBAL:
        {
            if ( column == 4 )
            {
                if ( m_script[row].at( 3 ).toInt() < (int)Fn::Property::G_BACKGROUND_COLOR_MAIN )
                {
                    m_script[row].replace( column, text );
                }
                else
                {
                    QVector3D vec = string2Vector3D( text );
                    m_script[row].replace( column, QColor( vec.x(), vec.y(), vec.z() ) );
                }
            }
            break;
        }
        case ScriptCommand::INC_GLOBAL:
        {
            break;
        }
        case ScriptCommand::SET_PROPERTY:
        {
            if ( column == 4 )
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
            if ( column == 5 )
            {
                if ( m_script[row].at( 3 ).toInt() == (int)Fn::Property::D_COLOR )
                {
                    QVector3D vec = string2Vector3D( text );
                    m_script[row].replace( column, QColor( vec.x(), vec.y(), vec.z() ) );
                }
                else
                {
                    m_script[row].replace( column, text );
                }
            }
            break;
        }
        case ScriptCommand::INC_PROPERTY:
        {
            break;
        }
        case ScriptCommand::SET_ARCBALL:
        {
            if ( column == 3 )
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
                    float w = parts[3].toFloat( &ok );
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
            if ( column > 3 )
            {
                m_script[row].replace( column, text );
            }
            break;
        }
        case ScriptCommand::BEGIN_LOOP:
        {
            if ( column == 3 )
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
            m_script[row].replace( column, text );
            break;
        }
        case ScriptCommand::SET_ROI_PROPERTY:
        {
            m_script[row].replace( column, text );
            break;
        }
        case ScriptCommand::INC_ROI_PROPERTY:
        {
            break;
        }
    }
}

void ScriptWidget::slotGlobalSelectChanged( int line, int prop, int data )
{
    m_script[line].replace( 3, data );
    m_lastGlobal = data;
}

void ScriptWidget::slotPropertySelectChanged( int line, int prop, int data )
{
    m_script[line].replace( 3, data );
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
    m_beginSlider->setMax( qMax( 0, m_script.size() - 2 ) );
    m_endSlider->decrement();
    m_endSlider->setMax( qMax( 0, m_script.size() - 1 ) );
    rebuild();
}

void ScriptWidget::insertCommand( int row )
{
    if ( row >= 0 && row < m_script.size() )
    {
        QList<QVariant> line;
        line.push_back( true );
        line.push_back( true );
        line.push_back( (int)ScriptCommand::NONE );
        m_script.insert( row, line );
    }
    m_lastInsertedLine = row;
    m_beginSlider->setMax( qMax( 0, m_script.size() - 2 ) );
    m_endSlider->setMax( qMax( 0, m_script.size() - 1 ) );
    m_endSlider->increment();
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
    if ( m_widgetToEnsureVisible )
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
    line.push_back( true );
    line.push_back( (int)ScriptCommand::NONE );
    m_script.push_back( line );
    rebuild();
}

void ScriptWidget::slotCheckboxChanged( int state, int line )
{
    m_script[line].replace( 0, state );
    if ( m_script[line].at( 2 ).toInt() == (int)ScriptCommand::BEGIN_LOOP )
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
        while( m_script[line].at( 2 ).toInt() != (int)ScriptCommand::END_LOOP );
    }
    if ( m_script[line].at( 2 ).toInt() == (int)ScriptCommand::BEGIN_BLOCK )
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
        while( m_script[line].at( 2 ).toInt() != (int)ScriptCommand::END_BLOCK );
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

void ScriptWidget::slotCheckboxChanged2( int state, int line )
{
    m_script[line].replace( 1, state );
}

void ScriptWidget::slotKeyPressed( int key, Qt::KeyboardModifiers mods )
{
    if ( key == 32 )
    {
        if ( mods & Qt::ShiftModifier )
        {
            m_runButton->toggle();
        }
        else
        {
            m_pauseButton->toggle();
        }
    }
}

void ScriptWidget::slotRangeChanged()
{
    if ( m_buildRange->checked() )
    {
        rebuild();
    }
}

void ScriptWidget::slotCopyCamera( int mode )
{
    QList<QVariant> camera = m_glWidget->getCamera()->getState();
    QList<QVariant> arcball = m_glWidget->getArcBall()->getState();

    switch ( mode )
    {
        case 0: // copy camera
        {
            QList<QVariant>commandLine;

            commandLine.push_back( true );
            commandLine.push_back( true );
            commandLine.push_back( (int)ScriptCommand::SET_CAMERA );

            commandLine.push_back( camera[0] );
            commandLine.push_back( camera[1] );
            commandLine.push_back( camera[2] );

            if ( lastIsNone() )
            {
                m_script.insert( m_script.size() - 1, commandLine );
            }
            else
            {
                m_script.push_back( commandLine );
            }

            m_beginSlider->setMax( qMax( 0, m_script.size() - 2 ) );
            m_endSlider->setMax( qMax( 0, m_script.size() - 1 ) );
            m_endSlider->increment();

        }
            break;
        case 1: // copy camera with loop
        {
            QList<QVariant>commandLine0;
            QList<QVariant>commandLine1;
            QList<QVariant>commandLine2;

            commandLine0.push_back( true );
            commandLine0.push_back( true );
            commandLine0.push_back( (int)ScriptCommand::BEGIN_LOOP );
            commandLine0.push_back( m_loopSize->getValue() );

            commandLine1.push_back( true );
            commandLine1.push_back( true );
            commandLine1.push_back( (int)ScriptCommand::SET_CAMERA );
            commandLine1.push_back( camera[0] );
            commandLine1.push_back( camera[1] );
            commandLine1.push_back( camera[2] );

            commandLine2.push_back( true );
            commandLine2.push_back( true );
            commandLine2.push_back( (int)ScriptCommand::END_LOOP );

            if ( lastIsNone() )
            {
                m_script.insert( m_script.size() - 1, commandLine0 );
                m_script.insert( m_script.size() - 1, commandLine1 );
                m_script.insert( m_script.size() - 1, commandLine2 );
            }
            else
            {
                m_script.push_back( commandLine0 );
                m_script.push_back( commandLine1 );
                m_script.push_back( commandLine2 );
            }

            m_beginSlider->setMax( qMax( 0, m_script.size() - 2 ) );
            m_endSlider->setMax( qMax( 0, m_script.size() - 1 ) );
            m_endSlider->increment();
            m_endSlider->increment();
            m_endSlider->increment();

        }
            break;
        case 2: // copy arcball
        {
            QList<QVariant>commandLine;

            commandLine.push_back( true );
            commandLine.push_back( true );
            commandLine.push_back( (int)ScriptCommand::SET_ARCBALL );

            commandLine.push_back( m_glWidget->getArcBall()->getRotation() );
            commandLine.push_back( arcball[8] );
            commandLine.push_back( arcball[2] );
            commandLine.push_back( arcball[3] );

            if ( lastIsNone() )
            {
                m_script.insert( m_script.size() - 1, commandLine );
            }
            else
            {
                m_script.push_back( commandLine );
            }

            m_beginSlider->setMax( qMax( 0, m_script.size() - 2 ) );
            m_endSlider->setMax( qMax( 0, m_script.size() - 1 ) );
            m_endSlider->increment();

        }
            break;
        case 3: // copy arcball with loop
        {
            QList<QVariant>commandLine0;
            QList<QVariant>commandLine1;
            QList<QVariant>commandLine2;

            commandLine0.push_back( true );
            commandLine0.push_back( true );
            commandLine0.push_back( (int)ScriptCommand::BEGIN_LOOP );
            commandLine0.push_back( m_loopSize->getValue() );

            commandLine1.push_back( true );
            commandLine1.push_back( true );
            commandLine1.push_back( (int)ScriptCommand::SET_ARCBALL );
            commandLine1.push_back( m_glWidget->getArcBall()->getRotation() );
            commandLine1.push_back( arcball[8] );
            commandLine1.push_back( arcball[2] );
            commandLine1.push_back( arcball[3] );

            commandLine2.push_back( true );
            commandLine2.push_back( true );
            commandLine2.push_back( (int)ScriptCommand::END_LOOP );

            if ( lastIsNone() )
            {
                m_script.insert( m_script.size() - 1, commandLine0 );
                m_script.insert( m_script.size() - 1, commandLine1 );
                m_script.insert( m_script.size() - 1, commandLine2 );
            }
            else
            {
                m_script.push_back( commandLine0 );
                m_script.push_back( commandLine1 );
                m_script.push_back( commandLine2 );
            }

            m_beginSlider->setMax( qMax( 0, m_script.size() - 2 ) );
            m_endSlider->setMax( qMax( 0, m_script.size() - 1 ) );
            m_endSlider->increment();
            m_endSlider->increment();
            m_endSlider->increment();

        }
            break;
    }

    rebuild();
}

bool ScriptWidget::lastIsNone()
{
    return ( m_script.last().at( 2 ).toInt() == 0 );
}
