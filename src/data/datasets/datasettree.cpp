/*
 * datasettree.cpp
 *
 *  Created on: 17.09.2013
 *      Author: Ralph
 */

#include "datasettree.h"

#include "../models.h"

#include "../../algos/colormapbase.h"

#include "../../gui/gl/colormapfunctions.h"
#include "../../gui/gl/glfunctions.h"
#include "../../gui/gl/treerenderer.h"
#include "../../gui/gl/meshrenderer.h"

#include "../mesh/trianglemesh2.h"


#include <QDebug>
#include <QQueue>
#include <QWidget>

DatasetTree::DatasetTree( QDir fn ) :
    DatasetMesh( fn , Fn::DatasetType::TREE ),
    m_tree( 0 ),
    m_root( 0 ),
    m_treeRenderer( 0 ),
    m_treeRenderer2( 0 ),
    m_numLeaves( 0 ),
    m_numNodes( 0 ),
    m_zoom( 1 ),
    m_zoom2( 1 )
{
    m_properties["maingl"].set( Fn::Property::D_COLORMAP, -1 );
    m_properties["maingl"].getWidget( Fn::Property::D_COLORMAP )->hide();
    m_properties["maingl"].set( Fn::Property::D_COLORMODE, 2 );
    m_properties["maingl"].getWidget( Fn::Property::D_COLORMODE )->hide();
    m_properties["maingl"].getWidget( Fn::Property::D_SELECTED_MIN )->hide();
    m_properties["maingl"].getWidget( Fn::Property::D_SELECTED_MAX )->hide();
    m_properties["maingl"].getWidget( Fn::Property::D_LOWER_THRESHOLD )->hide();
    m_properties["maingl"].getWidget( Fn::Property::D_UPPER_THRESHOLD )->hide();
    m_properties["maingl"].getWidget( Fn::Property::D_COLOR )->hide();
    m_properties["maingl"].getWidget( Fn::Property::D_COPY_COLORS )->hide();

    m_properties["maingl"].createBool( Fn::Property::D_INTERPOLATION, false );
    m_properties["maingl"].createBool( Fn::Property::D_RENDER_MESH, true, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_ALPHA, 1.0f, 0.0, 1.0 );
    m_properties["maingl"].createInt( Fn::Property::D_DIM, 3 );

    m_properties["maingl"].createInt( Fn::Property::D_TREE_SELECTED_CLUSTER, 0, 0, 0, "tree" );
    m_properties["maingl"].createRadioGroup( Fn::Property::D_TREE_COLOR_SELECTION, { "base", "selected cluster", "user defined", "partion" }, 0, "tree" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_TREE_COLOR_SELECTION ), SIGNAL( valueChanged( QVariant ) ), this,
                    SLOT( colorSelectionChanged( QVariant ) ) );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_TREE_SELECTED_CLUSTER ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( selectCluster( QVariant ) ) );
    m_properties["maingl"].createColor( Fn::Property::D_TREE_SELECTED_CLUSTER_COLOR, QColor( 255, 0, 0 ), "tree settings" );
    m_properties["maingl"].createColor( Fn::Property::D_TREE_UNSELECTED_CLUSTER_COLOR, QColor( 128, 128, 128 ), "tree settings" );
    m_properties["maingl"].createColor( Fn::Property::D_TREE_USER_CLUSTER_COLOR, QColor( 0, 0, 255 ), "tree" );
    m_properties["maingl"].createButton( Fn::Property::D_TREE_SET_USER_CLUSTER_COLOR, "tree" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_TREE_SET_USER_CLUSTER_COLOR ), SIGNAL( valueChanged( QVariant ) ), this,
                    SLOT( setClusterColor() ) );

    m_properties["maingl"].createRadioGroup( Fn::Property::D_TREE_PARTITION_MODE, { "level", "x clusters" }, 0, "tree" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_TREE_PARTITION_MODE ), SIGNAL( valueChanged( QVariant ) ), this,
                        SLOT( partitionModeChanged( QVariant ) ) );

    m_properties["maingl"].createFloat( Fn::Property::D_TREE_PARTITION_LEVEL, 1.0f, 0.0f, 1.0f, "tree" );
    m_properties["maingl"].createInt( Fn::Property::D_TREE_PARTITION_SIZE, 10, 1, 1000, "tree" );
    m_properties["maingl"].getWidget( Fn::Property::D_TREE_PARTITION_SIZE )->setHidden( true );

    m_properties["maingl"].createButton( Fn::Property::D_TREE_APPLY_PARTITION_MODE, "tree" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_TREE_APPLY_PARTITION_MODE ), SIGNAL( valueChanged( QVariant ) ), this,
                            SLOT( applyPartitionMode() ) );

    m_properties["maingl"].unsetTab( "colormap" );
    m_properties["maingl"].unsetTab( "paint" );


    PropertyGroup props2( m_properties["maingl"] );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"].getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );

}

DatasetTree::~DatasetTree()
{
}

void DatasetTree::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target ).get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }

    if ( m_resetRenderer )
    {
        delete m_renderer;
        m_renderer = 0;
        m_resetRenderer = false;
    }

    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( getMesh(target) );
        m_renderer->init();
    }
    m_renderer->setMesh( getMesh(target) );
    if ( m_properties[target].get( Fn::Property::D_RENDER_MESH).toBool() )
    {
        m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }

    GLFunctions::drawColormapBar( properties( target ), width, height, renderMode );
}

void DatasetTree::drawTree( QMatrix4x4 mvpMatrix, int width, int height )
{
    m_width = width;
    m_height = height;

    if ( m_treeRenderer == 0 )
    {
        m_treeRenderer = new TreeRenderer( "tree", m_tree );
        m_treeRenderer->init();
    }
    m_treeRenderer->draw( mvpMatrix );
}

void DatasetTree::drawRoot( QMatrix4x4 mvpMatrix, int width, int height )
{
    m_width2 = width;
    m_height2 = height;

    if ( m_treeRenderer2 == 0 )
    {
        m_treeRenderer2 = new TreeRenderer( "tree2", m_root );
        m_treeRenderer2->init();
    }
    m_treeRenderer2->setColorIndex( 1 );
    m_treeRenderer2->draw( mvpMatrix );
}


void DatasetTree::importTree( QString dims, std::vector<QString>coords, std::vector<QString>clusters )
{
    QStringList dl = dims.split( " " );

    int nx = dl[0].toInt();
    int ny = dl[1].toInt();
    int nz = dl[2].toInt();
    float dx = dl[3].toFloat();
    float dy = dl[4].toFloat();
    float dz = dl[5].toFloat();
    float ax = dl[6].toFloat();
    float ay = dl[7].toFloat();
    float az = dl[8].toFloat();


    m_properties["maingl"].createInt( Fn::Property::D_NX, nx );
    m_properties["maingl"].createInt( Fn::Property::D_NY, ny );
    m_properties["maingl"].createInt( Fn::Property::D_NZ, nz );
    m_properties["maingl"].createFloat( Fn::Property::D_DX, dx );
    m_properties["maingl"].createFloat( Fn::Property::D_DY, dy );
    m_properties["maingl"].createFloat( Fn::Property::D_DZ, dz );
    m_properties["maingl"].createFloat( Fn::Property::D_ADJUST_X, ax );
    m_properties["maingl"].createFloat( Fn::Property::D_ADJUST_Y, ay );
    m_properties["maingl"].createFloat( Fn::Property::D_ADJUST_Z, az );

    nx -= 1;
    ny -= 1;
    nz -= 1;

    m_numLeaves = coords.size();
    m_numNodes = clusters.size();

    m_properties["maingl"].setMax( Fn::Property::D_TREE_SELECTED_CLUSTER, m_numLeaves + m_numNodes );

    qDebug() << "num leaves:" << m_numLeaves << "num nodes:" << m_numNodes;

    unsigned int currentId = coords.size();

    ColormapBase cmap = ColormapFunctions::getColormap( 2 );

    for ( unsigned int i = 0; i < coords.size(); ++i )
    {
        Tree* leaf = new Tree( i, 0.0 );
        QStringList cList = coords[i].split( " " );
        QVector3D pos( cList[0].toInt(), cList[1].toInt(), cList[2].toInt() );
        leaf->setTexturePosition( pos );
        m_nodes.push_back( leaf );
    }

    for( unsigned int i = 0; i < clusters.size(); ++i )
    {
        QStringList cl = clusters[i].split( " " );
        float value = cl[0].toFloat();

        m_tree = new Tree( currentId++, value );

        m_nodes.push_back( m_tree );

        for ( int k = 1; k < cl.size(); ++k )
        {
            unsigned int id = cl[k].toUInt();

            if ( id > m_nodes.size() - 1 )
            {
                qCritical() << "node id error";
            }
            m_tree->addChild( m_nodes[id] );
            m_nodes[id]->setParent( m_tree );
        }
    }

    for( int i = 0; i < m_numLeaves; ++i )
    {
        float v = ( (float)i/(float)m_numLeaves );
        QColor c = cmap.getColor( qMax( 0.0f, qMin( 1.0f, v ) ) );
        m_nodes[i]->setColor( 0, c, true, false );
    }

//    for ( int i = 0; i < m_projection.size(); ++i )
//    {
//        float v = ( (float)i/(float)m_projection.size() );
//        QColor c = cmap.getColor( qMax( 0.0f, qMin( 1.0f, v ) ) );
//        if ( m_projection[i] != -1 )
//        {
//            m_nodes[m_projection[i]]->setColor( 0, c, true, false );
//        }
//    }

    m_root = m_tree;

    QColor unselectedColor = m_properties["maingl"].get( Fn::Property::D_TREE_UNSELECTED_CLUSTER_COLOR ).value<QColor>();
    m_tree->setColor( 2, unselectedColor , false, true );
    m_tree->setColor( 3, unselectedColor , false, true );

    qDebug() << "test num leaves:" << m_tree->getNumLeaves();

    createTexture();
    m_properties["maingl"].set( Fn::Property::D_HAS_TEXTURE, true );
    m_properties["maingl"].getProperty( Fn::Property::D_HAS_TEXTURE )->setPropertyTab( "tree settings" );
    m_properties["maingl"].set( Fn::Property::D_TREE_SELECTED_CLUSTER, m_root->getId() );
}

void DatasetTree::createTexture()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_textureGLuint );

    glBindTexture( GL_TEXTURE_3D, m_textureGLuint );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );

    int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();

    int blockSize = nx * ny * nz;
    m_textureData.resize( blockSize * 3 );
    for ( int i = 0; i < blockSize * 3; ++i )
    {
        m_textureData[i] = 0.0;
    }

    createTextureRec( m_tree );
    GLFunctions::f->glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGB, GL_FLOAT, m_textureData.data() );
}

void DatasetTree::createTextureRec( Tree* tree )
{
    QList<Tree*>children = tree->getChildren();
    if ( children.size() > 0 )
    {
        for( int i = 0; i < children.size(); ++i )
        {
            createTextureRec( children[i] );
        }
    }
    else
    {
        int nx = m_properties["maingl"].get( Fn::Property::D_NX ).toInt();
        int ny = m_properties["maingl"].get( Fn::Property::D_NY ).toInt();
        //int nz = m_properties["maingl"].get( Fn::Property::D_NZ ).toInt();

        QVector3D pos = tree->getTexturePosition();
        QColor color = tree->getColor( m_properties["maingl"].get( Fn::Property::D_TREE_COLOR_SELECTION ).toInt() );

        int id = ( pos.x() + pos.y() * nx + pos.z() * nx * ny ) * 3;

        m_textureData[id] = color.redF();
        m_textureData[id+1] = color.greenF();
        m_textureData[id+2] = color.blueF();
    }
}

void DatasetTree::selectCluster( QVariant id )
{
    QColor unselectedColor = m_properties["maingl"].get( Fn::Property::D_TREE_UNSELECTED_CLUSTER_COLOR ).value<QColor>();
    QColor selectedColor = m_properties["maingl"].get( Fn::Property::D_TREE_SELECTED_CLUSTER_COLOR ).value<QColor>();
    m_tree->setColor( 1, unselectedColor , false, true );
    m_tree->setColor( id.toInt(), 1, selectedColor );

    m_treeRenderer->update();
    updateMeshColor();
}

bool DatasetTree::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target )
{
    if( target == "tree" )
    {
        float moveX = pos.z();
        int leaves = m_tree->getNumLeaves();

        float y = 1.0 - ( pos.y() / static_cast<float>( m_height ) );

        float zoom = qMin( leaves, m_width * ( m_zoom - 1 ) ) / 2;
        float range = ( leaves - moveX - zoom ) - ( 0 - moveX + zoom );
        float posXNormalized = ( pos.x() / static_cast<float>( m_width ) );
        float x = ( 0 - moveX + zoom ) + range * posXNormalized;

        qDebug() << x << moveX << zoom;
        int id = pickClusterRec( m_tree, 0, m_width * m_numLeaves, x, y );
        qDebug() << id;
        if ( id != -1 )
        {
            if ( m_nodes[id]->getParent() )
            {
                m_tree = m_nodes[id]->getParent();
            }
            m_properties["maingl"].set( Fn::Property::D_TREE_SELECTED_CLUSTER, id );
            QColor unselectedColor = m_properties["maingl"].get( Fn::Property::D_TREE_UNSELECTED_CLUSTER_COLOR ).value<QColor>();
            QColor selectedColor = m_properties["maingl"].get( Fn::Property::D_TREE_SELECTED_CLUSTER_COLOR ).value<QColor>();
            m_root->setColor( 1, unselectedColor , false, true );
            m_tree->setColor( id, 1, selectedColor );
            m_treeRenderer->setTree( m_tree );
            m_treeRenderer->setSelected( id );
            m_treeRenderer->update();
            m_treeRenderer2->setSelected( id );
            m_treeRenderer2->update();

            glDeleteTextures( 1, &m_textureGLuint );
            m_textureGLuint = 0;
            updateMeshColor();

            Models::g()->submit();
        }
        return true;
    }
    if( target == "tree2" )
    {
        qDebug() << "huhu";
        float moveX = pos.z();
        int leaves = m_root->getNumLeaves();

        float y = 1.0 - ( pos.y() / static_cast<float>( m_height2 ) );

        float zoom = qMin( leaves, m_width2 * ( m_zoom2 - 1 ) ) / 2;
        float range = ( leaves - moveX - zoom ) - ( 0 - moveX + zoom );
        float posXNormalized = ( pos.x() / static_cast<float>( m_width2 ) );
        float x = ( 0 - moveX + zoom ) + range * posXNormalized;

        qDebug() << x << moveX << zoom;
        int id = pickClusterRec( m_root, 0, m_width2 * leaves, x, y );
        qDebug() << id;
        if ( id != -1 )
        {
            if ( m_nodes[id]->getParent() )
            {
                m_tree = m_nodes[id]->getParent();
            }
            m_properties["maingl"].set( Fn::Property::D_TREE_SELECTED_CLUSTER, id );
            QColor unselectedColor = m_properties["maingl"].get( Fn::Property::D_TREE_UNSELECTED_CLUSTER_COLOR ).value<QColor>();
            QColor selectedColor = m_properties["maingl"].get( Fn::Property::D_TREE_SELECTED_CLUSTER_COLOR ).value<QColor>();
            m_root->setColor( 1, unselectedColor , false, true );
            m_tree->setColor( id, 1, selectedColor );
            m_treeRenderer->setTree( m_tree );
            m_treeRenderer->setSelected( id );
            m_treeRenderer->update();
            m_treeRenderer2->setSelected( id );
            m_treeRenderer2->update();

            glDeleteTextures( 1, &m_textureGLuint );
            m_textureGLuint = 0;
            updateMeshColor();

            Models::g()->submit();
        }
        return true;
    }
    return false;
}

int DatasetTree::pickClusterRec( Tree* tree, int left, int right, float x, float y )
{
    if ( ( tree->getValue() > ( y - 0.01 ) ) && ( tree->getValue() < ( y + 0.01 ) ) )
    {
        if( x > left && x < right )
        {
            return tree->getId();
        }
    }
    else
    {
        QList<Tree*> children = tree->getChildren();

        int offset = 0;
        for ( int i = 0; i < children.size(); ++i )
        {
            Tree* child = children[i];

            int size = child->getNumLeaves();

            int id = pickClusterRec( child, left + offset, left + offset + size, x, y );
            if( id != -1 )
            {
                return id;
            }

            offset += size;
        }
    }
    return -1;
}

void DatasetTree::colorSelectionChanged( QVariant value )
{
    if ( m_treeRenderer != 0 )
    {
        m_treeRenderer->setColorIndex( value.toInt() );
        m_treeRenderer->update();
    }

    glDeleteTextures( 1, &m_textureGLuint );
    m_textureGLuint = 0;
    updateMeshColor();
    Models::g()->submit();
}

void DatasetTree::setClusterColor()
{
    QColor color = m_properties["maingl"].get( Fn::Property::D_TREE_USER_CLUSTER_COLOR ).value<QColor>();
    int id = m_properties["maingl"].get( Fn::Property::D_TREE_SELECTED_CLUSTER ).toInt();
    m_tree->setColor( id, 2, color );

    m_treeRenderer->update();

    glDeleteTextures( 1, &m_textureGLuint );
    m_textureGLuint = 0;
    updateMeshColor();
    Models::g()->submit();
}

void DatasetTree::partitionModeChanged( QVariant value )
{
    switch ( value.toInt() )
    {
        case 0:
            m_properties["maingl"].getWidget( Fn::Property::D_TREE_PARTITION_LEVEL )->setHidden( false );
            m_properties["maingl"].getWidget( Fn::Property::D_TREE_PARTITION_SIZE )->setHidden( true );
            break;
        case 1:
            m_properties["maingl"].getWidget( Fn::Property::D_TREE_PARTITION_LEVEL )->setHidden( true );
            m_properties["maingl"].getWidget( Fn::Property::D_TREE_PARTITION_SIZE )->setHidden( false );
            break;
        default:
            break;
    }
}

void DatasetTree::applyPartitionMode()
{
    int mode = m_properties["maingl"].get( Fn::Property::D_TREE_PARTITION_MODE ).toInt();

    int id = m_properties["maingl"].get( Fn::Property::D_TREE_SELECTED_CLUSTER ).toInt();
    Tree* selected = m_nodes[id];

    switch ( mode )
    {
        case 0:
        {
            float value = m_properties["maingl"].get( Fn::Property::D_TREE_PARTITION_LEVEL ).toFloat();
            QList<int>parts;
            QQueue<Tree*>todo;
            todo.enqueue( selected );

            QColor unselectedColor = m_properties["maingl"].get( Fn::Property::D_TREE_UNSELECTED_CLUSTER_COLOR ).value<QColor>();
            m_tree->setColor( 3, unselectedColor , false, true );

            while( !todo.empty() )
            {
                Tree* current = todo.dequeue();
                if ( current->getValue() < value )
                {
                    parts.push_back( current->getId() );
                }
                else
                {
                    QList<Tree*> childs = current->getChildren();
                    for ( int i = 0; i < childs.size(); ++i )
                    {
                        todo.enqueue( childs[i] );
                    }
                }

                ColormapBase cmap = ColormapFunctions::getColormap( 2 );
                for ( int i = 0; i < parts.size(); ++i )
                {
                    float v = ( (float)i/(float)parts.size() );
                    QColor c = cmap.getColor( qMax( 0.0f, qMin( 1.0f, v ) ) );
                    m_tree->setColor( parts[i], 3, c );
                }
            }
            break;
        }
        case 1:
        {
            int targetCount = m_properties["maingl"].get( Fn::Property::D_TREE_PARTITION_SIZE ).toInt();
            QList<int>parts;
            QQueue<Tree*>todo;
            todo.enqueue( selected );

            QColor unselectedColor = m_properties["maingl"].get( Fn::Property::D_TREE_UNSELECTED_CLUSTER_COLOR ).value<QColor>();
            //m_tree->setColor( 3, unselectedColor , false, true );

            while( !todo.empty() )
            {
                if ( todo.size() >= targetCount )
                {
                    break;
                }
                else
                {
                    // order current partitions by size
                    qSort( todo.begin(), todo.end(), PtrGreater() );
                    // take highest
                    Tree* current = todo.dequeue();
                    // enqueue children
                    QList<Tree*> childs = current->getChildren();
                    for ( int i = 0; i < childs.size(); ++i )
                    {
                        todo.enqueue( childs[i] );
                    }
                }

                ColormapBase cmap = ColormapFunctions::getColormap( 2 );
                for ( int i = 0; i < todo.size(); ++i )
                {
                    float v = ( (float)i/(float)todo.size() );
                    QColor c = cmap.getColor( qMax( 0.0f, qMin( 1.0f, v ) ) );
                    m_tree->setColor( todo[i]->getId(), 3, c );
                }
            }
            break;
        }
    }
    m_treeRenderer->update();
    glDeleteTextures( 1, &m_textureGLuint );
    m_textureGLuint = 0;
    updateMeshColor();
    Models::g()->submit();
}

void DatasetTree::updateMeshColor()
{
    int colorID = m_properties["maingl"].get( Fn::Property::D_TREE_COLOR_SELECTION ).toInt();
    qDebug() << colorID;
    qDebug() << "+++" << m_projection.size() << " - " << m_mesh[0]->numVerts();
    if ( m_renderer != 0 )
    {
        m_renderer->beginUpdateColor();

        for ( unsigned int i = 0; i < m_projection.size(); ++i )
        {
            if ( m_projection[i] != -1 )
            {
                for ( unsigned int m = 0; m < m_mesh.size(); ++m )
                {
                    m_mesh[m]->setVertexColor( i, m_nodes[ m_projection[i] ]->getColor( colorID ) );
                }
            }

        }
        m_renderer->endUpdateColor();
    }
}
