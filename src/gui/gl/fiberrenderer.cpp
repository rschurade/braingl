/*
 * fiberrenderer.cpp
 *
 * Created on: 28.12.2012
 * @author Ralph Schurade
 */
#include "fiberrenderer.h"

#include "glfunctions.h"

#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/datasets/fiberselector.h"

#include "../../data/properties/propertygroup.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>

#include "math.h"

FiberRenderer::FiberRenderer( FiberSelector* selector,
                                  std::vector<Fib>* fibs,
                                  int numPoints )  :
    m_selector( selector ),
    vbo( 0 ),
    dataVbo( 0 ),
    indexVbo( 0 ),
    m_fibs( fibs ),
    m_numLines( fibs->size() ),
    m_numPoints( numPoints ),
    m_isInitialized( false ),
    m_updateExtraData( false ),
    m_selectedExtraData( 0 )
{
}

FiberRenderer::~FiberRenderer()
{
    glDeleteBuffers( 1, &vbo );
    glDeleteBuffers( 1, &dataVbo );
    glDeleteBuffers( 1, &indexVbo );
}

void FiberRenderer::init()
{
    initializeOpenGLFunctions();
    glGenBuffers( 1, &vbo );
    glGenBuffers( 1, &dataVbo );
    glGenBuffers( 1, &indexVbo );
}

void FiberRenderer::draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, PropertyGroup& props )
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

    if ( m_updateExtraData )
    {
    	updateExtraData( m_selectedExtraData );
    }

    QGLShaderProgram* program = GLFunctions::getShader( "fiber" );
    program->bind();

    GLFunctions::setupTextures();
    GLFunctions::setTextureUniforms( GLFunctions::getShader( "fiber" ), "maingl" );

    // Set modelview-projection matrix
    program->setUniformValue( "mvp_matrix", p_matrix * mv_matrix );
    program->setUniformValue( "mv_matrixInvert", mv_matrix.inverted() );
    program->setUniformValue( "mv_matrixTI", mv_matrix.transposed().inverted() );
    program->setUniformValue( "userTransformMatrix", props.get( Fn::Property::D_TRANSFORM ).value<QMatrix4x4>() );

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
    program->setUniformValue( "u_fibGrowth", props.get( Fn::Property::D_FIBER_GROW_LENGTH).toFloat() );

    program->setUniformValue( "u_lighting", props.get( Fn::Property::D_LIGHT_SWITCH ).toBool() );
    program->setUniformValue( "u_lightAmbient", props.get( Fn::Property::D_LIGHT_AMBIENT ).toFloat() );
    program->setUniformValue( "u_lightDiffuse", props.get( Fn::Property::D_LIGHT_DIFFUSE ).toFloat() );
    program->setUniformValue( "u_materialAmbient", props.get( Fn::Property::D_MATERIAL_AMBIENT ).toFloat() );
    program->setUniformValue( "u_materialDiffuse", props.get( Fn::Property::D_MATERIAL_DIFFUSE ).toFloat() );
    program->setUniformValue( "u_materialSpecular", props.get( Fn::Property::D_MATERIAL_SPECULAR ).toFloat() );
    program->setUniformValue( "u_materialShininess", props.get( Fn::Property::D_MATERIAL_SHININESS ).toFloat() );

    glLineWidth( props.get( Fn::Property::D_FIBER_THICKNESS ).toFloat() );

    std::vector<bool>*selected = m_selector->getSelection();

    int percent = props.get( Fn::Property::D_FIBER_THIN_OUT ).toFloat() * 10;

    for ( unsigned int i = 0; i < m_fibs->size(); ++i )
    {
        if ( ( i % 1000 ) > percent )
        {
            continue;
        }

        if ( selected->at( i ) )
        {
            QColor c = m_fibs->at( i ).customColor();
            program->setUniformValue( "u_color", c.redF(), c.greenF(), c.blueF(), 1.0 );
            c = m_fibs->at( i ).globalColor();
            program->setUniformValue( "u_globalColor", c.redF(), c.greenF(), c.blueF(), 1.0 );
            glDrawArrays( GL_LINE_STRIP, m_startIndexes[i], m_pointsPerLine[i] );
        }
        else
        {
            if ( Models::getGlobal( Fn::Property::G_UNSELECTED_FIBERS_GREY ).toBool() )
            {
                program->setUniformValue( "u_color", .4f, .4f, .4f, 1.0 );
                program->setUniformValue( "u_globalColor", .4f, .4f, .4f, 1.0 );
                glDrawArrays( GL_LINE_STRIP, m_startIndexes[i], m_pointsPerLine[i] );
            }
        }
    }


    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void FiberRenderer::setupTextures()
{
}

void FiberRenderer::setShaderVars( PropertyGroup& props )
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

    program->setUniformValue( "u_colorMode", props.get( Fn::Property::D_COLORMODE ).toInt() );
    program->setUniformValue( "u_mriSource", props.get( Fn::Property::D_STIPPLE_PROB_MASK ).toInt() );
    program->setUniformValue( "u_colormap", props.get( Fn::Property::D_COLORMAP ).toInt() );
    program->setUniformValue( "u_color", 1.0, 0.0, 0.0, 1.0 );

    if ( props.get( Fn::Property::D_COLORMODE ).toInt() == 3 )
    {
        float texMin = props.get( Fn::Property::D_MIN ).toFloat();
        float texMax = props.get( Fn::Property::D_MAX ).toFloat();
        program->setUniformValue( "u_lowerThreshold", ( props.get( Fn::Property::D_LOWER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
        program->setUniformValue( "u_upperThreshold", ( props.get( Fn::Property::D_UPPER_THRESHOLD ).toFloat() - texMin ) / ( texMax - texMin ) );
        program->setUniformValue( "u_selectedMin", ( props.get( Fn::Property::D_SELECTED_MIN ).toFloat() - texMin ) / ( texMax - texMin ) );
        program->setUniformValue( "u_selectedMax", ( props.get( Fn::Property::D_SELECTED_MAX ).toFloat() - texMin ) / ( texMax - texMin ) );
    }
    else
    {
        program->setUniformValue( "u_selectedMin", props.get( Fn::Property::D_SELECTED_MIN ).toFloat() );
        program->setUniformValue( "u_selectedMax", props.get( Fn::Property::D_SELECTED_MAX ).toFloat() );
        program->setUniformValue( "u_lowerThreshold", props.get( Fn::Property::D_LOWER_THRESHOLD ).toFloat() );
        program->setUniformValue( "u_upperThreshold", props.get( Fn::Property::D_UPPER_THRESHOLD ).toFloat() );
    }
    program->setUniformValue( "u_cutdx", props.get( Fn::Property::D_DX ).toFloat() );
    program->setUniformValue( "u_cutdy", props.get( Fn::Property::D_DY ).toFloat() );
    program->setUniformValue( "u_cutdz", props.get( Fn::Property::D_DZ ).toFloat() );
    program->setUniformValue( "u_cutx", props.get( Fn::Property::D_NX ).toFloat() / 10.f );
    program->setUniformValue( "u_cuty", props.get( Fn::Property::D_NY ).toFloat() / 10.f );
    program->setUniformValue( "u_cutz", props.get( Fn::Property::D_NZ ).toFloat() / 10.f );
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
    }
    catch ( std::bad_alloc& )
    {
        qCritical() << "***error*** failed to allocate enough memory for vbo";
        exit ( 0 );
    }


    for ( unsigned int i = 0; i < m_fibs->size(); ++i )
    {
        Fib fib = m_fibs->at(i);

        if ( fib.length() < 2 )
        {
            printf( "fib with size < 2 detected" );
            continue;
        }

        QVector3D lineStart = fib.firstVert();
        QVector3D lineEnd = fib.lastVert();

        // push back the first vertex, done seperately because of nomal calculation
        verts.push_back( lineStart.x() );
        verts.push_back( lineStart.y() );
        verts.push_back( lineStart.z() );

        QVector3D secondVert = fib.getVert( 1 );

        QVector3D localColor( lineStart.x() - secondVert.x(), lineStart.y() - secondVert.y(), lineStart.z() - secondVert.z() );
        localColor.normalize();

        verts.push_back( localColor.x() );
        verts.push_back( localColor.y() );
        verts.push_back( localColor.z() );

        for ( unsigned int k = 1; k < fib.length() - 1; ++k )
        {
            verts.push_back( fib[k].x() );
            verts.push_back( fib[k].y() );
            verts.push_back( fib[k].z() );

            QVector3D localColor( fib[k-1].x() - fib[k+1].x(), fib[k-1].y() - fib[k+1].y(), fib[k-1].z() - fib[k+1].z() );
            localColor.normalize();

            verts.push_back( localColor.x() );
            verts.push_back( localColor.y() );
            verts.push_back( localColor.z() );

        }

        // push back the last vertex, done seperately because of nomal calculation
        verts.push_back( lineEnd.x() );
        verts.push_back( lineEnd.y() );
        verts.push_back( lineEnd.z() );

        QVector3D sec2last = fib[ fib.length() - 2 ];
        QVector3D localColor2( sec2last.x() - lineEnd.x(), sec2last.y() - lineEnd.y(), sec2last.z() - lineEnd.z() );
        localColor.normalize();

        verts.push_back( localColor2.x() );
        verts.push_back( localColor2.y() );
        verts.push_back( localColor2.z() );
    }

    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, verts.size() * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    verts.clear();

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

    qDebug() << "create fiber vbo's done";

    m_numPoints = verts.size() / 6;

    m_isInitialized = true;
}

void FiberRenderer::colorChanged()
{
}

void FiberRenderer::setExtraData( unsigned int dataFieldId )
{
	m_updateExtraData = true;
	m_selectedExtraData = dataFieldId;
}

void FiberRenderer::updateExtraData( unsigned int dataFieldId )
{
    std::vector<float>data;
    std::vector<float>indexes;
    for ( unsigned int i = 0; i < m_fibs->size(); ++i )
    {
        Fib fib = m_fibs->at(i);
        for ( unsigned int k = 0; k < fib.length(); ++k )
        {
            data.push_back( fib.getData( dataFieldId, k ) );
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

    m_updateExtraData = false;
}
