/*
 * shadereditwidget.cpp
 *
 *  Created on: Nov 20, 2012
 *      Author: schurade
 */
#include "shadereditwidget.h"

#include "../gl/glfunctions.h"

#include <QtCore/QDebug>

ShaderEditWidget::ShaderEditWidget( QWidget* parent ) :
    QWidget( parent )
{
    QHBoxLayout* layout1 = new QHBoxLayout();

    QVBoxLayout* buttonlayout = new QVBoxLayout();


    QVector<QString>names = GLFunctions::getShaderNames();

    QButtonGroup* buttonGroup = new QButtonGroup(this);

    for ( int i = 0; i < names.size(); ++i )
    {
        QPushButton* button = new QPushButton( names[i] + "_vs", this );
        buttonlayout->addWidget( button );
        buttonGroup->addButton( button, 2 * i );


        QPushButton* button2 = new QPushButton( names[i] + "_fs", this );
        buttonlayout->addWidget( button2 );
        buttonGroup->addButton( button2, 2 * i + 1 );
    }

    connect( buttonGroup, SIGNAL( buttonClicked( QAbstractButton* ) ), this, SLOT( slotSelectCode( QAbstractButton* ) ) );

    buttonlayout->addStretch();

    QPushButton* sbutton = new QPushButton( "Save", this );
    buttonlayout->addWidget( sbutton );
    connect( sbutton, SIGNAL( clicked() ), this, SLOT( slotSavePressed() ) );

    m_edit = new QTextEdit( tr("edit" ) );
    m_edit->setMinimumHeight( 500 );
    m_edit->setMinimumWidth( 500 );

    m_currentName = names[0] + "_vs";
    m_edit->setText( GLFunctions::getShaderCode( m_currentName ) );

    layout1->addWidget( m_edit, 80 );
    layout1->addLayout( buttonlayout, 20 );

    this->setLayout( layout1 );
    this->setMinimumSize( 600, 600 );

}

ShaderEditWidget::~ShaderEditWidget()
{
    // TODO Auto-generated destructor stub
}

void ShaderEditWidget::slotSelectCode( QAbstractButton* button )
{
    m_currentName = button->text();
    m_edit->setText( GLFunctions::getShaderCode( button->text() ) );
}

void ShaderEditWidget::slotSavePressed()
{
    GLFunctions::setShaderCode( m_currentName, m_edit->toPlainText() );
}
