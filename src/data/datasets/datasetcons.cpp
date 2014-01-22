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
        DatasetFibers( QString( "new" ), Fn::DatasetType::CONS ), cons( cons )
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
        QVector<float> fib;
        fib.push_back( f.x() );
        fib.push_back( f.y() );
        fib.push_back( f.z() );
        fib.push_back( t.x() );
        fib.push_back( t.y() );
        fib.push_back( t.z() );
        m_fibs.push_back( fib );
    }
    QVector<QVector<float> > data0;
    QVector<float> min0;
    QVector<float> max0;
    data0.reserve( m_fibs.size() );
    for ( int i = 0; i < m_fibs.size(); ++i )
    {
        data0.push_back( QVector<float>( m_fibs[i].size() / 3 ) );
    }
    m_data.push_back( data0 );
    m_dataNames.push_back( "no data" );

    m_customColors.resize( consNumber );

    /* // add standard properties
     m_properties["maingl"]->create( Fn::Property::D_ACTIVE, true );
    m_properties["maingl"]->create( Fn::Property::D_SIZE, -1 );
    m_properties["maingl"]->create( Fn::Property::D_CREATED_BY, (int) Fn::Algo::NONE );

    m_properties["maingl"]->create( Fn::Property::D_HAS_TEXTURE, false );

// add new properties
    m_properties["maingl"]->create( Fn::Property::D_GLYPHRADIUS, 1.0f );

    m_properties["maingl"]->create( Fn::Property::D_DRAW_GLYPHS, true );

    m_properties["maingl"]->create( Fn::Property::D_THRESHOLD, 0.0f, 0.0f, 1.0f, "general" );

    float min = -1;
    float max = 1;
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP, 1, "general" );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MIN, min, min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MAX, max, min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_LOWER_THRESHOLD, min + ( max - min ) / 1000., min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_UPPER_THRESHOLD, max, min, max, "general" );

    m_properties["maingl"]->create( Fn::Property::D_COLOR, QColor( 255, 255, 255 ), "general" );
    m_properties["maingl"]->create( Fn::Property::D_GLYPH_COLORMODE,
    { "orientation", "value" }, 0, "general" );

    PropertyGroup* props2 = new PropertyGroup( *( m_properties["maingl"] ) );
    m_properties.insert( "maingl2", props2 );*/

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

