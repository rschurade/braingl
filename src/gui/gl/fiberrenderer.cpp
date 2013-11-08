/*
 * fiberrenderer.cpp
 *
 * Created on: 28.12.2012
 * @author Ralph Schurade
 */
#include "fiberrenderer.h"
#include "fiberrendererthread.h"

#include "glfunctions.h"

#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/datasets/fiberselector.h"

#include "../../data/properties/propertygroup.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>

FiberRenderer::FiberRenderer( FiberSelector* selector, QVector< QVector< float > >* data, QVector< QVector< float > >* extraData, int numPoints )  :
    ObjectRenderer(),
    m_selector( selector ),
    vbo( 0 ),
    dataVbo( 0 ),
    m_data( data ),
    m_extraData( extraData ),
    m_numLines( data->size() ),
    m_numPoints( numPoints ),
    m_isInitialized( false )
{
    m_colorField.resize( m_numLines );
}

FiberRenderer::~FiberRenderer()
{
    glDeleteBuffers( 1, &vbo );
    glDeleteBuffers( 1, &dataVbo );
    glDeleteBuffers( 1, &indexVbo );
}

void FiberRenderer::init()
{
    glGenBuffers( 1, &vbo );
    glGenBuffers( 1, &dataVbo );
    glGenBuffers( 1, &indexVbo );
}

void FiberRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup* props )
{
    float alpha = props->get( Fn::Property::D_ALPHA ).toFloat();
    if ( renderMode == 0 ) // picking
    {
        return;
    }
    else if ( renderMode == 1 ) // we are drawing opaque objects
    {
        if ( alpha < 1.0 )
        {
            // obviously not opaque
            return;
        }
    }
    else // we are drawing tranparent objects
    {
        if ( !(alpha < 1.0 ) )
        {
            // not transparent
            return;
        }
    }

    QGLShaderProgram* program = GLFunctions::getShader( "fiber" );
    program->bind();

    GLFunctions::setupTextures();
    GLFunctions::setTextureUniforms( GLFunctions::getShader( "fiber" ), "maingl" );

    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );

    initGeometry();

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    setShaderVars( props );

    glBindBuffer( GL_ARRAY_BUFFER, dataVbo );
    int extraLocation = program->attributeLocation( "a_extra" );
    program->enableAttributeArray( extraLocation );
    glVertexAttribPointer( extraLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0 );

    glBindBuffer( GL_ARRAY_BUFFER, indexVbo );
    int indexLocation = program->attributeLocation( "a_indexes" );
    program->enableAttributeArray( indexLocation );
    glVertexAttribPointer( indexLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0 );

    program->setUniformValue( "u_alpha", alpha );
    program->setUniformValue( "u_renderMode", renderMode );
    program->setUniformValue( "u_canvasSize", width, height );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );
    program->setUniformValue( "P0", 12 );
    program->setUniformValue( "C5", 13 );
    program->setUniformValue( "u_fibGrowth", props->get( Fn::Property::D_FIBER_GROW_LENGTH).toFloat() );

    program->setUniformValue( "u_lighting", props->get( Fn::Property::D_LIGHT_SWITCH ).toBool() );
    program->setUniformValue( "u_lightAmbient", props->get( Fn::Property::D_LIGHT_AMBIENT ).toFloat() );
    program->setUniformValue( "u_lightDiffuse", props->get( Fn::Property::D_LIGHT_DIFFUSE ).toFloat() );
    program->setUniformValue( "u_materialAmbient", props->get( Fn::Property::D_MATERIAL_AMBIENT ).toFloat() );
    program->setUniformValue( "u_materialDiffuse", props->get( Fn::Property::D_MATERIAL_DIFFUSE ).toFloat() );
    program->setUniformValue( "u_materialSpecular", props->get( Fn::Property::D_MATERIAL_SPECULAR ).toFloat() );
    program->setUniformValue( "u_materialShininess", props->get( Fn::Property::D_MATERIAL_SHININESS ).toFloat() );


    glLineWidth( props->get( Fn::Property::D_FIBER_THICKNESS ).toFloat() );

    QVector<bool>*selected = m_selector->getSelection();

    for ( int i = 0; i < m_data->size(); ++i )
    {
        if ( selected->at( i ) )
        {
            program->setUniformValue( "u_color", m_colorField[i].redF(),
                                                   m_colorField[i].greenF(),
                                                   m_colorField[i].blueF(), 1.0 );
            program->setUniformValue( "u_globalColor", m_globalColors[i].x(),
                                                         m_globalColors[i].y(),
                                                         m_globalColors[i].z(), 1.0 );
            glDrawArrays( GL_LINE_STRIP, m_startIndexes[i], m_pointsPerLine[i] );
        }
    }


    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void FiberRenderer::setupTextures()
{
}

void FiberRenderer::setShaderVars( PropertyGroup* props )
{
    QGLShaderProgram* program = GLFunctions::getShader( "fiber" );

    program->bind();

    intptr_t offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int numFloats = 6;

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * numFloats, (const void *) offset );
    offset += sizeof(float) * 3;

    int normalLocation = program->attributeLocation( "a_normal" );
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer( normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * numFloats, (const void *) offset );
    offset += sizeof(float) * 3;

    program->setUniformValue( "u_colorMode", props->get( Fn::Property::D_COLORMODE ).toInt() );
    program->setUniformValue( "u_colormap", props->get( Fn::Property::D_COLORMAP ).toInt() );
    program->setUniformValue( "u_color", 1.0, 0.0, 0.0, 1.0 );
    program->setUniformValue( "u_selectedMin", props->get( Fn::Property::D_SELECTED_MIN ).toFloat() );
    program->setUniformValue( "u_selectedMax", props->get( Fn::Property::D_SELECTED_MAX ).toFloat() );
    program->setUniformValue( "u_lowerThreshold", props->get( Fn::Property::D_LOWER_THRESHOLD ).toFloat() );
    program->setUniformValue( "u_upperThreshold", props->get( Fn::Property::D_UPPER_THRESHOLD ).toFloat() );
    program->setUniformValue( "u_dx", props->get( Fn::Property::D_DX ).toFloat() );
    program->setUniformValue( "u_dy", props->get( Fn::Property::D_DY ).toFloat() );
    program->setUniformValue( "u_dz", props->get( Fn::Property::D_DZ ).toFloat() );
    program->setUniformValue( "u_x", props->get( Fn::Property::D_NX ).toFloat() / 10.f );
    program->setUniformValue( "u_y", props->get( Fn::Property::D_NY ).toFloat() / 10.f );
    program->setUniformValue( "u_z", props->get( Fn::Property::D_NZ ).toFloat() / 10.f );
}

void FiberRenderer::initGeometry()
{
    if ( m_isInitialized )
    {
        return;
    }
    qDebug() << "create fiber vbo's...";

    std::vector<float>verts;

    try
    {
        verts.reserve( m_numPoints * 6 );
        m_globalColors.reserve( m_numLines * 3 );
    }
    catch ( std::bad_alloc& )
    {
        qDebug() << "***error*** failed to allocate enough memory for vbo";
        exit ( 0 );
    }


    for ( int i = 0; i < m_data->size(); ++i )
    {
        QVector<float> fib = m_data->at(i);

        if ( fib.size() < 6 )
        {
            printf( "fib with size < 2 detected" );
            continue;
        }

        int numFloats = fib.size();
        QVector3D lineStart( fib[0], fib[1], fib[2] );
        QVector3D lineEnd( fib[numFloats-3], fib[numFloats-2], fib[numFloats-1] );

        QVector3D gc( fabs( lineStart.x() - lineEnd.x() ), fabs( lineStart.y() - lineEnd.y() ), fabs( lineStart.z() - lineEnd.z() ) );
        gc.normalize();
        m_globalColors.push_back( gc );

        // push back the first vertex, done seperately because of nomal calculation
        verts.push_back( fib[0] );
        verts.push_back( fib[1] );
        verts.push_back( fib[2] );

        QVector3D localColor( fabs( fib[0] - fib[3] ), fabs( fib[1] - fib[4] ), fabs( fib[2] - fib[5] ) );
        localColor.normalize();

        verts.push_back( localColor.x() );
        verts.push_back( localColor.y() );
        verts.push_back( localColor.z() );

        for ( int k = 1; k < fib.size() / 3 - 1; ++k )
        {
            verts.push_back( fib[k*3] );
            verts.push_back( fib[k*3+1] );
            verts.push_back( fib[k*3+2] );

            QVector3D localColor( fabs( fib[k*3-3] - fib[k*3+3] ), fabs( fib[k*3-2] - fib[k*3+4] ), fabs( fib[k*3-1] - fib[k*3+5] ) );
            localColor.normalize();

            verts.push_back( localColor.x() );
            verts.push_back( localColor.y() );
            verts.push_back( localColor.z() );

        }

        // push back the last vertex, done seperately because of nomal calculation
        verts.push_back( fib[numFloats-3] );
        verts.push_back( fib[numFloats-2] );
        verts.push_back( fib[numFloats-1] );

        QVector3D localColor2( fabs( fib[numFloats-6] - fib[numFloats-3] ), fabs( fib[numFloats-5] - fib[numFloats-2] ), fabs( fib[numFloats-4] - fib[numFloats-1] ) );
        localColor.normalize();

        verts.push_back( localColor2.x() );
        verts.push_back( localColor2.y() );
        verts.push_back( localColor2.z() );
    }

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    verts.clear();
    //verts.squeeze();

    m_pointsPerLine.resize( m_data->size() );
    m_startIndexes.resize( m_data->size() );

    int currentStart = 0;
    for ( int i = 0; i < m_data->size(); ++i )
    {
        m_pointsPerLine[i] = m_data->at( i ).size() / 3;
        m_startIndexes[i] = currentStart;
        currentStart += m_pointsPerLine[i];
    }

    updateExtraData( m_extraData );

    qDebug() << "create fiber vbo's done";

    m_numPoints = verts.size() / 6;

    m_isInitialized = true;
}

void FiberRenderer::colorChanged( QVariant color )
{
    QVector<bool>*selected = m_selector->getSelection();
    for ( int i = 0; i < m_numLines; ++i )
    {
        if ( selected->at( i ) )
        {
            m_colorField.replace( i, color.value<QColor>() );
        }
    }
}

void FiberRenderer::updateExtraData( QVector< QVector< float > >* extraData )
{
    m_extraData = extraData;
    QVector<float>data;
    QVector<float>indexes;
    for ( int i = 0; i < extraData->size(); ++i )
    {
        QVector<float>fib = extraData->at(i);
        for ( int k = 0; k < fib.size(); ++k )
        {
            data.push_back( fib[k] );
            indexes.push_back( k );
        }
    }

    glDeleteBuffers( 1, &dataVbo );
    glGenBuffers( 1, &dataVbo );

    glDeleteBuffers( 1, &indexVbo );
    glGenBuffers( 1, &indexVbo );

    glBindBuffer( GL_ARRAY_BUFFER, dataVbo );
    glBufferData( GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, indexVbo );
    glBufferData( GL_ARRAY_BUFFER, indexes.size() * sizeof(GLfloat), indexes.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

}
