/*
 * objectrenderer.cpp
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */
#include "objectrenderer.h"

#include "../../data/enums.h"

#include <QDebug>

ObjectRenderer::ObjectRenderer() :
	m_previousSettings( "" )
{
}

ObjectRenderer::~ObjectRenderer()
{
}

QString ObjectRenderer::createSettingsString( std::initializer_list<QVariant>settings )
{
    QString result("");

    for ( auto i = settings.begin(); i != settings.end(); ++i )
    {
        result += (*i).toString();
    }
    return result;
}

void ObjectRenderer::init()
{
}

void ObjectRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props )
{
}

void ObjectRenderer::initGeometry( PropertyGroup& props )
{
}

void ObjectRenderer::setShaderVars( PropertyGroup& props )
{
}

void ObjectRenderer::unproject( float x, float y, float z, QMatrix4x4& mvMatrix, QMatrix4x4& pMatrix, QVector4D& viewport, float& xOut, float& yOut, float& zOut )
{
    /* find the inverse modelview-projection-matrix */
    QMatrix4x4 a = ( pMatrix * mvMatrix ).inverted();

    /* transform to normalized coordinates in the range [-1, 1] */
    QVector4D in;
    in.setX( ( x - viewport.x() ) / viewport.z() * 2.0f - 1.0f );
    in.setY( ( y - viewport.y() ) / viewport.w() * 2.0f - 1.0f );
    in.setZ( 2.0f * z - 1.0f );
    in.setW( 1.0f );

    /* find the object's coordinates */
    QVector4D out = a * in;
    if( out.w() != 0.0f )
    {
        out.setW( 1.0f / out.w() );
    }

    /* calculate output */
    xOut = out.x() * out.w();
    yOut = out.y() * out.w();
    zOut = out.z() * out.w();
}

bool ObjectRenderer::project( float x, float y, float z, QMatrix4x4& mvMatrix, QMatrix4x4& pMatrix, QVector4D& viewport, float& xOut, float& yOut, float& zOut )
{
    QVector4D in( x, y, z, 1.0 );
    QVector4D out = mvMatrix * in;
    in = pMatrix * out;
    if ( in.w() == 0.0 )
    {
        return false;
    }
    in.setX( in.x() / in.w() );
    in.setY( in.y() / in.w() );
    in.setZ( in.z() / in.w() );
    /* Map x, y and z to range 0-1 */
    in.setX( in.x() * 0.5 + 0.5 );
    in.setY( in.y() * 0.5 + 0.5 );
    in.setZ( in.z() * 0.5 + 0.5 );
    /* Map x,y to viewport */
    xOut = in.x() * viewport.z() + viewport.x();
    yOut = in.y() * viewport.w() + viewport.y();
    zOut = in.z();
    return true;
}
