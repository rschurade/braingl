/*
 * glwidget.h
 *
 * Created on: May 03, 2012
 * @author Ralph Schurade
 */

#include "glwidget.h"
#include "../gl/arcball.h"
#include "../gl/camera.h"
#include "../gl/camerabase.h"
#include "../gl/glfunctions.h"

#include "../../data/datastore.h"
#include "../../data/enums.h"
#include "../../data/models.h"
#include "../../data/datasets/dataset.h"
#include "../../data/vptr.h"
#include "../../data/roi.h"

#include <QtGui>

GLWidget::GLWidget( QString name, QItemSelectionModel* roiSelectionModel, QWidget *parent ) :
    QGLWidget( QGLFormat( QGL::SampleBuffers ), parent ),
    m_name( name ),
    m_roiSelectionModel( roiSelectionModel ),
    m_visible( false ),
    m_nx( 160 ),
    m_ny( 200 ),
    m_nz( 160 ),
    m_picked( 0 ),
    m_sliceXPosAtPick( 0 ),
    m_sliceYPosAtPick( 0 ),
    m_sliceZPosAtPick( 0 ),
    skipDraw( false ),
    m_width( 0 ),
    m_height( 0 ),
    m_doScreenshot( false )
{
    m_arcBall = new ArcBall( 400, 400 );
    m_camera = new Camera( 400, 400 );
    m_cameraInUse = m_arcBall;
    m_sceneRenderer = new SceneRenderer( name );

    m_pMatrix.setToIdentity();
    m_mvMatrix.setToIdentity();

    connect( Models::d(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::r(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
}

GLWidget::GLWidget( QString name, QItemSelectionModel* roiSelectionModel, QWidget *parent, const QGLWidget *shareWidget ) :
    QGLWidget( parent, shareWidget ),
    m_name( name ),
    m_roiSelectionModel( roiSelectionModel ),
    m_visible( false ),
    m_nx( 160 ),
    m_ny( 200 ),
    m_nz( 160 ),
    m_picked( 0 ),
    m_sliceXPosAtPick( 0 ),
    m_sliceYPosAtPick( 0 ),
    m_sliceZPosAtPick( 0 ),
    skipDraw( false ),
    m_width( 0 ),
    m_height( 0 ),
    m_doScreenshot( false )
{
    m_arcBall = new ArcBall( 400, 400 );
    m_camera = new Camera( 400, 400 );
    m_cameraInUse = m_arcBall;
    m_sceneRenderer = new SceneRenderer( name );

    m_pMatrix.setToIdentity();
    m_mvMatrix.setToIdentity();

    connect( Models::d(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::r(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize( 50, 50 );
}

QSize GLWidget::sizeHint() const
{
    return QSize( 400, 400 );
}

CameraBase* GLWidget::getCameraInUse()
{
    return m_cameraInUse;
}

ArcBall* GLWidget::getArcBall()
{
    return m_arcBall;
}

Camera* GLWidget::getCamera()
{
    return m_camera;
}

void GLWidget::initializeGL()
{
    m_sceneRenderer->initGL();
    GLFunctions::initTextRenderer();
    GLFunctions::initShapeRenderer();
}

void GLWidget::paintGL()
{
    if ( m_doScreenshot )
    {
        if( Models::getGlobal( Fn::Property::G_SCREENSHOT_STEREOSCOPIC ).toBool() )
        {
            // insert code for stereoscopic screenshots here
        }
        else
        {
            calcMVPMatrix();
            m_doScreenshot = false;
            QImage* image = m_sceneRenderer->screenshot( m_mvMatrix, m_pMatrix );
            image->save(  m_screenshotFileName, "PNG" );
            delete image;
            calcMVPMatrix();
        }
    }

    if ( !skipDraw )
    {
        if ( m_visible )
        {
            m_sceneRenderer->draw( m_mvMatrix, m_pMatrix );
        }
    }
    else
    {
        skipDraw = false;
    }
}

void GLWidget::resizeGL( int width, int height )
{
    m_cameraInUse->set_win_size( width, height );
    m_sceneRenderer->resizeGL( width, height );
    m_width = width;
    m_height = height;

    if ( m_name == "maingl" )
    {
        Models::setGlobal( Fn::Property::G_WIDTH_MAINGL, width );
        Models::setGlobal( Fn::Property::G_HEIGHT_MAINGL, height );
    }
    else if( m_name == "maingl2" )
    {
        Models::setGlobal( Fn::Property::G_WIDTH_MAINGL2, width );
        Models::setGlobal( Fn::Property::G_HEIGHT_MAINGL2, height );
    }

    update();
}

void GLWidget::update()
{
    //glViewport( 0, 0, m_width, m_height );
    calcMVPMatrix();
    updateGL();
}

void GLWidget::calcMVPMatrix()
{
    int cam = Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_TYPE, 0 ) ).toInt();
    int projection = Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_PROJECTION, 0 ) ).toInt();
    if (  cam == 0 )
    {
        m_cameraInUse = m_arcBall;
    }
    else
    {
        m_cameraInUse = m_camera;
    }

    m_nx = Models::g()->data( Models::g()->index( (int)Fn::Property::G_MAX_SAGITTAL, 0 ) ).toFloat();
    m_ny = Models::g()->data( Models::g()->index( (int)Fn::Property::G_MAX_CORONAL, 0 ) ).toFloat();
    m_nz = Models::g()->data( Models::g()->index( (int)Fn::Property::G_MAX_AXIAL, 0 ) ).toFloat();
    float dx = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SLICE_DX, 0 ) ).toFloat();
    float dy = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SLICE_DY, 0 ) ).toFloat();
    float dz = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SLICE_DZ, 0 ) ).toFloat();
    m_nx *= dx;
    m_ny *= dy;
    m_nz *= dz;

    m_arcBall->setRotCenter( m_nx / 2., m_ny / 2., m_nz / 2. );

    int boundingbox = qMax ( m_nx, qMax( m_ny, m_nz ) );

    // Reset projection
    m_pMatrix.setToIdentity();

    float zoom  = m_cameraInUse->getZoom();
    float halfBB = ( boundingbox / 2 ) / zoom;

    float ratio = 1.0f;
    if ( m_doScreenshot )
    {
        float screenshotWidth = Models::getGlobal( Fn::Property::G_SCREENSHOT_WIDTH ).toFloat();
        float screenshotHeight = Models::getGlobal( Fn::Property::G_SCREENSHOT_HEIGHT ).toFloat();
        ratio = screenshotWidth / screenshotHeight;
    }
    else
    {
        ratio = static_cast<float>( m_width ) / static_cast<float>( m_height );
    }

    if ( projection == 0 ) // ortho
    {
        if ( ratio >= 1.0 )
        {
            m_pMatrix.ortho( -halfBB * ratio, halfBB * ratio, -halfBB, halfBB, -3000, 3000 );
        }
        else
        {
            m_pMatrix.ortho( -halfBB, halfBB, -halfBB / ratio, halfBB / ratio, -3000, 3000 );
        }
    }
    else // perspective
    {
        float nearPlane = Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_NEAR, 0 ) ).toFloat();
        float farPlane = Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_FAR, 0 ) ).toFloat();
        float angle = Models::g()->data( Models::g()->index( (int)Fn::Property::G_CAMERA_ANGLE, 0 ) ).toFloat();
        angle /= zoom;

        if ( m_doScreenshot && Models::getGlobal( Fn::Property::G_SCREENSHOT_STEREOSCOPIC ).toBool() )
        {
            //m_pMatrix.frustum( something )
        }
        else
        {
            m_pMatrix.perspective( angle, ratio, nearPlane, farPlane );
        }
    }
    m_mvMatrix = m_cameraInUse->getMVMat();

    if ( dynamic_cast<ArcBall*>( m_cameraInUse ) )
    {
        QVector3D pos;
        QVector3D view;
        QVector3D up;
        getCameraParametersFromModelviewMatrix( pos, view, up );
        QList<QVariant>l;
        l.push_back( pos );
        //l.push_back( pos + view * 10 );
        l.push_back( -m_arcBall->getRotCenter() );
        l.push_back( up );
        m_camera->setState( l );
    }

    if ( dynamic_cast<Camera*>( m_cameraInUse ) )
    {
        QMatrix4x4 rot = m_camera->getMVMat();
        m_arcBall->setRotation( m_arcBall->mat2quat( rot ) );
    }

    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_ZOOM, 0 ), zoom );
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_MOVEX, 0 ), m_cameraInUse->getMoveX() );
    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_MOVEY, 0 ), m_cameraInUse->getMoveY() );
}


void GLWidget::mousePressEvent( QMouseEvent *event )
{
    setFocus();
    if ( event->buttons() & Qt::LeftButton )
    {
        m_cameraInUse->click( event->x(), event->y() );

    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_cameraInUse->midClick( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::RightButton )
    {
        rightMouseDown( event );
    }
    update();
}

void GLWidget::mouseMoveEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        m_cameraInUse->drag( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_cameraInUse->midDrag( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::RightButton )
    {
        rightMouseDrag( event );
    }
    update();
}

void GLWidget::mouseReleaseEvent( QMouseEvent *event )
{
    update();
}

void GLWidget::enterEvent( QEvent *event )
{
    update();
}

void GLWidget::wheelEvent( QWheelEvent *event )
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    m_cameraInUse->mouseWheel( numSteps );
    update();
}

void GLWidget::setView( Fn::Orient view )
{
    m_cameraInUse->setView( view );
    update();
}

void GLWidget::screenshot( QString fn )
{
    m_doScreenshot = true;
    m_screenshotFileName = fn;
}

void GLWidget::rightMouseDown( QMouseEvent* event )
{

    //qDebug() << "Object name:" << m_sceneRenderer->getRenderTarget();

    QString target = m_sceneRenderer->getRenderTarget();

    int x = event->x();
    int y = event->y();
    m_rightMouseDown = QVector2D( x, y );
    // get id
    m_picked = m_sceneRenderer->get_object_id( x, y, m_width, m_height );

    m_sceneRenderer->renderPick();
    QVector3D pickPos = m_sceneRenderer->mapMouse2World( x, y );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    //qDebug() << "picked object id:" << m_picked << "at position:" << pickPos;

    int countDatasets = Models::d()->rowCount();
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = Models::d()->index( i, (int)Fn::Property::D_ACTIVE );
        if ( Models::d()->data( index, Qt::DisplayRole ).toBool() )
        {
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
            ds->mousePick( m_picked, pickPos, event->modifiers(), target );
        }
    }

    if ( Models::r()->rowCount() > 0 )
    {
        QModelIndex mi = Models::r()->index( 0, (int)Fn::Property::R_PICK_ID );
        QModelIndexList l = ( Models::r()->match( mi, Qt::DisplayRole, m_picked ) );
        m_roiSelectionModel->clear();
        if ( l.size() > 0 )
        {
            m_roiSelectionModel->select( l.first(), QItemSelectionModel::Select );
        }
    }

    m_sliceXPosAtPick = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SAGITTAL, 0 ) ).toInt();
    m_sliceYPosAtPick = Models::g()->data( Models::g()->index( (int)Fn::Property::G_CORONAL, 0 ) ).toInt();
    m_sliceZPosAtPick = Models::g()->data( Models::g()->index( (int)Fn::Property::G_AXIAL, 0 ) ).toInt();

    m_pickOld = QVector2D( x, y );
    m_rightMouseDown = QVector2D( x, y );
    skipDraw = true;
    Models::g()->submit();
}

void GLWidget::rightMouseDrag( QMouseEvent* event )
{
    QString target = m_sceneRenderer->getRenderTarget();

    int x = event->x();
    int y = event->y();

    QVector3D vs = m_sceneRenderer->mapMouse2World( x, y, 0 );
    QVector3D vs2 = m_sceneRenderer->mapMouse2World( m_pickOld.x(), m_pickOld.y(), 0 );
    QVector3D dir = vs - vs2;

    m_pickOld = QVector2D( x, y );

    int m_x = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SAGITTAL, 0 ) ).toInt();
    int m_y = Models::g()->data( Models::g()->index( (int)Fn::Property::G_CORONAL, 0 ) ).toInt();
    int m_z = Models::g()->data( Models::g()->index( (int)Fn::Property::G_AXIAL, 0 ) ).toInt();
    float slowDown = 4.0f * m_cameraInUse->getZoom();

    m_sceneRenderer->renderPick();
    QVector3D pickPos = m_sceneRenderer->mapMouse2World( x, y );
    /* return to the default frame buffer */
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    int countDatasets = Models::d()->rowCount();
    bool updateRequired = false;
    for ( int i = 0; i < countDatasets; ++i )
    {
        QModelIndex index = Models::d()->index( i, (int)Fn::Property::D_ACTIVE );
        if ( Models::d()->data( index, Qt::DisplayRole ).toBool() )
        {
            Dataset* ds = VPtr<Dataset>::asPtr( Models::d()->data( Models::d()->index( i, (int)Fn::Property::D_DATASET_POINTER ), Qt::DisplayRole ) );
            updateRequired |= ds->mousePick( m_picked, pickPos, event->modifiers(), target );
        }
    }

    if ( updateRequired )
    {
        skipDraw = true;
        Models::g()->submit();
    }

    if ( !(event->modifiers() & Qt::ControlModifier) )
    {
        switch ( m_picked )
        {
            case 0:
                break;
            case 1:
            {
                QVector2D v1 = m_sceneRenderer->mapWorld2Mouse( m_nx / 2, m_ny / 2, m_z );
                QVector2D v2 = m_sceneRenderer->mapWorld2Mouse( m_nx / 2, m_ny / 2, m_z + 1.0 );
                QVector2D v3 = v1 - v2;
                float distX = ( m_rightMouseDown.x() - x ) * v3.x() / m_width;
                float distY = ( m_rightMouseDown.y() - y ) * v3.y() / m_height;
                int newSlice = m_sliceZPosAtPick + distX * m_nz / slowDown - distY * m_nz / slowDown;
                if ( m_z != newSlice )
                {
                    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_AXIAL, 0 ), newSlice );
                    skipDraw = true;
                    Models::g()->submit();
                }
                break;
            }
            case 2:
            {
                QVector2D v1 = m_sceneRenderer->mapWorld2Mouse( m_nx / 2, m_y, m_nz / 2 );
                QVector2D v2 = m_sceneRenderer->mapWorld2Mouse( m_nx / 2, m_y + 1.0, m_nz / 2 );
                QVector2D v3 = v1 - v2;
                float distX = ( m_rightMouseDown.x() - x ) * v3.x() / m_width;
                float distY = ( m_rightMouseDown.y() - y ) * v3.y() / m_height;
                int newSlice = m_sliceYPosAtPick + distX * m_ny / slowDown - distY * m_ny / slowDown;
                if ( m_y != newSlice )
                {
                    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CORONAL, 0 ), newSlice );
                    skipDraw = true;
                    Models::g()->submit();
                }
                break;
            }
            case 3:
            {
                QVector2D v1 = m_sceneRenderer->mapWorld2Mouse( m_x, m_ny / 2, m_nz / 2 );
                QVector2D v2 = m_sceneRenderer->mapWorld2Mouse( m_x + 1.0, m_ny / 2, m_nz / 2 );
                QVector2D v3 = v1 - v2;
                float distX = ( m_rightMouseDown.x() - x ) * v3.x() / m_width;
                float distY = ( m_rightMouseDown.y() - y ) * v3.y() / m_height;
                int newSlice = m_sliceXPosAtPick + distX * m_nx / slowDown - distY * m_nx / slowDown;
                if ( m_x != newSlice )
                {
                    Models::g()->setData( Models::g()->index( (int)Fn::Property::G_SAGITTAL, 0 ), newSlice );
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
                    ROI* roi = VPtr<ROI>::asPtr( Models::r()->data( Models::r()->index( mi.row(), (int)Fn::Property::R_POINTER, mi.parent() ), Qt::DisplayRole ) );
                    float newx = roi->properties()->get( Fn::Property::R_X ).toFloat() + dir.x();
                    float newy = roi->properties()->get( Fn::Property::R_Y ).toFloat() + dir.y();
                    float newz = roi->properties()->get( Fn::Property::R_Z ).toFloat() + dir.z();

                    roi->properties()->set( Fn::Property::R_X, newx );
                    roi->properties()->set( Fn::Property::R_Y, newy );
                    roi->properties()->set( Fn::Property::R_Z, newz );
                    roi->slotPropChanged();
                }
                break;
            }
        }
    }
}

void GLWidget::keyPressEvent( QKeyEvent* event )
{
    int x = this->size().width() / 2;
    int y = this->size().height() / 2;

    if ( event->modifiers() & Qt::ShiftModifier )
    {
        int m_x = Models::g()->data( Models::g()->index( (int)Fn::Property::G_SAGITTAL, 0 ) ).toInt();
        int m_y = Models::g()->data( Models::g()->index( (int)Fn::Property::G_CORONAL, 0 ) ).toInt();
        int m_z = Models::g()->data( Models::g()->index( (int)Fn::Property::G_AXIAL, 0 ) ).toInt();

        int m_nx = Models::g()->data( Models::g()->index( (int)Fn::Property::G_MAX_SAGITTAL, 0 ) ).toInt();
        int m_ny = Models::g()->data( Models::g()->index( (int)Fn::Property::G_MAX_CORONAL, 0 ) ).toInt();
        int m_nz = Models::g()->data( Models::g()->index( (int)Fn::Property::G_MAX_AXIAL, 0 ) ).toInt();
        switch( event->key() )
        {
            case Qt::Key_Left :
            {
                m_x = qMax( 0, m_x -1 );
                break;
            }
            case Qt::Key_Right:
            {
                m_x = qMin( m_nx, m_x +1 );
                break;
            }
            case Qt::Key_Down :
            {
                m_y = qMax( 0, m_y -1 );
                break;
            }
            case Qt::Key_Up:
            {
                m_y = qMin( m_ny, m_y +1 );
                break;
            }
            case Qt::Key_PageDown:
            {
                m_z = qMax( 0, m_z -1 );
                break;
            }
            case Qt::Key_PageUp:
            {
                m_z = qMin( m_nz, m_z +1 );
                break;
            }
        }
        Models::g()->setData( Models::g()->index( (int)Fn::Property::G_SAGITTAL, 0 ), m_x );
        Models::g()->setData( Models::g()->index( (int)Fn::Property::G_CORONAL, 0 ), m_y );
        Models::g()->setData( Models::g()->index( (int)Fn::Property::G_AXIAL, 0 ), m_z );

        Models::g()->submit();
    }
    else
    {
        if ( dynamic_cast<Camera*>( m_cameraInUse) )
        {
            switch( event->key() )
            {
                case Qt::Key_Left :
                    m_camera->viewLeft();
                    break;
                case Qt::Key_Right:
                    m_camera->viewRight();
                    break;
                case Qt::Key_Up :
                    m_camera->viewUp();
                    break;
                case Qt::Key_Down:
                    m_camera->viewDown();
                    break;
                case 69:
                    m_camera->strafeLeft();
                    break;
                case 81:
                    m_camera->strafeRight();
                    break;
                case 87:
                    m_camera->up();
                    break;
                case 83:
                    m_camera->down();
                    break;
                case 68:
                    m_camera->left();
                    break;
                case 65:
                    m_camera->right();
                    break;
                case 82:
                    m_camera->forward();
                    break;
                case 70:
                    m_camera->backward();
                    break;
            }
            QList<QVariant> state = m_camera->getState();
            emit signalCameraChanged();
        }
        else
        {
            switch( event->key() )
            {
                case Qt::Key_Left :
                {
                    m_cameraInUse->click( x, y );
                    m_cameraInUse->drag( x - 20, y );
                    break;
                }
                case Qt::Key_Right:
                {
                    m_cameraInUse->click( x, y );
                    m_cameraInUse->drag( x + 20, y );
                    break;
                }
                case Qt::Key_Up :
                {
                    m_cameraInUse->click( x, y );
                    m_cameraInUse->drag( x, y - 20 );
                    break;
                }
                case Qt::Key_Down:
                {
                    m_cameraInUse->click( x, y );
                    m_cameraInUse->drag( x, y + 20 );
                    break;
                }
            }
        }
    }
    update();

    emit signalKeyPressed( event->key(), event->modifiers() );
}

void GLWidget::visibilityChanged( bool visible )
{
    m_visible = visible;
}

/// get camera parameters from
/// the OpenGL modelview matrix
///
/// \param[out] eyepos  eye position
/// \param[out] viewdir viewing direction
/// \param[out] updir   up direction vector
void GLWidget::getCameraParametersFromModelviewMatrix( QVector3D &eyepos,  QVector3D &viewdir, QVector3D &updir )
{
    QVector3D xdir( 0.0, 0.0, 0.0 );
    QVector3D ydir( 0.0, 0.0, 0.0 );
    QVector3D zdir( 0.0, 0.0, 0.0 );

    xdir.setX( m_mvMatrix( 0, 0 ) );
    ydir.setX( m_mvMatrix( 1, 0 ) );
    zdir.setX( m_mvMatrix( 2, 0 ) );
    xdir.setY( m_mvMatrix( 0, 1 ) );
    ydir.setY( m_mvMatrix( 1, 1 ) );
    zdir.setY( m_mvMatrix( 2, 1 ) );
    xdir.setZ( m_mvMatrix( 0, 2 ) );
    ydir.setZ( m_mvMatrix( 1, 2 ) );
    zdir.setZ( m_mvMatrix( 2, 2 ) );

    // This is a, b, c components.
    QVector3D bvec( -m_mvMatrix( 0, 3 ), -m_mvMatrix( 1, 3 ), -m_mvMatrix( 2, 3 ) );

    QMatrix4x4 basis_mat( xdir.x(), xdir.y(), xdir.z(), 0, ydir.x(), ydir.y(), ydir.z(), 0, zdir.x(), zdir.y(), zdir.z(), 0, 0, 0, 0, 1 );
    // This matrix should not be singular.
    // invert() gives matrix inverse.
    basis_mat = basis_mat.inverted();

    eyepos = basis_mat * bvec;
    viewdir = -zdir;
    updir = ydir;
    //qDebug() << eyepos << viewdir << updir;
}
