/*
 * glwidget.h
 *
 * Created on: May 03, 2012
 * @author Ralph Schurade
 */

#include "glwidget.h"
#include "../gl/arcball.h"
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
    m_height( 0 )
{
    m_arcBall = new ArcBall( 400, 400 );
    m_sceneRenderer = new SceneRenderer( name );

    m_pMatrix.setToIdentity();
    m_mvMatrix.setToIdentity();

    connect( Models::d(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::r(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
    connect( Models::g(), SIGNAL( dataChanged( QModelIndex, QModelIndex ) ), this, SLOT( update() ) );
}

GLWidget::GLWidget( QString name, QItemSelectionModel* roiSelectionModel, QWidget *parent, const QGLWidget *shareWidget ) :
    QGLWidget( parent, shareWidget ),
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
    m_height( 0 )
{
    m_arcBall = new ArcBall( 400, 400 );
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

void GLWidget::initializeGL()
{
    m_sceneRenderer->initGL();
    GLFunctions::initTextRenderer();
    GLFunctions::initShapeRenderer();
}

void GLWidget::paintGL()
{
    if ( !skipDraw )
    {
        m_sceneRenderer->draw( m_mvMatrix, m_pMatrix );
    }
    else
    {
        skipDraw = false;
    }
}

void GLWidget::resizeGL( int width, int height )
{
    m_arcBall->set_win_size( width, height );
    m_sceneRenderer->resizeGL( width, height );
    m_width = width;
    m_height = height;

    calcMVPMatrix();
}

void GLWidget::calcMVPMatrix()
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

    int boundingbox = qMax ( m_nx, qMax( m_ny, m_nz ) );

    // Reset projection
    m_pMatrix.setToIdentity();

    float halfBB = boundingbox / 2.0;

    float bbx = boundingbox;
    float bby = boundingbox;

    float ratio = static_cast<float>( m_width )/ static_cast<float>( m_height );
    if ( ratio >= 1.0 )
    {
        m_pMatrix.ortho( -halfBB * ratio, halfBB * ratio, -halfBB, halfBB, -3000, 3000 );
        bbx = boundingbox * ratio;
    }
    else
    {
        m_pMatrix.ortho( -halfBB, halfBB, -halfBB / ratio, halfBB / ratio, -3000, 3000 );
        bby = boundingbox / ratio;
    }

    m_mvMatrix = m_arcBall->getMVMat();

    Models::g()->setData( Models::g()->index( (int)Fn::Global::ZOOM, 0 ), m_arcBall->getZoom() );
    Models::g()->setData( Models::g()->index( (int)Fn::Global::MOVEX, 0 ), m_arcBall->getMoveX() );
    Models::g()->setData( Models::g()->index( (int)Fn::Global::MOVEY, 0 ), m_arcBall->getMoveY() );
    Models::g()->setData( Models::g()->index( (int)Fn::Global::BBX, 0 ), bbx );
    Models::g()->setData( Models::g()->index( (int)Fn::Global::BBY, 0 ), bby );
}


void GLWidget::mousePressEvent( QMouseEvent *event )
{
    setFocus();
    if ( event->buttons() & Qt::LeftButton )
    {
        m_arcBall->click( event->x(), event->y() );

    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_arcBall->midClick( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::RightButton )
    {
        rightMouseDown( event->x(), event->y() );
    }
    calcMVPMatrix();
    updateGL();
}

void GLWidget::mouseMoveEvent( QMouseEvent *event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        m_arcBall->drag( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::MiddleButton )
    {
        m_arcBall->midDrag( event->x(), event->y() );
    }
    if ( event->buttons() & Qt::RightButton )
    {
        rightMouseDrag( event->x(), event->y() );
    }
    calcMVPMatrix();
    updateGL();
}

void GLWidget::mouseReleaseEvent( QMouseEvent *event )
{
    updateGL();
}

void GLWidget::enterEvent( QEvent *event )
{
    updateGL();
}

void GLWidget::wheelEvent( QWheelEvent *event )
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    m_arcBall->mouseWheel( numSteps );
    calcMVPMatrix();
    updateGL();
}

void GLWidget::update()
{
    if ( !m_visible )
    {
        return;
    }
    calcMVPMatrix();
    updateGL();
}

void GLWidget::setView( Fn::Orient view )
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
    update();
}

QImage* GLWidget::screenshot()
{
    return m_sceneRenderer->screenshot();
}

void GLWidget::rightMouseDown( int x, int y )
{
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
        m_roiSelectionModel->clear();
        if ( l.size() > 0 )
        {
            m_roiSelectionModel->select( l.first(), QItemSelectionModel::Select );
        }
    }

    m_sliceXPosAtPick = Models::g()->data( Models::g()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toInt();
    m_sliceYPosAtPick = Models::g()->data( Models::g()->index( (int)Fn::Global::CORONAL, 0 ) ).toInt();
    m_sliceZPosAtPick = Models::g()->data( Models::g()->index( (int)Fn::Global::AXIAL, 0 ) ).toInt();

    m_pickOld = QVector2D( x, y );
    m_rightMouseDown = QVector2D( x, y );
}

void GLWidget::rightMouseDrag( int x, int y )
{
    QVector3D vs = m_sceneRenderer->mapMouse2World( x, y, 0 );
    QVector3D vs2 = m_sceneRenderer->mapMouse2World( m_pickOld.x(), m_pickOld.y(), 0 );
    QVector3D dir = vs - vs2;

    m_pickOld = QVector2D( x, y );

    int m_x = Models::g()->data( Models::g()->index( (int)Fn::Global::SAGITTAL, 0 ) ).toInt();
    int m_y = Models::g()->data( Models::g()->index( (int)Fn::Global::CORONAL, 0 ) ).toInt();
    int m_z = Models::g()->data( Models::g()->index( (int)Fn::Global::AXIAL, 0 ) ).toInt();
    float slowDown = 4.0f * m_arcBall->getZoom();

    m_sceneRenderer->renderPick();
    QVector3D pickPos = m_sceneRenderer->mapMouse2World( x, y );
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
            QVector2D v1 = m_sceneRenderer->mapWorld2Mouse( m_nx / 2, m_ny / 2, m_z );
            QVector2D v2 = m_sceneRenderer->mapWorld2Mouse( m_nx / 2, m_ny / 2, m_z + 1.0 );
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
            QVector2D v1 = m_sceneRenderer->mapWorld2Mouse( m_nx / 2, m_y, m_nz / 2 );
            QVector2D v2 = m_sceneRenderer->mapWorld2Mouse( m_nx / 2, m_y + 1.0, m_nz / 2 );
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
            QVector2D v1 = m_sceneRenderer->mapWorld2Mouse( m_x, m_ny / 2, m_nz / 2 );
            QVector2D v2 = m_sceneRenderer->mapWorld2Mouse( m_x + 1.0, m_ny / 2, m_nz / 2 );
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

void GLWidget::keyPressEvent( QKeyEvent* event )
{
    int x = this->size().width() / 2;
    int y = this->size().height() / 2;
    switch( event->key() )
    {
        case Qt::Key_Left :
        {
            m_arcBall->click( x, y );
            m_arcBall->drag( x - 20, y );
            break;
        }
        case Qt::Key_Right:
        {
            m_arcBall->click( x, y );
            m_arcBall->drag( x + 20, y );
            break;
        }
        case Qt::Key_Up :
        {
            m_arcBall->click( x, y );
            m_arcBall->drag( x, y - 20 );
            break;
        }
        case Qt::Key_Down:
        {
            m_arcBall->click( x, y );
            m_arcBall->drag( x, y + 20 );
            break;
        }
    }
    calcMVPMatrix();
    updateGL();
}

void GLWidget::visibilityChanged( bool visible )
{
    m_visible = visible;
}
