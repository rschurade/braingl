/*
 * fiberrenderer.cpp
 *
 * Created on: 28.12.2012
 * @author Ralph Schurade
 */
#include "tuberenderer.h"
#include "tuberendererthread.h"

#include "glfunctions.h"

#include "../../data/enums.h"
#include "../../data/datasets/fiberselector.h"
#include "../../data/properties/propertygroup.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>

TubeRenderer::TubeRenderer( FiberSelector* selector, std::vector<Fib>* fibs )  :
    ObjectRenderer(),
    m_selector( selector ),
    vboIds( new GLuint[ 4 ] ),
    m_fibs( fibs ),
    m_numLines( fibs->size() ),
    m_numPoints( 0 ),
    m_isInitialized( false )
{
}

TubeRenderer::~TubeRenderer()
{
    glDeleteBuffers( 4, vboIds );
}

void TubeRenderer::init()
{
    glGenBuffers( 4, vboIds );
}

void TubeRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props )
{
    float alpha = props.get( Fn::Property::D_ALPHA ).toFloat();

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

    QGLShaderProgram* program = GLFunctions::getShader( "tube" );
    program->bind();

    GLFunctions::setupTextures();
    GLFunctions::setTextureUniforms( GLFunctions::getShader( "tube" ), "maingl" );

    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "mv_matrixTI", mv_matrix.transposed().inverted() );


    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    initGeometry();

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    setShaderVars( props );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[2] );
    int extraLocation = program->attributeLocation( "a_extra" );
    program->enableAttributeArray( extraLocation );
    glVertexAttribPointer( extraLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float), 0 );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[3] );
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

    std::vector<bool>*selected = m_selector->getSelection();
    for ( unsigned int i = 0; i < m_fibs->size(); ++i )
    {
        if ( selected->at( i ) )
        {
            QColor c = m_fibs->at( i ).customColor();
            program->setUniformValue( "u_color", c.redF(), c.greenF(), c.blueF(), 1.0 );
            c = m_fibs->at( i ).globalColor();
            program->setUniformValue( "u_globalColor", c.redF(), c.greenF(), c.blueF(), 1.0 );
            //glDrawArrays( GL_QUAD_STRIP, m_startIndexes[i]*2, m_pointsPerLine[i]*2 ); // XXX not in Core
            glDrawArrays( GL_TRIANGLE_STRIP, m_startIndexes[i]*2, m_pointsPerLine[i]*2 ); // XXX quadstrip indices should also work for tristrip
        }
    }

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

void TubeRenderer::setupTextures()
{
}

void TubeRenderer::setShaderVars( PropertyGroup& props )
{
    QGLShaderProgram* program = GLFunctions::getShader( "tube" );

    program->bind();

    intptr_t offset = 0;
    // Tell OpenGL programmable pipeline how to locate vertex position data
    int numFloats = 7;

    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * numFloats, (const void *) offset );

    offset += sizeof(float) * 3;
    int normalLocation = program->attributeLocation( "a_normal" );
    program->enableAttributeArray( normalLocation );
    glVertexAttribPointer( normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * numFloats, (const void *) offset );

    offset += sizeof(float) * 3;
    int dirLocation = program->attributeLocation( "a_direction" );
    program->enableAttributeArray( dirLocation );
    glVertexAttribPointer( dirLocation, 1, GL_FLOAT, GL_FALSE, sizeof(float) * numFloats, (const void *) offset );

    program->setUniformValue( "u_fibGrowth", props.get( Fn::Property::D_FIBER_GROW_LENGTH).toFloat() );
    program->setUniformValue( "u_colorMode", props.get( Fn::Property::D_COLORMODE ).toInt() );
    program->setUniformValue( "u_colormap", props.get( Fn::Property::D_COLORMAP ).toInt() );
    program->setUniformValue( "u_color", 1.0, 0.0, 0.0, 1.0 );
    program->setUniformValue( "u_selectedMin", props.get( Fn::Property::D_SELECTED_MIN ).toFloat() );
    program->setUniformValue( "u_selectedMax", props.get( Fn::Property::D_SELECTED_MAX ).toFloat() );
    program->setUniformValue( "u_lowerThreshold", props.get( Fn::Property::D_LOWER_THRESHOLD ).toFloat() );
    program->setUniformValue( "u_upperThreshold", props.get( Fn::Property::D_UPPER_THRESHOLD ).toFloat() );
    program->setUniformValue( "u_thickness", props.get( Fn::Property::D_FIBER_THICKNESS ).toFloat() / 100.f );
}

void TubeRenderer::initGeometry()
{
    if ( m_isInitialized )
    {
        return;
    }
    qDebug() << "create tube vbo's...";
    int numThreads = GLFunctions::idealThreadCount;

    std::vector<TubeRendererThread*> threads;
    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        threads.push_back( new TubeRendererThread( m_fibs, i ) );
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

    std::vector<float> verts;
    // combine verts from all threads
    for ( int i = 0; i < numThreads; ++i )
    {
        verts.insert( verts.end(), threads[i]->getVerts()->begin(), threads[i]->getVerts()->end() );
    }

    for ( int i = 0; i < numThreads; ++i )
    {
        delete threads[i];
    }

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    m_pointsPerLine.resize( m_fibs->size() );
    m_startIndexes.resize( m_fibs->size() );

    int currentStart = 0;
    for ( unsigned int i = 0; i < m_fibs->size(); ++i )
    {
        m_pointsPerLine[i] = m_fibs->at( i ).length();
        m_startIndexes[i] = currentStart;
        currentStart += m_pointsPerLine[i];
    }

    updateExtraData( 0 );

    qDebug() << "create tube vbo's done";

    m_numPoints = verts.size() / 7;

    m_isInitialized = true;
}

void TubeRenderer::colorChanged()
{
}

void TubeRenderer::updateExtraData( unsigned int dataFieldId )
{
    std::vector<float>data;
        std::vector<float>indexes;
        for ( unsigned int i = 0; i < m_fibs->size(); ++i )
        {
            Fib fib = m_fibs->at(i);
            for ( unsigned int k = 0; k < fib.length(); ++k )
            {
                data.push_back( fib.getData( dataFieldId, k ) );
                data.push_back( fib.getData( dataFieldId, k ) );
                indexes.push_back( k );
                indexes.push_back( k );
            }
        }
    glDeleteBuffers( 1, &vboIds[2] );
    glGenBuffers( 1, &vboIds[2] );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[2] );
    glBufferData( GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glDeleteBuffers( 1, &vboIds[3] );
    glGenBuffers( 1, &vboIds[3] );

    glBindBuffer( GL_ARRAY_BUFFER, vboIds[3] );
    glBufferData( GL_ARRAY_BUFFER, indexes.size() * sizeof(GLfloat), indexes.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
