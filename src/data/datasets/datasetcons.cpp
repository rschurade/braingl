/*
 * datasetcons.cpp
 *
 *  Created on: May 1, 2013
 *      Author: user
 */

#include "datasetcons.h"

DatasetCons::DatasetCons( QString fileName ) :
        Dataset( fileName, Fn::DatasetType::CONS ), vectorArray( NULL )
{
    cons = new Connections( fileName );
    consNumber = cons->edges.length();

    // add standard properties
    m_properties["maingl"]->set( Fn::Property::ACTIVE, true );
    m_properties["maingl"]->set( Fn::Property::SIZE, -1 );
    m_properties["maingl"]->set( Fn::Property::CREATED_BY, (int) Fn::Algo::NONE );

    m_properties["maingl"]->set( Fn::Property::HAS_TEXTURE, false );

    // add new properties
    //m_properties["maingl"]->set( Fn::Property::THRESHOLD, 0.5f, 0.0f, 1.0f );
    m_properties["maingl"]->set( Fn::Property::GLYPHRADIUS, 1.0f );
}

DatasetCons::DatasetCons( Connections* cons ) :
        Dataset( QString( "new" ), Fn::DatasetType::CONS ), vectorArray( NULL ), cons( cons )
{
    consNumber = cons->edges.length();

    // add standard properties
    m_properties["maingl"]->set( Fn::Property::ACTIVE, true );
    m_properties["maingl"]->set( Fn::Property::SIZE, -1 );
    m_properties["maingl"]->set( Fn::Property::CREATED_BY, (int) Fn::Algo::NONE );

    m_properties["maingl"]->set( Fn::Property::HAS_TEXTURE, false );

    // add new properties
    //m_properties["maingl"]->set( Fn::Property::THRESHOLD, 0.5f, 0.0f, 1.0f );
    m_properties["maingl"]->set( Fn::Property::GLYPHRADIUS, 1.0f );
}

DatasetCons::~DatasetCons()
{
}

void DatasetCons::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::ACTIVE ).toBool() )
    {
        return;
    }

    //TODO: setup vector renderer...if ( m_vrenderer == 0 )
    {
        m_vrenderer = new VectorGlyphRenderer();
        m_vrenderer->init();
        m_vrenderer->initGeometry( getVectors(), consNumber );
    }
    qDebug() << "con draw";
    m_vrenderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
}

QString DatasetCons::getValueAsString()
{
    return QString( "Connections" );
}

Connections* DatasetCons::getCons()
{
    return cons;
}

float* DatasetCons::getVectors()
{
    if ( vectorArray )
        delete vectorArray;
    qDebug() << "edges: " << consNumber;
    int offset = 28;
    vectorArray = new float[offset * consNumber];

    for ( int i = 0; i < consNumber; i++ )
    {
        float v = 1;
        Edge* e = cons->edges.at( i );
        QVector3D f = e->fn;
        QVector3D t = e->tn;

        QVector3D d = t - f;

        vectorArray[offset * i] = f.x();
        vectorArray[offset * i + 1] = f.y();
        vectorArray[offset * i + 2] = f.z();
        vectorArray[offset * i + 3] = t.x();
        vectorArray[offset * i + 4] = t.y();
        vectorArray[offset * i + 5] = t.z();
        vectorArray[offset * i + 6] = v;
        vectorArray[offset * i + 7] = 1;
        vectorArray[offset * i + 8] = d.x();
        vectorArray[offset * i + 9] = d.y();
        vectorArray[offset * i + 10] = d.z();
        vectorArray[offset * i + 11] = d.x();
        vectorArray[offset * i + 12] = d.y();
        vectorArray[offset * i + 13] = d.z();

        vectorArray[offset * i + 14] = t.x();
        vectorArray[offset * i + 15] = t.y();
        vectorArray[offset * i + 16] = t.z();
        vectorArray[offset * i + 17] = f.x();
        vectorArray[offset * i + 18] = f.y();
        vectorArray[offset * i + 19] = f.z();
        vectorArray[offset * i + 20] = v;
        vectorArray[offset * i + 21] = -1;
        vectorArray[offset * i + 22] = -d.x();
        vectorArray[offset * i + 23] = -d.y();
        vectorArray[offset * i + 24] = -d.z();
        vectorArray[offset * i + 25] = -d.x();
        vectorArray[offset * i + 26] = -d.y();
        vectorArray[offset * i + 27] = -d.z();

    }
    return vectorArray;
}
