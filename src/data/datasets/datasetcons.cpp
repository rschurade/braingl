/*
 * datasetcons.cpp
 *
 *  Created on: May 1, 2013
 *      Author: user
 */

#include "datasetcons.h"

#include "../properties/propertyfloat.h"

DatasetCons::DatasetCons( QString fileName ) :
    DatasetFibers( fileName, Fn::DatasetType::CONS )
{
    cons = new Connections( fileName );
    init();
}

DatasetCons::DatasetCons( Connections* cons ) :
    DatasetFibers( QString( "new" ), Fn::DatasetType::CONS ),
    cons( cons )
{
    init();
}

DatasetCons::~DatasetCons()
{
}

void DatasetCons::init()
{
    consNumber = cons->edges.length();
    for ( int i = 0; i < consNumber; i++ )
    {
        Edge* e = cons->edges.at( i );
        QVector3D f = e->fn;
        QVector3D t = e->tn;
        Fib fib;
        fib.addVert( f.x(), f.y(), f.z() );
        fib.addVert( t.x(), t.y(), t.z() );
        fib.setData( 0, 0, e->m_value );
        fib.setData( 0, 1, e->m_value );
        m_fibs.push_back( fib );
    }

    m_dataNames.push_back( "value" );

    createProps();

}

QString DatasetCons::getValueAsString()
{
    return QString( "Connections" );
}

Connections* DatasetCons::getCons()
{
    return cons;
}

QString DatasetCons::getSaveFilter()
{
    return QString( "Connexels ascii (*.cxls);;fib files binary(*.fib *.vtk );;fib files ascii (*.fib *.vtk);;all files (*.*)" );
}

QString DatasetCons::getDefaultSuffix()
{
    return QString( "cxls" );
}

