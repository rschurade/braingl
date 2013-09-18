/*
 * datasettree.cpp
 *
 *  Created on: 17.09.2013
 *      Author: Ralph
 */

#include "datasettree.h"

#include "../../gui/gl/colormapfunctions.h"
#include "../../algos/colormapbase.h"


#include "../../algos/tree.h"

#include <QDebug>

DatasetTree::DatasetTree( QDir fn ) :
    Dataset( fn , Fn::DatasetType::TREE ),
    m_tree( 0 ),
    m_numLeaves( 0 ),
    m_numNodes( 0 )
{
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP, -1 );
    m_properties["maingl"]->create( Fn::Property::D_INTERPOLATION, false );
    m_properties["maingl"]->create( Fn::Property::D_ALPHA, 1.0f, 0.0, 1.0 );
    m_properties["maingl"]->create( Fn::Property::D_DIM, 3 );

    m_properties["maingl"]->create( Fn::Property::D_TREE_SELECTED_CLUSTER, 0, 0, 0, "general" );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_TREE_SELECTED_CLUSTER ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( selectCluster( QVariant ) ) );
    m_properties["maingl"]->create( Fn::Property::D_TREE_SELECTED_CLUSTER_COLOR, QColor( 255, 0, 0 ), "general" );
    m_properties["maingl"]->create( Fn::Property::D_TREE_UNSELECTED_CLUSTER_COLOR, QColor( 128, 128, 128 ), "general" );

    PropertyGroup* props2 = new PropertyGroup( *( m_properties["maingl"] ) );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"]->getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );

}

DatasetTree::~DatasetTree()
{
}

void DatasetTree::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{

}

void DatasetTree::importTree( QString dims, QVector<QString>coords, QVector<QString>clusters )
{
    QStringList dl = dims.split( " " );

    m_properties["maingl"]->create( Fn::Property::D_NX, dl[0].toInt() );
    m_properties["maingl"]->create( Fn::Property::D_NY, dl[1].toInt() );
    m_properties["maingl"]->create( Fn::Property::D_NZ, dl[2].toInt() );
    m_properties["maingl"]->create( Fn::Property::D_DX, 1.0f );
    m_properties["maingl"]->create( Fn::Property::D_DY, 1.0f );
    m_properties["maingl"]->create( Fn::Property::D_DZ, 1.0f );


    QVector<Tree*>nodes;
    QVector<Tree*>leaves( coords.size() );

    m_numLeaves = coords.size();
    m_numNodes = clusters.size();

    m_properties["maingl"]->setMax( Fn::Property::D_TREE_SELECTED_CLUSTER, m_numLeaves + m_numNodes );

    qDebug() << "num leaves:" << m_numLeaves << "num nodes:" << m_numNodes;

    int currentId = coords.size();

    ColormapBase cmap = ColormapFunctions::getColormap( 2 );

    for( int i = 0; i < clusters.size(); ++i )
    {
        QStringList cl = clusters[i].split( " " );
        float value = cl[0].toFloat();

        m_tree = new Tree( currentId++, value );

        nodes.push_back( m_tree );

        for ( int k = 1; k < cl.size(); k += 2 )
        {
            bool isNode = cl[k].toInt();
            int id = cl[k+1].toInt();
            if( isNode )
            {
                if( id < nodes.size() )
                {
                    nodes[id]->setParent( m_tree );
                    m_tree->addChild( nodes[id] );
                }
                else
                {
                    qDebug() << "node id error";
                }
            }
            else
            {
                Tree* leaf = new Tree( id, 0.0 );
                leaf->setParent( m_tree );
                m_tree->addChild( leaf );

                QStringList cList = coords[id].split( " " );
                QVector3D pos( cList[0].toInt(), cList[1].toInt(), cList[2].toInt() );
                leaf->setTexturePosition( pos );
                leaves[id] = leaf;
            }
        }
    }

    for( int i = 0; i < leaves.size(); ++i )
    {
        float v = ( (float)i/(float)m_numLeaves );
        QColor c = cmap.getColor( qMax( 0.0f, qMin( 1.0f, v ) ) );
        leaves[i]->setColor( 0, c, true, false );
    }

    nodes.clear();
    leaves.clear();
    qDebug() << "test num leaves:" << m_tree->getNumLeaves();

    createTexture();
    m_properties["maingl"]->set( Fn::Property::D_HAS_TEXTURE, true );
}

void DatasetTree::createTexture()
{
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_textureGLuint );

    glBindTexture( GL_TEXTURE_3D, m_textureGLuint );

    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP );

    int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
    int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
    int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

    int blockSize = nx * ny * nz;
    m_textureData.resize( blockSize * 3 );
    for ( int i = 0; i < blockSize * 3; ++i )
    {
        m_textureData[i] = 0.0;
    }

    createTextureRec( m_tree );

    glTexImage3D( GL_TEXTURE_3D, 0, GL_RGBA, nx, ny, nz, 0, GL_RGB, GL_FLOAT, m_textureData.data() );
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
        int nx = m_properties["maingl"]->get( Fn::Property::D_NX ).toInt();
        int ny = m_properties["maingl"]->get( Fn::Property::D_NY ).toInt();
        //int nz = m_properties["maingl"]->get( Fn::Property::D_NZ ).toInt();

        QVector3D pos = tree->getTexturePosition();
        QColor color = tree->getColor( 0 );

        int id = ( pos.x() + pos.y() * nx + pos.z() * nx * ny ) * 3;

        m_textureData[id] = color.redF();
        m_textureData[id+1] = color.greenF();
        m_textureData[id+2] = color.blueF();
    }
}

void DatasetTree::selectCluster( QVariant id )
{

}
