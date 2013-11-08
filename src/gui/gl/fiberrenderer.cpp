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

FiberRenderer::FiberRenderer( FiberSelector* selector, QVector< QVector< float > >* data, QVector< QVector< float > >* extraData )  :
    ObjectRenderer(),
    m_selector( selector ),
    vbo( 0 ),
    dataVbo( 0 ),
    m_data( data ),
    m_extraData( extraData ),
    m_numLines( data->size() ),
    m_numPoints( 0 ),
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
            program->setUniformValue( "u_globalColor", m_globalColorField[i].x(),
                                                         m_globalColorField[i].y(),
                                                         m_globalColorField[i].z(), 1.0 );
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
    int numThreads = GLFunctions::idealThreadCount;

    QVector<FiberRendererThread*> threads;
    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new FiberRendererThread( m_data, i ) );
    }

    // run threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads[i]->start();
    }

    // wait for all threads to finish
    for ( int i = 0; i < numThreads; ++i )
    {
        threads[i]->wait();
    }

    QVector<float> verts;
    m_globalColorField.clear();
    // combine verts from all threads
    for ( int i = 0; i < numThreads; ++i )
    {
        verts += *( threads[i]->getVerts() );
        m_globalColorField += *( threads[i]->getGlobalColors() );
    }

    for ( int i = 0; i < numThreads; ++i )
    {
        delete threads[i];
    }

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

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
