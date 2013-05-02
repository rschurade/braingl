/*
 * scenerenderer.cpp
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */
#include "glfunctions.h"

#include "scenerenderer.h"
#include "arcball.h"
#include "slicerenderer.h"
#include "colormaprenderer.h"

#include "../../data/datastore.h"
#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/datasets/dataset.h"
#include "../../data/vptr.h"
#include "../../data/roi.h"

#include "../../thirdparty/newmat10/newmat.h"

#include <QtOpenGL/QGLShaderProgram>
#include <QDebug>

#include <math.h>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

SceneRenderer::SceneRenderer( QString renderTarget, QItemSelectionModel* roiSelectionModel ) :
    m_renderTarget( renderTarget ),
    m_roiSelectionModel( roiSelectionModel ),
    vboIds( new GLuint[ 2 ] ),
    skipDraw( false ),
    m_boundingbox( 200 ),
    m_nx( 160 ),
    m_ny( 200 ),
    m_nz( 160 ),
    m_width( 1 ),
    m_height( 1 ),
    m_renderMode( 0 ),
    m_ratio( 1.0 ),
    m_picked( 0 ),
    m_sliceXPosAtPick( 0 ),
    m_sliceYPosAtPick( 0 ),
    m_sliceZPosAtPick( 0 )
{
    m_sliceRenderer = new SliceRenderer();
    m_sliceRenderer->setModel( Models::g() );

    m_arcBall = new ArcBall( 400, 400 );

    m_mvMatrix.setToIdentity();
}

SceneRenderer::~SceneRenderer()
{
}

void SceneRenderer::initGL()
{
    qDebug() << "gl init main widget";
    GLenum errorCode = glewInit();
    if ( GLEW_OK != errorCode )
    {
        qDebug() << "Problem: glewInit failed, something is seriously wrong.";
        qDebug() << glewGetErrorString( errorCode );
        exit( false );
    }
    else
    {
        qDebug() << "OpenGL initialized.";
    }

    const GLubyte *renderer = glGetString( GL_RENDERER );
    const GLubyte *vendor = glGetString( GL_VENDOR );
    const GLubyte *version = glGetString( GL_VERSION );
    const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );

    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    qDebug() << "GL Vendor :" << QString( (char*) vendor );
    qDebug() << "GL Renderer :" << QString( (char*) renderer );
    qDebug() << "GL Version (string) :" << QString( (char*) version );
    qDebug() << "GL Version (integer) :" << major << "." << minor;
    qDebug() << "GLSL Version :" << QString( (char*) glslVersion );

    GLFunctions::loadShaders();

    QColor color = Models::g()->data( Models::g()->index( (int)Fn::Global::BACKGROUND_COLOR_MAIN, 0 ) ).value<QColor>();
    glClearColor( color.redF(), color.greenF(), color.blueF(), 1.0 );

    glEnable( GL_DEPTH_TEST );

    glShadeModel( GL_SMOOTH );
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_MULTISAMPLE );

    static GLfloat lightPosition[ 4 ] =
    { 0.5, 5.0, -3000.0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, lightPosition );

    calcMVPMatrix();
    m_sliceRenderer->init();

    int textureSizeMax;
    glGetIntegerv( GL_MAX_TEXTURE_SIZE, &textureSizeMax );
    Models::g()->setData( Models::g()->index( (int)Fn::Global::SCREENSHOT_QUALITY, 0 ), textureSizeMax );
    Models::g()->setData( Models::g()->index( (int)Fn::Global::SCREENSHOT_QUALITY, 0 ), textureSizeMax / 4 );

    VertexData vertices[] =

    {
        { QVector3D( -1.0, -1.0, 0 ), QVector3D( 0.0, 0.0, 0.0 ) },
        { QVector3D( 1.0,  -1.0, 0 ), QVector3D( 1.0, 0.0, 0.0 ) },
        { QVector3D( 1.0,  1.0,  0 ), QVector3D( 1.0, 1.0, 0.0 ) },
        { QVector3D( -1.0, 1.0,  0 ), QVector3D( 0.0, 1.0, 0.0 ) }
    };
    glGenBuffers( 2, vboIds );
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertices, GL_STATIC_DRAW );

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    // Transfer index data to VBO 0
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort), indices, GL_STATIC_DRAW );
}

void SceneRenderer::resizeGL( int width, int height )
{
    m_width = width;
    m_height = height;
    m_arcBall->set_win_size( width, height );

    m_ratio = static_cast<float>( width )/ static_cast<float>(height);
    glViewport( 0, 0, width, height );

    initFBO( width, height );

    calcMVPMatrix();
}

void SceneRenderer::calcMVPMatrix()
{
    m_nx = Models::g()->data( Models::g()->index( (int)Fn::Global::MAX_SAGITTAL, 0 ) ).toFloat();
    m_ny = Models::g()->data( Models::g()->index( (int)Fn::Global::MAX_CORONAL, 0 ) ).toFloat();
    m_nz = Models::g()->data( Models::g()->index( (int)Fn::Global::MAX_AXIAL, 0 ) ).toFloat();
    float dx = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DX, 0 ) ).toFloat();
    float dy = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DY, 0 ) ).toFloat();
    float dz = Models::g()->data( Models::g()->index( (int)Fn::Global::SLICE_DZ, 0 ) ).toFloat();
    m_nx *= dx;
    m_ny *= dy;
    m_nz *= dz;

    m_arcBall->setRotCenter( m_nx / 2., m_ny / 2., m_nz / 2. );

    m_boundingbox = qMax ( m_nx, qMax( m_ny, m_nz ) );

    // Reset projection
    m_pMatrix.setToIdentity();

    float halfBB = m_boundingbox / 2.0;

    float bbx = m_boundingbox;
    float bby = m_boundingbox;

    if ( m_ratio >= 1.0 )
    {
        m_pMatrix.ortho( -halfBB * m_ratio, halfBB * m_ratio, -halfBB, halfBB, -3000, 3000 );
        bbx = m_boundingbox * m_ratio;
    }
    else
    {
        m_pMatrix.ortho( -halfBB, halfBB, -halfBB / m_ratio, halfBB / m_ratio, -3000, 3000 );
        bby = m_boundingbox / m_ratio;
    }

    m_mvMatrix = m_arcBall->getMVMat();

    Models::g()->setData( Models::g()->index( (int)Fn::Global::ZOOM, 0 ), m_arcBall->getZoom() );
    Models::g()->setData( Models::g()->index( (int)Fn::Global::MOVEX, 0 ), m_arcBall->getMoveX() );
    Models::g()->setData( Models::g()->index( (int)Fn::Global::MOVEY, 0 ), m_arcBall->getMoveY() );
    Models::g()->setData( Models::g()->index( (int)Fn::Global::BBX, 0 ), bbx );
    Models::g()->setData( Models::g()->index( (int)Fn::Global::BBY, 0 ), bby );
}

void SceneRenderer::draw()
{
    if ( !skipDraw )
    {
        renderScene();

        //***************************************************************************************************
        //
        // Pass 6 - merge previous results and render on quad
        //
        //***************************************************************************************************/

        renderMerge();
    }
    else
    {
        skipDraw = false;
    }
}

void SceneRenderer::renderScene()
{
    GLFunctions::getAndPrintGLError( "before render scene");
    QColor bgColor = Models::g()->data( Models::g()->index( (int)Fn::Global::BACKGROUND_COLOR_MAIN, 0 ) ).value<QColor>();

    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glClearDepth( 1 );
    //glDisable( GL_BLEND );

    GLuint tex = getTexture( "D0" );
    glActiveTexture( GL_TEXTURE9 );
    glBindTexture( GL_TEXTURE_2D, tex );

    tex = getTexture( "D1" );
    glActiveTexture( GL_TEXTURE10 );
    glBindTexture( GL_TEXTURE_2D, tex );

    tex = getTexture( "D2" );
    glActiveTexture( GL_TEXTURE11 );
    glBindTexture( GL_TEXTURE_2D, tex );

    tex = getTexture( "PICK" );
    glActiveTexture( GL_TEXTURE12 );
    glBindTexture( GL_TEXTURE_2D, tex );


    //***************************************************************************************************
    //
    // Pass 1 - draw opaque objects
    //
    //***************************************************************************************************/
    glClearColor( bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    clearTexture( "D0", 0.0, 0.0, 0.0, 0.0 );
    clearTexture( "PICK", 0.0, 0.0, 0.0, 0.0 );
    clearTexture( "C0", bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 1.0 );
    setRenderTargets( "C0", "D0" );
    renderScenePart( 1, "C0", "D0" );

    //***************************************************************************************************
    //
    // Pass 2
    //
    //***************************************************************************************************/
    clearTexture( "C1", bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 0.0 );
    clearTexture( "D1", bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 0.0 );

    renderScenePart( 2, "C1", "D1" );

    //***************************************************************************************************
    //
    // Pass 3
    //
    //***************************************************************************************************/
    clearTexture( "C2", bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 0.0 );
    clearTexture( "D2", bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 0.0 );

    renderScenePart( 3, "C2", "D2" );

    //***************************************************************************************************
    //
    // Pass 4
    //
    //***************************************************************************************************/

    clearTexture( "C3", bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 0.0 );
    clearTexture( "D1", bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 0.0 );


    renderScenePart( 4, "C3", "D1" );

    //***************************************************************************************************
    //
    // Pass 5
    //
    //***************************************************************************************************/
    m_renderMode = 3;
    clearTexture( "D2", bgColor.redF(), bgColor.greenF(), bgColor.blueF(), 0.0 );
    setRenderTarget( "D2" );

    m_sliceRenderer->draw( m_pMatrix, m_mvMatrix, m_width, m_height, m_renderMode, m_renderTarget );
    renderDatasets();
    renderRois();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void SceneRenderer::renderScenePart( int renderMode, QString target0, QString target1 )
{
    m_renderMode = renderMode;
    setRenderTargets( target0, target1 );

    m_sliceRenderer->draw( m_pMatrix, m_mvMatrix, m_width, m_height, m_renderMode, m_renderTarget );
    renderDatasets();
    renderRois();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void SceneRenderer::renderMerge()
{
    m_renderMode = 0;

    // Tell OpenGL which VBOs to use
    glBindBuffer( GL_ARRAY_BUFFER, vboIds[ 0 ] );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboIds[ 1 ] );

    GLuint tex = getTexture( "C0" );
    glActiveTexture( GL_TEXTURE5 );
    glBindTexture( GL_TEXTURE_2D, tex );

    tex = getTexture( "C1" );
    glActiveTexture( GL_TEXTURE6 );
    glBindTexture( GL_TEXTURE_2D, tex );

    tex = getTexture( "C2" );
    glActiveTexture( GL_TEXTURE7 );
    glBindTexture( GL_TEXTURE_2D, tex );

    tex = getTexture( "C3" );
    glActiveTexture( GL_TEXTURE8 );
    glBindTexture( GL_TEXTURE_2D, tex );

    tex = getTexture( "D0" );
    glActiveTexture( GL_TEXTURE9 );
    glBindTexture( GL_TEXTURE_2D, tex );

    tex = getTexture( "D2" );
    glActiveTexture( GL_TEXTURE11 );
    glBindTexture( GL_TEXTURE_2D, tex );

    QGLShaderProgram* program = GLFunctions::getShader( "merge" );
    program->bind();
    // Offset for position
    intptr_t offset = 0;

    // Tell OpenGL programmable pipeline how to locate vertex position data
    int vertexLocation = program->attributeLocation( "a_position" );
    program->enableAttributeArray( vertexLocation );
    glVertexAttribPointer( vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *) offset );

    // Offset for texture coordinate
    offset += sizeof(QVector3D);

    // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
    int texcoordLocation = program->attributeLocation( "a_texcoord" );
    program->enableAttributeArray( texcoordLocation );
    glVertexAttribPointer( texcoordLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *) offset );

    program->setUniformValue( "C0", 5 );
    program->setUniformValue( "C1", 6 );
    program->setUniformValue( "C2", 7 );
    program->setUniformValue( "C3", 8 );
    program->setUniformValue( "D0", 9 );
    program->setUniformValue( "D1", 10 );
    program->setUniformValue( "D2", 11 );

    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    GLFunctions::getAndPrintGLError( "after render scene");
}

QImage* SceneRenderer::screenshot()
{
    int size = Models::g()->data( Models::g()->index( (int)Fn::Global::SCREENSHOT_QUALITY, 0 ) ).toInt();
    int tmpWidth = m_width;
    int tmpHeight = m_height;
    resizeGL( size, size );
    renderScene();
    setRenderTarget( "SCREENSHOT" );
    renderMerge();

    QImage* out = getOffscreenTexture( m_width, m_height );

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    resizeGL( tmpWidth, tmpHeight );

    return out;

}

void SceneRenderer::renderDatasets()
{
    int countDatasets = Models::d()->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
        ds->draw( m_pMatrix, m_mvMatrix, m_width, m_height, m_renderMode, m_renderTarget );
    }
}

void SceneRenderer::renderRois()
{
   int countTopBoxes = Models::r()->rowCount();
   for ( int i = 0; i < countTopBoxes; ++i )
   {
       ROI* roi = VPtr<ROI>::asPtr( Models::r()->data( Models::r()->index( i, (int)Fn::ROI::POINTER ), Qt::DisplayRole ) );
       if ( roi->properties()->get( Fn::ROI::ACTIVE ).toBool() )
       {
           roi->draw( m_pMatrix, m_mvMatrix, m_width, m_height, m_renderMode );

           QModelIndex mi = Models::r()->index( i, 0 );
           int countBoxes = Models::r()->rowCount(  mi );

           for ( int k = 0; k < countBoxes; ++k )
           {
               roi = VPtr<ROI>::asPtr( Models::r()->data( Models::r()->index( k, (int)Fn::ROI::POINTER, mi ), Qt::DisplayRole ) );
               if ( roi->properties()->get( Fn::ROI::ACTIVE ).toBool() )
               {
                   roi->draw( m_pMatrix, m_mvMatrix, m_width, m_height, m_renderMode );
               }
           }
       }
   }
}

void SceneRenderer::setView( Fn::Orient view )
{
    m_arcBall->setView( (int)view );
    int countDatasets = Models::d()->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = Models::d()->index( i, (int)Fn::Property::ACTIVE );
        if ( Models::d()->data( index, Qt::DisplayRole ).toBool() )
        {
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
            ds->properties()->set( Fn::Property::RENDER_SLICE, (int)view );
        }
    }
}

void SceneRenderer::leftMouseDown( int x, int y )
{
    m_arcBall->click( x, y );
}

void SceneRenderer::leftMouseDrag( int x, int y )
{
    m_arcBall->drag( x, y );
    calcMVPMatrix();
}

void SceneRenderer::middleMouseDown( int x, int y )
{
    m_arcBall->midClick( x, y );
    calcMVPMatrix();
}

void SceneRenderer::middleMouseDrag( int x, int y )
{
    m_arcBall->midDrag( x, y );
    calcMVPMatrix();
}

void SceneRenderer::mouseWheel( int step )
{
    m_arcBall->mouseWheel( step );
    calcMVPMatrix();
}

void SceneRenderer::renderPick()
{
    // render
    m_renderMode = 0;
    setRenderTarget( "C0" );

    glEnable( GL_DEPTH_TEST );
    /* clear the frame buffer */
    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //render_picking_scene();
    m_sliceRenderer->draw( m_pMatrix, m_mvMatrix, m_width, m_height, m_renderMode, m_renderTarget );
    renderRois();
    renderDatasets();
}

void SceneRenderer::rightMouseDown( int x, int y )
{
    m_rightMouseDown = QVector2D( x, y );
    // get id
    m_picked = get_object_id( x, y, m_width, m_height );

    renderPick();
    QVector3D pickPos = mapMouse2World( x, y );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    //qDebug() << "picked object id:" << m_picked << "at position:" << pickPos;

    int countDatasets = Models::d()->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = Models::d()->index( i, (int)Fn::Property::ACTIVE );
        if ( Models::d()->data( index, Qt::DisplayRole ).toBool() )
        {
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
            ds->mousePick( m_picked, pickPos );
        }
    }

    if ( Models::r()->rowCount() > 0 )
    {
        QModelIndex mi = Models::r()->index( 0, (int)Fn::ROI::PICK_ID );
        QModelIndexList l = ( Models::r()->match( mi, Qt::DisplayRole, m_picked ) );
        if ( l.size() > 0 )
        {
            m_roiSelectionModel->clear();
            m_roiSelectionModel->select( l.first(), QItemSelectionModel::Select );
        }
    }

    m_sliceXPosAtPick = Models::g()->data( Models::g()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toInt();
    m_sliceYPosAtPick = Models::g()->data( Models::g()->index( (int)Fn::Global::CORONAL, 0 ) ).toInt();
    m_sliceZPosAtPick = Models::g()->data( Models::g()->index( (int)Fn::Global::AXIAL, 0 ) ).toInt();

    m_pickOld = QVector2D( x, y );
    m_rightMouseDown = QVector2D( x, y );
}

void SceneRenderer::rightMouseDrag( int x, int y )
{
    QVector3D vs = mapMouse2World( x, y, 0 );
    QVector3D vs2 = mapMouse2World( m_pickOld.x(), m_pickOld.y(), 0 );
    QVector3D dir = vs - vs2;

    m_pickOld = QVector2D( x, y );

    int m_x = Models::g()->data( Models::g()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toInt();
    int m_y = Models::g()->data( Models::g()->index( (int)Fn::Global::CORONAL, 0 ) ).toInt();
    int m_z = Models::g()->data( Models::g()->index( (int)Fn::Global::AXIAL, 0 ) ).toInt();
    float slowDown = 4.0f * m_arcBall->getZoom();

    renderPick();
    QVector3D pickPos = mapMouse2World( x, y );
    /* return to the default frame buffer */
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    int countDatasets = Models::d()->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = Models::d()->index( i, (int)Fn::Property::ACTIVE );
        if ( Models::d()->data( index, Qt::DisplayRole ).toBool() )
        {
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int)Fn::Property::DATASET_POINTER ), Qt::DisplayRole ) );
            ds->mousePick( m_picked, pickPos );
        }
    }


    switch ( m_picked )
    {
        case 0:
            break;
        case 1:
        {
            QVector2D v1 = mapWorld2Mouse( m_nx / 2, m_ny / 2, m_z );
            QVector2D v2 = mapWorld2Mouse( m_nx / 2, m_ny / 2, m_z + 1.0 );
            QVector2D v3 = v1 - v2;
            float distX = ( m_rightMouseDown.x() - x ) * v3.x() / m_width;
            float distY = ( m_rightMouseDown.y() - y ) * v3.y() / m_height;
            int newSlice = m_sliceZPosAtPick + distX * m_nz / slowDown - distY * m_nz / slowDown;
            if ( m_z != newSlice )
            {
                Models::g()->setData( Models::g()->index( (int)Fn::Global::AXIAL, 0 ), newSlice );
                skipDraw = true;
                Models::g()->submit();
            }
            break;
        }
        case 2:
        {
            QVector2D v1 = mapWorld2Mouse( m_nx / 2, m_y, m_nz / 2 );
            QVector2D v2 = mapWorld2Mouse( m_nx / 2, m_y + 1.0, m_nz / 2 );
            QVector2D v3 = v1 - v2;
            float distX = ( m_rightMouseDown.x() - x ) * v3.x() / m_width;
            float distY = ( m_rightMouseDown.y() - y ) * v3.y() / m_height;
            int newSlice = m_sliceYPosAtPick + distX * m_ny / slowDown - distY * m_ny / slowDown;
            if ( m_y != newSlice )
            {
                Models::g()->setData( Models::g()->index( (int)Fn::Global::CORONAL, 0 ), newSlice );
                skipDraw = true;
                Models::g()->submit();
            }
            break;
        }
        case 3:
        {
            QVector2D v1 = mapWorld2Mouse( m_x, m_ny / 2, m_nz / 2 );
            QVector2D v2 = mapWorld2Mouse( m_x + 1.0, m_ny / 2, m_nz / 2 );
            QVector2D v3 = v1 - v2;
            float distX = ( m_rightMouseDown.x() - x ) * v3.x() / m_width;
            float distY = ( m_rightMouseDown.y() - y ) * v3.y() / m_height;
            int newSlice = m_sliceXPosAtPick + distX * m_nx / slowDown - distY * m_nx / slowDown;
            if ( m_x != newSlice )
            {
                Models::g()->setData( Models::g()->index( (int)Fn::Global::SAGITTAL, 0 ), newSlice );
                skipDraw = true;
                Models::g()->submit();
            }
            break;
        }
        default:
        {
            if ( m_roiSelectionModel->hasSelection() )
            {
                QModelIndex mi = m_roiSelectionModel->selectedIndexes().first();
                ROI* roi = VPtr<ROI>::asPtr( Models::r()->data( Models::r()->index( mi.row(), (int)Fn::ROI::POINTER, mi.parent() ), Qt::DisplayRole ) );
                float newx = roi->properties()->get( Fn::ROI::X ).toFloat() + dir.x();
                float newy = roi->properties()->get( Fn::ROI::Y ).toFloat() + dir.y();
                float newz = roi->properties()->get( Fn::ROI::Z ).toFloat() + dir.z();

                roi->properties()->set( Fn::ROI::X, newx );
                roi->properties()->set( Fn::ROI::Y, newy );
                roi->properties()->set( Fn::ROI::Z, newz );
                roi->properties()->slotPropChanged();
            }
            break;
        }
    }
}

QVector3D SceneRenderer::mapMouse2World( int x, int y, int z )
{
    GLint viewport[4];
    GLfloat winX, winY;

    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = (float) x;
    winY = (float) viewport[3] - (float) y;
    GLdouble posX, posY, posZ;
    gluUnProject( winX, winY, z, m_mvMatrix.data(), m_pMatrix.data(), viewport, &posX, &posY, &posZ );

    QVector3D v( posX, posY, posZ );
    return v;
}

QVector2D SceneRenderer::mapWorld2Mouse( float x, float y, float z )
{
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    GLdouble winX, winY, winZ;
    gluProject( x, y, z, m_mvMatrix.data(), m_pMatrix.data(), viewport, &winX, &winY, &winZ );
    return QVector2D( winX, winY );
}

QVector3D SceneRenderer::mapMouse2World( float x, float y )
{
    GLint viewport[4];
    GLfloat winX, winY;

    glGetIntegerv( GL_VIEWPORT, viewport );

    winX = (float) x;
    winY = (float) viewport[3] - (float) y;

    GLfloat z;
    glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, (void*)&z);

    GLdouble posX, posY, posZ;
    gluUnProject( winX, winY, z, m_mvMatrix.data(), m_pMatrix.data(), viewport, &posX, &posY, &posZ );

    QVector3D v( posX, posY, posZ );
    return v;
}

void SceneRenderer::generate_pixel_buffer_objects( int width, int height )
{
    /* generate the first pixel buffer objects */
    glGenBuffers( 1, &pbo_a );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, pbo_a );
    glBufferData( GL_PIXEL_PACK_BUFFER, width * height * 4, NULL, GL_STREAM_READ );
    /* to avoid weird behaviour the first frame the data is loaded */
    glReadPixels( 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* generate the second pixel buffer objects */
    glGenBuffers( 1, &pbo_b );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, pbo_b );
    glBufferData( GL_PIXEL_PACK_BUFFER, width * height * 4, NULL, GL_STREAM_READ );
    glReadPixels( 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* unbind */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
}

GLuint SceneRenderer::get_object_id( int x, int y, int width, int height )
{
    glBindFramebuffer( GL_FRAMEBUFFER, FBO );
    glReadBuffer( GL_COLOR_ATTACHMENT2 );

    static int frame_event = 0;
    int read_pbo, map_pbo;
    uint object_id;

    uint red, green, blue, pixel_index;
    GLubyte* ptr;

    generate_pixel_buffer_objects( width, height );
    /* switch between pixel buffer objects */
    if (frame_event == 0)
    {
        frame_event = 1;
        read_pbo = pbo_a;
        map_pbo = pbo_b;
    }
    else {
        frame_event = 0;
        read_pbo = pbo_b;
        map_pbo = pbo_a;

    }
    /* read one pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, read_pbo ) ;
    glReadPixels( 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* map the other pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, map_pbo );
    ptr = (GLubyte*)glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_WRITE );
    /* get the mouse coordinates */
    /* OpenGL has the {0,0} at the down-left corner of the screen */
    y = height - y;
    object_id = -1;

    if ( x >= 0 && x < width && y >= 0 && y < height )
    {
        pixel_index = ( x + y * width ) * 4;
        blue = ptr[pixel_index];
        green = ptr[pixel_index + 1];
        red = ptr[pixel_index + 2];
        //alpha = ptr[pixel_index + 3];
        //object_id = alpha + ( red << 24 ) + ( green << 16 ) + ( blue << 8 );
        object_id = ( red << 16 ) + ( green << 8 ) + ( blue );
        //qDebug() << "output" << red << green << blue << alpha;
    }
    glUnmapBuffer( GL_PIXEL_PACK_BUFFER );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );

    glDeleteBuffers( 1, &pbo_a );
    glDeleteBuffers( 1, &pbo_b );

    return object_id;
}

QImage* SceneRenderer::getOffscreenTexture( int width, int height )
{
    static int frame_event = 0;
    int read_pbo, map_pbo;

    uint red, green, blue, alpha, pixel_index;
    GLubyte* ptr;

    generate_pixel_buffer_objects( width, height );
    /* switch between pixel buffer objects */
    if (frame_event == 0)
    {
        frame_event = 1;
        read_pbo = pbo_a;
        map_pbo = pbo_a;
    }
    else {
        frame_event = 0;
        map_pbo = pbo_a;
        read_pbo = pbo_b;
    }
    /* read one pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, read_pbo ) ;
    glReadPixels( 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0 );
    /* map the other pixel buffer */
    glBindBuffer( GL_PIXEL_PACK_BUFFER, map_pbo );
    ptr = (GLubyte*)glMapBuffer( GL_PIXEL_PACK_BUFFER, GL_READ_WRITE );
    /* get the mouse coordinates */
    /* OpenGL has the {0,0} at the down-left corner of the screen */

    QImage* image = new QImage( width, height, QImage::Format_RGB32 );
    QColor c;
    for ( int x = 0; x < width; ++x )
    {
        for ( int y = 0; y < height; ++y )
        {
            pixel_index = ( x + y * width ) * 4;
            blue = ptr[pixel_index];
            green = ptr[pixel_index + 1];
            red = ptr[pixel_index + 2];
            alpha = ptr[pixel_index + 3];

            c = QColor( red, green, blue, alpha );
            image->setPixel( x, ( height - y )-1, c.rgba() );
        }
    }

    glUnmapBuffer( GL_PIXEL_PACK_BUFFER );
    glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );

    glDeleteBuffers( 1, &pbo_a );
    glDeleteBuffers( 1, &pbo_b );

    return image;
}

void SceneRenderer::initFBO( int width, int height )
{
    if ( textures.size() > 0 )
    {
        foreach ( GLuint tex, textures )
        {
            glDeleteTextures( 1, &tex );
        }
        glDeleteFramebuffers( 1, &FBO );
        glDeleteRenderbuffers( 1, &RBO );

    }

    textures[ "C0" ] = createTexture( width, height );
    textures[ "C1" ] = createTexture( width, height );
    textures[ "C2" ] = createTexture( width, height );
    textures[ "C3" ] = createTexture( width, height );
    textures[ "D0" ] = createTexture( width, height );
    textures[ "D1" ] = createTexture( width, height );
    textures[ "D2" ] = createTexture( width, height );
    textures[ "PICK" ] = createTexture( width, height );
    textures[ "SCREENSHOT" ] = createTexture( width, height );

    /* create a framebuffer object */
    glGenFramebuffers( 1, &FBO );
    /* attach the texture and the render buffer to the frame buffer */
    glBindFramebuffer( GL_FRAMEBUFFER, FBO );

    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers( 3,  attachments );

    /* create a renderbuffer object for the depth buffer */
    glGenRenderbuffers( 1, &RBO );
    /* bind the texture */
    glBindRenderbuffer( GL_RENDERBUFFER, RBO );
    /* create the render buffer in the GPU */
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height );

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures["C0"], 0 );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO );
    /* check the frame buffer */
    if ( glCheckFramebufferStatus( GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE )
    {
        /* handle an error : frame buffer incomplete */
        qDebug() << "frame buffer incomplete";
    }

    /* unbind the render buffer */
    glBindRenderbuffer( GL_RENDERBUFFER, 0 );
    /* return to the default frame buffer */
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

GLuint SceneRenderer::createTexture( int width, int height )
{
    /* generate a texture id */
    GLuint ptex;
    glGenTextures( 1, &ptex );
    /* bind the texture */
    glBindTexture( GL_TEXTURE_2D, ptex );
    /* set texture parameters */
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    /* create the texture in the GPU */
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    /* unbind the texture */
    glBindTexture( GL_TEXTURE_2D, 0 );

    return ptex;
}

void SceneRenderer::setRenderTarget( QString target )
{
    glBindFramebuffer( GL_FRAMEBUFFER, FBO );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[target], 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textures["PICK"], 0 );
}

void SceneRenderer::setRenderTargets( QString target0, QString target1 )
{
    glBindFramebuffer( GL_FRAMEBUFFER, FBO );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[target0], 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textures[target1], 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, textures["PICK"], 0 );
}

void SceneRenderer::clearTexture( QString target, float r, float g, float b, float a )
{
    glBindFramebuffer( GL_FRAMEBUFFER, FBO );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[target], 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0 );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0 );
    glClearColor( r, g, b, a );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

GLuint SceneRenderer::getTexture( QString name )
{
    return textures[name];
}
