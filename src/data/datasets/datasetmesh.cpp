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
    Dataset( fileName, Fn::DatasetType::MESH_BINARY ), m_renderer( 0 )
{
    m_mesh.push_back( mesh );
    initProperties();
    finalizeProperties();
}

DatasetMesh::DatasetMesh( QDir fileName, Fn::DatasetType type ) :
    Dataset( fileName, type ), m_renderer( 0 )
{
    initProperties();
}

DatasetMesh::~DatasetMesh()
{
}

void DatasetMesh::initProperties()
{
    m_properties["maingl"]->create( Fn::Property::D_COLORMODE,
    { "per mesh", "mri", "per vertex", "vertex data" }, 0, "general" );
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP, 1, "general" );
    m_properties["maingl"]->create( Fn::Property::D_COLOR, QColor( 255, 255, 255 ), "general" );
    m_properties["maingl"]->create( Fn::Property::D_ALPHA, 1.f, 0.01f, 1.f, "general" );

    m_properties["maingl"]->create( Fn::Property::D_RENDER_COLORMAP, false, "colormap" );
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP_X, 50, 1, 2000, "colormap" );
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP_Y, 50, 1, 2000, "colormap" );
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP_DX, 400, 1, 2000, "colormap" );
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP_DY, 20, 1, 100, "colormap" );
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP_TEXT_SIZE, 30, 1, 100, "colormap" );

    m_properties["maingl"]->create( Fn::Property::D_PAINTMODE,
    { "off", "paint", "paint values" }, 0, "paint" );
    m_properties["maingl"]->create( Fn::Property::D_PAINTSIZE, 20.f, 1.f, 1000.f, "paint" );
    m_properties["maingl"]->create( Fn::Property::D_PAINTCOLOR, QColor( 255, 0, 0 ), "paint" );
    m_properties["maingl"]->create( Fn::Property::D_PAINTVALUE, 0.5f, -1.0f, 1.0f, "paint" );

    float min = 0.0;
    float max = 1.0;

    m_properties["maingl"]->create( Fn::Property::D_MIN, min );
    m_properties["maingl"]->create( Fn::Property::D_MAX, max );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MIN, min, min, max, "colormap"  );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MAX, max, min, max, "colormap"  );
    m_properties["maingl"]->create( Fn::Property::D_LOWER_THRESHOLD, min + ( max - min ) / 1000., min, max, "colormap"  );
    m_properties["maingl"]->create( Fn::Property::D_UPPER_THRESHOLD, max, min, max, "colormap"  );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
            m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
            m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_PAINTMODE ), SIGNAL( valueChanged( QVariant ) ), this,
            SLOT( paintModeChanged( QVariant ) ) );

    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_LOWER_X, false, "special" );
    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_LOWER_Y, false, "special" );
    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_LOWER_Z, false, "special" );
    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_HIGHER_X, false, "special" );
    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_HIGHER_Y, false, "special" );
    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_HIGHER_Z, false, "special" );

    m_properties["maingl"]->create( Fn::Property::D_ADJUST_X, 0.0f, -500.0f, 500.0f, "special" );
    m_properties["maingl"]->create( Fn::Property::D_ADJUST_Y, 0.0f, -500.0f, 500.0f, "special" );
    m_properties["maingl"]->create( Fn::Property::D_ADJUST_Z, 0.0f, -500.0f, 500.0f, "special" );

    m_properties["maingl"]->create( Fn::Property::D_RENDER_WIREFRAME, false, "special" );

    m_properties["maingl"]->create( Fn::Property::D_LIGHT_SWITCH, true, "light" );
    m_properties["maingl"]->create( Fn::Property::D_LIGHT_AMBIENT,   0.3f, 0.0f, 1.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_LIGHT_DIFFUSE,   0.6f, 0.0f, 1.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_LIGHT_SPECULAR,  0.5f, 0.0f, 1.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_MATERIAL_AMBIENT,   0.5f, 0.0f, 10.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_MATERIAL_DIFFUSE,   0.8f, 0.0f, 10.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_MATERIAL_SPECULAR,  0.61f, 0.0f, 10.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_MATERIAL_SHININESS, 1.0f, 0.0f, 200.0f, "light" );

}

void DatasetMesh::finalizeProperties()
{
    PropertyGroup* props2 = new PropertyGroup( *( m_properties["maingl"] ) );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"]->getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );

    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
            m_properties["maingl2"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
            m_properties["maingl2"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );
    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_PAINTMODE ), SIGNAL( valueChanged( QVariant ) ), this,
            SLOT( paintModeChanged( QVariant ) ) );
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

bool DatasetMesh::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target )
{
    int paintMode = m_properties[target]->get( Fn::Property::D_PAINTMODE ).toInt();
    if ( pickId == 0 || paintMode == 0 || !( modifiers & Qt::ControlModifier ) )
    {
        return false;
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
        return false;
    }

    QVector<int> picked = getMesh()->pick( pos, m_properties["maingl"]->get( Fn::Property::D_PAINTSIZE ).toFloat() );

    if ( picked.size() > 0 )
    {
        m_renderer->beginUpdateColor();
        for ( int i = 0; i < picked.size(); ++i )
        {
            if ( paintMode == 1 ) //paint color
            {
                m_renderer->updateColor( picked[i], color.redF(), color.greenF(), color.blueF(), 1.0 );
                m_mesh[0]->setVertexColor( picked[i], color );
            }
            else if ( paintMode == 2 ) //TODO: paint values does not work
            {
                qDebug() << "setting: " << picked[i];
                m_mesh[0]->setVertexData( picked[i], 1.0f ); //m_mesh[0]->getVertexData( picked[i] ) + 0.1 );
            }
        }
        m_renderer->endUpdateColor();
        return true;
    }

    return false;
}

void DatasetMesh::paintModeChanged( QVariant mode )
{
    if ( mode.toInt() == 1 )
    {
        m_properties["maingl"]->set( Fn::Property::D_COLORMODE, 2 );
    }
    if ( mode.toInt() == 2 )
    {
        m_properties["maingl"]->set( Fn::Property::D_COLORMODE, 3 );
    }
}
