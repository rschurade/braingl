/*
 * datasetsurfaceset.cpp
 *
 *  Created on: Apr 22, 2013
 *      Author: boettgerj
 */

#include "datasetsurfaceset.h"

#include "../models.h"

#include "../mesh/trianglemesh2.h"
#include "../../gui/gl/meshrenderer.h"

DatasetSurfaceset::DatasetSurfaceset( QString fn, Fn::DatasetType type ) :
        DatasetMesh( fn, type )
{
    // TODO Auto-generated constructor stub

}

DatasetSurfaceset::~DatasetSurfaceset()
{
    // TODO Auto-generated destructor stub
}

void DatasetSurfaceset::addMesh( TriangleMesh2* tm, QString displayString )
{
    m_mesh.push_back( tm );
    m_displayList << displayString;
}

void DatasetSurfaceset::setProperty()
{
    m_properties.set( Fn::Property::SURFACE, m_displayList, 0, true );
}

TriangleMesh2* DatasetSurfaceset::getMesh()
{
    int n = m_properties.get( Fn::Property::SURFACE ).toInt();
    return m_mesh[n];
}

void DatasetSurfaceset::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode )
{
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( getMesh() );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
    }
    m_renderer->setMesh( getMesh() );
    m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, &m_properties );
}

void DatasetSurfaceset::mousePick( int pickId, QVector3D pos )
{
    if ( pickId == 0 )
    {
        return;
    }

    int paintMode = m_properties.get( Fn::Property::PAINTMODE ).toInt();
    if ( paintMode != 0 )
    {
        QColor color;
        if ( paintMode == 1 )
        {
            color = m_properties.get( Fn::Property::PAINTCOLOR ).value<QColor>();
        }
        else if ( paintMode == 2 )
        {
            color = m_properties.get( Fn::Property::COLOR ).value<QColor>();
        }

        QVector<int> picked = getMesh()->pick( pos, m_properties.get( Fn::Property::PAINTSIZE ).toFloat() );

        if ( picked.size() > 0 )
        {
            m_renderer->beginUpdateColor();
            for ( int i = 0; i < picked.size(); ++i )
            {
                m_renderer->updateColor( picked[i], color.redF(), color.greenF(), color.blueF(), 1.0 );
                for ( int m = 0; m < m_mesh.size(); m++ )
                {
                    m_mesh[m]->setVertexColor( picked[i], color );
                }
            }
            m_renderer->endUpdateColor();
        }
    }
}
