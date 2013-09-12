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

#include <QFile>
#include <QFileDialog>

DatasetMesh::DatasetMesh( TriangleMesh2* mesh, QDir fileName ) :
    Dataset( fileName, Fn::DatasetType::MESH_BINARY ),
    m_renderer( 0 )
{
    m_mesh.push_back( mesh );
    initProperties();
    finalizeProperties();
}

DatasetMesh::DatasetMesh( QDir fileName, Fn::DatasetType type ) :
    Dataset( fileName, type ),
    m_renderer( 0 )
{
    initProperties();
}

DatasetMesh::~DatasetMesh()
{
}

void DatasetMesh::initProperties()
{
    float min = 0.0;
    float max = 1.0;

    m_properties["maingl"]->create( Fn::Property::D_COLORMODE,
    { "per mesh", "mri", "per vertex", "vertex data" }, 0, "general" );
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP, 1, "general" );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MIN, min, min, max, "general"  );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MAX, max, min, max, "general"  );
    m_properties["maingl"]->create( Fn::Property::D_LOWER_THRESHOLD, min + ( max - min ) / 1000., min, max, "general"  );
    m_properties["maingl"]->create( Fn::Property::D_UPPER_THRESHOLD, max, min, max, "general"  );

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


    m_properties["maingl"]->create( Fn::Property::D_MIN, min );
    m_properties["maingl"]->create( Fn::Property::D_MAX, max );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
            m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
            m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_PAINTMODE ), SIGNAL( valueChanged( QVariant ) ), this,
            SLOT( paintModeChanged( QVariant ) ) );

    m_properties["maingl"]->create( Fn::Property::D_RENDER_WIREFRAME, false, "special" );

    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_LOWER_X, false, "special" );
    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_LOWER_Y, false, "special" );
    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_LOWER_Z, false, "special" );
    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_HIGHER_X, false, "special" );
    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_HIGHER_Y, false, "special" );
    m_properties["maingl"]->create( Fn::Property::D_MESH_CUT_HIGHER_Z, false, "special" );

    m_properties["maingl"]->create( Fn::Property::D_ADJUST_X, 0.0f, -500.0f, 500.0f, "special" );
    m_properties["maingl"]->create( Fn::Property::D_ADJUST_Y, 0.0f, -500.0f, 500.0f, "special" );
    m_properties["maingl"]->create( Fn::Property::D_ADJUST_Z, 0.0f, -500.0f, 500.0f, "special" );

    m_properties["maingl"]->create( Fn::Property::D_ROTATE_X, 0.0f, 0.0f, 360.0f, "special" );
    m_properties["maingl"]->create( Fn::Property::D_ROTATE_Y, 0.0f, 0.0f, 360.0f, "special" );
    m_properties["maingl"]->create( Fn::Property::D_ROTATE_Z, 0.0f, 0.0f, 360.0f, "special" );

    m_properties["maingl"]->create( Fn::Property::D_SCALE_X, 1.0f, 0.0f, 10.0f, "special" );
    m_properties["maingl"]->create( Fn::Property::D_SCALE_Y, 1.0f, 0.0f, 10.0f, "special" );
    m_properties["maingl"]->create( Fn::Property::D_SCALE_Z, 1.0f, 0.0f, 10.0f, "special" );

    m_properties["maingl"]->createButton( Fn::Property::D_MESH_MAKE_PERMANENT, "special" );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_MESH_MAKE_PERMANENT ), SIGNAL( valueChanged( QVariant ) ), this,
            SLOT( makePermanent() ) );

    m_properties["maingl"]->create( Fn::Property::D_LIGHT_SWITCH, true, "light" );
    m_properties["maingl"]->create( Fn::Property::D_LIGHT_AMBIENT,   0.3f, 0.0f, 1.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_LIGHT_DIFFUSE,   0.6f, 0.0f, 1.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_LIGHT_SPECULAR,  0.5f, 0.0f, 1.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_MATERIAL_AMBIENT,   0.5f, 0.0f, 10.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_MATERIAL_DIFFUSE,   0.8f, 0.0f, 10.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_MATERIAL_SPECULAR,  0.61f, 0.0f, 10.0f, "light" );
    m_properties["maingl"]->create( Fn::Property::D_MATERIAL_SHININESS, 1.0f, 0.0f, 200.0f, "light" );

    m_properties["maingl"]->create( Fn::Property::D_MESH_NUM_VERTEX, 0 );
    m_properties["maingl"]->create( Fn::Property::D_MESH_NUM_TRIANGLES, 0 );

    m_properties["maingl"]->create( Fn::Property::D_GLYPHSET_PICKED_ID, 0 );

    if( m_mesh.size() > 0 )
    {
        m_properties["maingl"]->set( Fn::Property::D_MESH_NUM_VERTEX, m_mesh[0]->numVerts() );
        m_properties["maingl"]->set( Fn::Property::D_MESH_NUM_TRIANGLES, m_mesh[0]->numTris() );
    }

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

void DatasetMesh::setProperties()
{
    m_properties["maingl"]->create( Fn::Property::D_SURFACE, m_displayList, 0, "general" );
    m_properties["maingl2"]->create( Fn::Property::D_SURFACE, m_displayList, 0, "general" );
}


/*****************************************************************************************************************************************************
 *
 * adds TriangleMesh to the list of meshes only if those meshes have the exact same number of vertexes
 *
 ****************************************************************************************************************************************************/
void DatasetMesh::addMesh( TriangleMesh2* tm, QString displayString )
{
    if ( m_mesh.size() > 0 )
    {
        int numVerts = m_mesh[0]->numVerts();
        if ( numVerts != tm->numVerts() )
        {
            return;
        }
    }
    m_mesh.push_back( tm );
    m_displayList.push_back( displayString );
}

TriangleMesh2* DatasetMesh::getMesh()
{
    if ( m_mesh.size() > 0 )
    {
        return m_mesh[0];
    }
    return 0;
}

TriangleMesh2* DatasetMesh::getMesh( int id )
{
    if ( m_mesh.size() > id )
    {
        return m_mesh[id];
    }
    return 0;
}

TriangleMesh2* DatasetMesh::getMesh( QString target )
{
    int n = properties( target )->get( Fn::Property::D_SURFACE ).toInt();
    return m_mesh[n];
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

    QVector<int> picked = getMesh( target )->pick( pos, m_properties["maingl"]->get( Fn::Property::D_PAINTSIZE ).toFloat() );

    if ( picked.size() > 0 )
    {
        m_renderer->beginUpdateColor();
        for ( int i = 0; i < picked.size(); ++i )
        {
            m_renderer->updateColor( picked[i], color.redF(), color.greenF(), color.blueF(), 1.0 );
            for ( int m = 0; m < m_mesh.size(); m++ )
            {
                if ( paintMode == 1 ) //paint color
                {
                    m_mesh[m]->setVertexColor( picked[i], color );
                }
                else if ( paintMode == 2 ) //paint values
                {
                    float value = m_mesh[0]->getVertexData( picked[i] ) + m_properties[target]->get( Fn::Property::D_PAINTVALUE ).toFloat();
                    if ( value < 0.0 )
                    {
                        value = 0.0;
                    }
                    if ( value > 1.0 )
                    {
                        value = 1.0;
                    }

                    m_mesh[m]->setVertexData( picked[i], value );
                }
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

void DatasetMesh::makePermanent()
{
    QMatrix4x4 mMatrix;
    mMatrix.setToIdentity();

    if( m_properties["maingl"]->contains( Fn::Property::D_ROTATE_X ) )
    {
        mMatrix.rotate( -m_properties["maingl"]->get( Fn::Property::D_ROTATE_X ).toFloat(), 1.0, 0.0, 0.0 );
        mMatrix.rotate( -m_properties["maingl"]->get( Fn::Property::D_ROTATE_Y ).toFloat(), 0.0, 1.0, 0.0 );
        mMatrix.rotate( -m_properties["maingl"]->get( Fn::Property::D_ROTATE_Z ).toFloat(), 0.0, 0.0, 1.0 );
        mMatrix.scale( m_properties["maingl"]->get( Fn::Property::D_SCALE_X ).toFloat(),
                m_properties["maingl"]->get( Fn::Property::D_SCALE_Y ).toFloat(),
                m_properties["maingl"]->get( Fn::Property::D_SCALE_Z ).toFloat() );
    }

    int adjustX = m_properties["maingl"]->get( Fn::Property::D_ADJUST_X ).toFloat();
    int adjustY = m_properties["maingl"]->get( Fn::Property::D_ADJUST_Y ).toFloat();
    int adjustZ = m_properties["maingl"]->get( Fn::Property::D_ADJUST_Z ).toFloat();

    int numVerts = m_mesh[0]->numVerts();

    for( int i = 0; i < numVerts; ++i )
    {
        QVector3D vert = m_mesh[0]->getVertex( i );

        vert = vert * mMatrix;
        vert.setX( vert.x() + adjustX );
        vert.setY( vert.y() + adjustY );
        vert.setZ( vert.z() + adjustZ );

        m_mesh[0]->setVertex( i, vert );
    }

    m_mesh[0]->finalize();
    delete m_renderer;
    m_renderer = 0;

    m_properties["maingl"]->set( Fn::Property::D_ROTATE_X, 0 );
    m_properties["maingl"]->set( Fn::Property::D_ROTATE_Y, 0 );
    m_properties["maingl"]->set( Fn::Property::D_ROTATE_Z, 0 );

    m_properties["maingl"]->set( Fn::Property::D_ADJUST_X, 0 );
    m_properties["maingl"]->set( Fn::Property::D_ADJUST_Y, 0 );
    m_properties["maingl"]->set( Fn::Property::D_ADJUST_Z, 0 );

    m_properties["maingl"]->set( Fn::Property::D_SCALE_X, 1 );
    m_properties["maingl"]->set( Fn::Property::D_SCALE_Y, 1 );
    m_properties["maingl"]->set( Fn::Property::D_SCALE_Z, 1 );
}

void DatasetMesh::save1Ds()
{
    QString filename = QFileDialog::getSaveFileName( NULL, "save 1D file", ".1D" );

    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        qDebug() << "file open failed: " << filename;
        return;
    }
    QTextStream out( &file );
    for ( int i = 0; i < getMesh( "maingl" )->numVerts(); i++ )
    {
        float vcolor = getMesh( "maingl" )->getVertexData( i );

        out << vcolor << endl;
    }
    file.close();
}

bool DatasetMesh::load1D()
{
    QString filename = QFileDialog::getOpenFileName( NULL, "load 1D file" );
    QFile file( filename );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        return false;
    }
    QTextStream in( &file );

    for ( int i = 0; i < getMesh( "maingl" )->numVerts(); i++ )
    {
        float v;
        in >> v;
        for ( int m = 0; m < m_mesh.size(); m++ )
        {
            m_mesh[m]->setVertexData( i, v );
        }
    }
    return true;
}
