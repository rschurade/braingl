/*
 * datasetcorrelation.cpp
 *
 *  Created on: Sep 12, 2013
 *      Author: schurade
 */

#include "datasetcorrelation.h"

#include "../models.h"

#include "../mesh/trianglemesh2.h"

#include "../../gui/gl/glfunctions.h"
#include "../../gui/gl/meshrenderer.h"

DatasetCorrelation::DatasetCorrelation( QDir fileName, float minThreshold, float maxThreshold, Fn::DatasetType type ) :
    DatasetMesh( fileName, type ),
    m_minThreshold( minThreshold ),
    m_maxThreshold( maxThreshold ),
    m_correlations( NULL ),
    m_prevPickedID( -1 )
{
    m_properties["maingl"].getProperty( Fn::Property::D_MIN )->setMin( -1 );
    m_properties["maingl"].getProperty( Fn::Property::D_MIN )->setValue( -1 );

    m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MIN )->setMin( -1 );
    m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MIN )->setValue( -1 );
    m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MAX )->setMin( -1 );

    m_properties["maingl"].getProperty( Fn::Property::D_LOWER_THRESHOLD )->setMin( -1 );
    m_properties["maingl"].getProperty( Fn::Property::D_LOWER_THRESHOLD )->setValue( -1 );
    m_properties["maingl"].getProperty( Fn::Property::D_UPPER_THRESHOLD )->setMin( -1 );

    m_properties["maingl"].set( Fn::Property::D_COLORMODE, 3 );

    finalizeProperties();
}

DatasetCorrelation::~DatasetCorrelation()
{
}

void DatasetCorrelation::setCorrelationMatrix( float** matrix )
{
    //TODO: think about data duplication
    delete m_correlations;
    m_correlations = new CorrelationMatrix( getMesh()->numVerts() );
    for ( unsigned int i = 0; i < getMesh()->numVerts(); ++i )
    {
        for ( unsigned int j = 0; j < getMesh()->numVerts(); ++j )
        {
            m_correlations->setValue( i, j, matrix[i][j] );
        }
    }
    m_correlations->setInitialized(true);
}

bool DatasetCorrelation::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target )
{

//TODO: Extra property for radius...
    m_picked = getMesh( target )->pick( pos, m_properties["maingl"].get( Fn::Property::D_PAINTSIZE ).toFloat() );

    int pickedID = getMesh( target )->closestVertexIndex( pos );

    properties( "maingl" ).set( Fn::Property::D_GLYPHSET_PICKED_ID, pickedID );

    DatasetMesh::mousePick( pickId, pos, modifiers, target );
    setPickedID( pickedID );
    return true;
}

void DatasetCorrelation::setPickedID( int pickedID )
{
    m_prevPickedID = pickedID;

    for ( unsigned int i = 0; i < getMesh()->numVerts(); ++i )
    {
        if ( pickedID != -1 )
        {
            for ( unsigned int m = 0; m < m_mesh.size(); m++ )
            {
                m_mesh[m]->setVertexData( i, m_correlations->getValue( pickedID, i ) );
            }
        }
    }
}

void DatasetCorrelation::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target ).get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }
    if ( m_renderer == 0 )
    {
        m_renderer = new MeshRenderer( getMesh( target ) );
        m_renderer->setModel( Models::g() );
        m_renderer->init();
    }
    m_renderer->setMesh( getMesh( target ) );
    m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );

    int pickedID = properties( "maingl" ).get( Fn::Property::D_GLYPHSET_PICKED_ID ).toInt();
    if ( m_prevPickedID != pickedID )
    {
        setPickedID( pickedID );
    }

    if ( ( Models::g()->data( Models::g()->index( (int) Fn::Property::G_RENDER_CROSSHAIRS, 0 ) ).toBool() && ( target == "maingl" ) ) ||
        ( Models::g()->data( Models::g()->index( (int) Fn::Property::G_RENDER_CROSSHAIRS_2, 0 ) ).toBool() && ( target == "maingl2" ) ) )
    {
        if ( pickedID > -1 )
        {
            float sx, sy, sz;
            QVector3D sel = getMesh( target )->getVertex( pickedID );
            sx = sel.x();
            sy = sel.y();
            sz = sel.z();
            QColor color = Models::g()->data( Models::g()->index( (int) Fn::Property::G_CROSSHAIR_COLOR, 0 ) ).value<QColor>();
            color.setAlphaF( 0.8 );

            float s = 5.0;
            float t = 0.5;

            GLFunctions::drawSphere( pMatrix, mvMatrix, sx, sy, sz, s, s, s, color, 0, width, height, renderMode );

            GLFunctions::drawBox( pMatrix, mvMatrix, sx + s, sy, sz, s, t, t, QColor( 255.0, 0.0, 0.0 ), 0, width, height, renderMode );
            GLFunctions::drawBox( pMatrix, mvMatrix, sx - s, sy, sz, s, t, t, QColor( 255.0, 0.0, 0.0 ), 0, width, height, renderMode );

            GLFunctions::drawBox( pMatrix, mvMatrix, sx, sy + s, sz, t, s, t, QColor( 0.0, 255.0, 0.0 ), 0, width, height, renderMode );
            GLFunctions::drawBox( pMatrix, mvMatrix, sx, sy - s, sz, t, s, t, QColor( 0.0, 255.0, 0.0 ), 0, width, height, renderMode );

            GLFunctions::drawBox( pMatrix, mvMatrix, sx, sy, sz + s, t, t, s, QColor( 0.0, 0.0, 255.0 ), 0, width, height, renderMode );
            GLFunctions::drawBox( pMatrix, mvMatrix, sx, sy, sz - s, t, t, s, QColor( 0.0, 0.0, 255.0 ), 0, width, height, renderMode );
        }
    }
}

QString DatasetCorrelation::getSaveFilter()
{
    return QString( "Mesh binary (*.vtk);; Mesh ascii (*.vtk);; Mesh 1D data (*.1D);; Mesh rgb data (*.rgb);; Mesh roi data (*.roi);; Binary connectivity matrix (*.bin);; all files (*.*)" );
}

QString DatasetCorrelation::getDefaultSuffix()
{
    return QString( "vtk" );
}

void DatasetCorrelation::saveBinaryMatrix( QString filename )
{
    m_correlations->save( filename );
}
