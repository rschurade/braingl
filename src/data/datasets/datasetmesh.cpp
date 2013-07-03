/*
 * datasetmesh.cpp
 *
 * Created on: Jul 19, 2012
 * @author Ralph Schurade
 */
#include "datasetmesh.h"

#include "../models.h"

#include "../mesh/trianglemesh2.h"
#include "../../gui/gl/meshrenderer.h"

DatasetMesh::DatasetMesh( TriangleMesh2* mesh, QDir fileName ) :
    Dataset( fileName, Fn::DatasetType::MESH_BINARY ),
    m_renderer( 0 )
{
    m_mesh.push_back( mesh );
    m_properties["maingl"]->set( Fn::Property::D_COLORMODE, { "per mesh", "mri", "per vertex", "vertex data" }, 0, "general" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP, 1, "general" );
    m_properties["maingl"]->set( Fn::Property::D_COLOR, QColor( 255, 255, 255 ), "general" );
    m_properties["maingl"]->set( Fn::Property::D_ALPHA, 1.f, 0.01f, 1.f, "general" );

    m_properties["maingl"]->set( Fn::Property::D_RENDER_COLORMAP, false, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP_X, 50, 1, 2000, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP_Y, 50, 1, 2000, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP_DX, 400, 1, 2000, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP_DY, 20, 1, 100, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP_TEXT_SIZE, 30, 1, 100, "colormap" );

    m_properties["maingl"]->set( Fn::Property::D_PAINTMODE, { "off", "paint" }, 0, "paint" );
    m_properties["maingl"]->set( Fn::Property::D_PAINTSIZE, 2.f, 1.f, 100.f, "paint" );
    m_properties["maingl"]->set( Fn::Property::D_PAINTCOLOR, QColor( 255, 0, 0 ), "paint" );

    float min = 0; //std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();
/*
    for ( int i = 0; i < mesh->numVerts(); ++i )
    {
        float value = mesh->getVertexData( i );
        min = qMin( min, value );
        max = qMax( max, value );
    }
*/
    m_properties["maingl"]->set( Fn::Property::D_MIN, min );
    m_properties["maingl"]->set( Fn::Property::D_MAX, max );
    m_properties["maingl"]->set( Fn::Property::D_SELECTED_MIN, min, min, max );
    m_properties["maingl"]->set( Fn::Property::D_SELECTED_MAX, max, min, max );
    m_properties["maingl"]->set( Fn::Property::D_LOWER_THRESHOLD, min + (max-min)/1000., min, max );
    m_properties["maingl"]->set( Fn::Property::D_UPPER_THRESHOLD, max, min, max );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_PAINTMODE ), SIGNAL( valueChanged( int ) ), this, SLOT( paintModeChanged( int ) ) );

    m_properties["maingl2"]->set( Fn::Property::D_COLORMODE, { "per mesh", "mri", "per vertex", "vertex data" }, 0, "general" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP, 1, "general" );
    m_properties["maingl2"]->set( Fn::Property::D_COLOR, QColor( 255, 255, 255 ), "general" );
    m_properties["maingl2"]->set( Fn::Property::D_ALPHA, 1.f, 0.f, 1.f, "general" );

    m_properties["maingl2"]->set( Fn::Property::D_RENDER_COLORMAP, false, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP_X, 50, 1, 2000, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP_Y, 50, 1, 2000, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP_DX, 400, 1, 2000, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP_DY, 20, 1, 100, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP_TEXT_SIZE, 30, 1, 100, "colormap" );

    m_properties["maingl2"]->set( Fn::Property::D_MIN, min );
    m_properties["maingl2"]->set( Fn::Property::D_MAX, max );
    m_properties["maingl2"]->set( Fn::Property::D_SELECTED_MIN, min, min, max );
    m_properties["maingl2"]->set( Fn::Property::D_SELECTED_MAX, max, min, max );
    m_properties["maingl2"]->set( Fn::Property::D_LOWER_THRESHOLD, min + (max-min)/1000., min, max );
    m_properties["maingl2"]->set( Fn::Property::D_UPPER_THRESHOLD, max, min, max );
}

DatasetMesh::DatasetMesh( QDir fileName, Fn::DatasetType type ) :
    Dataset( fileName, type ),
    m_renderer( 0 )
{
    PropertyGroup* props = new PropertyGroup();
    props->set( Fn::Property::D_ACTIVE, true, "general" );
    props->set( Fn::Property::D_RENDER_TARGET, "maingl2" );
    m_properties.insert( "maingl2", props );

    m_properties["maingl"]->set( Fn::Property::D_COLORMODE, { "per mesh", "mri", "per vertex", "vertex data" }, 0, "general" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP, 1, "general" );
    m_properties["maingl"]->set( Fn::Property::D_COLOR, QColor( 255, 255, 255 ), "general" );
    m_properties["maingl"]->set( Fn::Property::D_ALPHA, 1.f, 0.f, 1.f, "general" );

    m_properties["maingl"]->set( Fn::Property::D_RENDER_COLORMAP, false, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP_X, 50, 1, 2000, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP_Y, 50, 1, 2000, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP_DX, 400, 1, 2000, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP_DY, 20, 1, 100, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_COLORMAP_TEXT_SIZE, 30, 1, 100, "colormap" );

    m_properties["maingl"]->set( Fn::Property::D_PAINTMODE, { "off", "paint" }, 0, "paint" );
    m_properties["maingl"]->set( Fn::Property::D_PAINTSIZE, 2.f, 1.f, 100.f, "paint" );
    m_properties["maingl"]->set( Fn::Property::D_PAINTCOLOR, QColor( 255, 0, 0 ), "paint" );

    float min = 0; //std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();
/*
    for ( int i = 0; i < mesh->numVerts(); ++i )
    {
        float value = mesh->getVertexData( i );
        min = qMin( min, value );
        max = qMax( max, value );
    }
*/
//    m_properties["maingl"]->set( Fn::Property::D_MIN, min );
//    m_properties["maingl"]->set( Fn::Property::D_MAX, max );
//    m_properties["maingl"]->set( Fn::Property::D_SELECTED_MIN, min, min, max );
//    m_properties["maingl"]->set( Fn::Property::D_SELECTED_MAX, max, min, max );
//    m_properties["maingl"]->set( Fn::Property::D_LOWER_THRESHOLD, min + (max-min)/1000., min, max );
//    m_properties["maingl"]->set( Fn::Property::D_UPPER_THRESHOLD, max, min, max );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_PAINTMODE ), SIGNAL( valueChanged( int ) ), this, SLOT( paintModeChanged( int ) ) );

    m_properties["maingl2"]->set( Fn::Property::D_COLORMODE, { "per mesh", "mri", "per vertex", "vertex data" }, 0, "general" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP, 1, "general" );
    m_properties["maingl2"]->set( Fn::Property::D_COLOR, QColor( 255, 255, 255 ), "general" );
    m_properties["maingl2"]->set( Fn::Property::D_ALPHA, 1.f, 0.f, 1.f, "general" );

    m_properties["maingl2"]->set( Fn::Property::D_RENDER_COLORMAP, false, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP_X, 50, 1, 2000, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP_Y, 50, 1, 2000, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP_DX, 400, 1, 2000, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP_DY, 20, 1, 100, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_COLORMAP_TEXT_SIZE, 30, 1, 100, "colormap" );

    /*m_properties["maingl2"]->set( Fn::Property::D_MIN, min );
    m_properties["maingl2"]->set( Fn::Property::D_MAX, max );
    m_properties["maingl2"]->set( Fn::Property::D_SELECTED_MIN, min, min, max );
    m_properties["maingl2"]->set( Fn::Property::D_SELECTED_MAX, max, min, max );
    m_properties["maingl2"]->set( Fn::Property::D_LOWER_THRESHOLD, min + (max-min)/1000., min, max );
    m_properties["maingl2"]->set( Fn::Property::D_UPPER_THRESHOLD, max, min, max );*/
}
DatasetMesh::~DatasetMesh()
{
}

TriangleMesh2* DatasetMesh::getMesh()
{
    return m_mesh[0];
}

void DatasetMesh::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( m_mesh[0] );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
    }


    m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
}

QString DatasetMesh::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}

void DatasetMesh::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target )
{
    int paintMode = m_properties["maingl"]->get( Fn::Property::D_PAINTMODE ).toInt();
    if ( pickId == 0 ||  paintMode == 0 || !( modifiers & Qt::ControlModifier ) )
    {
        return;
    }

    QColor color;
    if ( ( modifiers & Qt::ControlModifier ) && !( modifiers & Qt::ShiftModifier ) )
    {
        color = m_properties["maingl"]->get( Fn::Property::D_PAINTCOLOR ).value<QColor>();
    }
    else if ( ( modifiers & Qt::ControlModifier ) && ( modifiers & Qt::ShiftModifier ) )
    {
        color = m_properties["maingl"]->get( Fn::Property::D_COLOR ).value<QColor>();
    }
    else
    {
        return;
    }

    QVector<int> picked = getMesh()->pick( pos, m_properties["maingl"]->get( Fn::Property::D_PAINTSIZE ).toFloat() );

    if ( picked.size() > 0 )
    {
        m_renderer->beginUpdateColor();
        for ( int i = 0; i < picked.size(); ++i )
        {
            m_renderer->updateColor( picked[i], color.redF(), color.greenF(), color.blueF(), 1.0 );
            m_mesh[0]->setVertexColor( picked[i], color );
        }
        m_renderer->endUpdateColor();
    }
}

void DatasetMesh::paintModeChanged( int mode )
{
    if ( mode > 0 )
    {
        m_properties["maingl"]->set( Fn::Property::D_COLORMODE, 2 );
    }
}
