/*
 * checkboxwithlabel.cpp
 *
 * Created on: 14.07.2012
 * @author Ralph Schurade
 */
#include "checkbox.h"

#include <QCheckBox>


CheckBox::CheckBox( QString label, int id, QWidget* parent ) :
    PropertyWidget( label, id, parent )
{
    QCheckBox* checkBox = new QCheckBox( this );
    m_controlWidget = checkBox;

    connect( checkBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotStateChanged( int ) ) );

    setLabelPosition( left );
    setup();
}

CheckBox::CheckBox( int id, QWidget* parent ) :
    PropertyWidget( "", id, parent )
{
    showLabel( false );
    QCheckBox* checkBox = new QCheckBox( this );
    m_controlWidget = checkBox;

    connect( checkBox, SIGNAL( stateChanged( int ) ), this, SLOT( slotStateChanged( int ) ) );

    setLabelPosition( left );
    setup();
}



CheckBox::~CheckBox()
{
}

void CheckBox::slotStateChanged( int state )
{
    emit( stateChanged( state, m_id ) );
}

void CheckBox::setChecked( bool state )
{
    dynamic_cast<QCheckBox*>( m_controlWidget )->setChecked( state );
}

void CheckBox::check()
{
    setChecked( true );
}

void CheckBox::uncheck()
{
    setChecked( false );
}


bool CheckBox::checked()
{
    return dynamic_cast<QCheckBox*>( m_controlWidget )->checkState();
}

void CheckBox::slotSetChecked2( int state, int id )
{
    if ( id == m_id )
    {
        setChecked( state );
    }
}
