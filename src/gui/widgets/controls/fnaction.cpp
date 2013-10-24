/*
 * fnaction.cpp
 *
 * Created on: Jan 4, 2013
 * @author Ralph Schurade
 */

#include "fnaction.h"

FNAction::FNAction( const QIcon &icon, const QString &text, QObject *parent, Fn::Algo algo ) :
    QAction( icon, text, parent ),
    m_algo( algo )
{
    connect( this, SIGNAL( triggered() ), this, SLOT( slotTriggered() ) );
}

FNAction::~FNAction()
{
}

void FNAction::slotTriggered()
{
    emit( sigTriggered( m_algo ) );
}
