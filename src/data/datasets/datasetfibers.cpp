/*
 * datasetfibers.cpp
 *
 * Created on: Dec 12, 2012
 * @author Ralph Schurade
 */
#include "datasetfibers.h"

#include "fiberselector.h"
#include "../models.h"

#include "../properties/propertybool.h"
#include "../properties/propertycolor.h"
#include "../properties/propertyint.h"
#include "../properties/propertyfloat.h"
#include "../properties/propertypath.h"
#include "../properties/propertystring.h"
#include "../properties/propertyselection.h"

#include "../loadervtk.h"
#include "../../gui/gl/fiberrenderer.h"
#include "../../gui/gl/tuberenderer.h"

#include <QTimer>


DatasetFibers::DatasetFibers( QDir filename, Fn::DatasetType type ) :
    Dataset( filename, type ),
    m_renderer( 0 ),
    m_tubeRenderer( 0 ),
    m_selector( 0 ),
    m_numPoints( 0 ),
    m_numLines( 0 ),
    m_morphValue( 1.0f)
{

}

DatasetFibers::DatasetFibers( QDir filename, QVector< QVector< float > > fibs ) :
    Dataset( filename, Fn::DatasetType::FIBERS ),
    m_fibs( fibs ),
    m_renderer( 0 ),
    m_tubeRenderer( 0 ),
    m_selector( 0 ),
    m_numPoints( 0 ),
    m_numLines( 0 ),
    m_morphValue( 1.0f)
{
    QVector<QVector<float> >data0;
    QVector<float>min0;
    QVector<float>max0;
    data0.reserve( fibs.size() );
    for ( int i = 0; i < fibs.size(); ++i )
    {
        data0.push_back( QVector<float>( fibs[i].size() / 3 ) );
    }
    m_data.push_back( data0 );
    m_dataNames.push_back( "no data" );

    createProps();
    m_customColors.resize( m_numLines );
}

DatasetFibers::DatasetFibers( QDir filename,
                                 QVector< QVector< float > > fibs,
                                 QVector< QVector< QVector< float > > >data,
                                 QVector<QString>dataNames,
                                 QVector< float > mins,
                                 QVector<float> maxes ) :
    Dataset( filename, Fn::DatasetType::FIBERS ),
    m_fibs( fibs ),
    m_data( data ),
    m_dataNames( dataNames ),
    m_dataMins( mins ),
    m_dataMaxes( maxes ),
    m_renderer( 0 ),
    m_tubeRenderer( 0 ),
    m_selector( 0 ),
    m_numPoints( 0 ),
    m_numLines( 0 ),
    m_morphValue( 1.0f)
{
    createProps();
}

DatasetFibers::DatasetFibers( QDir filename, LoaderVTK* lv ) :
    Dataset( filename, Fn::DatasetType::FIBERS ),
    m_renderer( 0 ),
    m_tubeRenderer( 0 ),
    m_selector( 0 ),
    m_numPoints( 0 ),
    m_numLines( 0 ),
    m_morphValue( 1.0f)
{
    copyFromLoader( lv );

    createProps();
}

DatasetFibers::~DatasetFibers()
{
    m_properties["maingl"]->set( Fn::Property::D_ACTIVE, false );
    m_properties["maingl2"]->set( Fn::Property::D_ACTIVE, false );
    m_fibs.clear();
    m_fibs.squeeze();
    m_orig_fibs.clear();
    m_orig_fibs.squeeze();
    m_straight_fibs.clear();
    m_straight_fibs.squeeze();
    if ( m_renderer != 0 )
    {
        delete m_renderer;
        m_renderer = 0;
    }
    if ( m_tubeRenderer != 0 )
    {
        delete m_tubeRenderer;
        m_tubeRenderer = 0;
    }
}

void DatasetFibers::createProps()
{
    m_numLines = m_fibs.size();
    for ( int i = 0; i < m_fibs.size(); ++i )
    {
        m_numPoints += m_fibs[i].size() / 3;
    }
    bool hasData =(  m_dataNames[0] != "no data" );
    qDebug() << "num points:" << m_numPoints << "num lines:" << m_fibs.size();

    m_properties["maingl"]->createInt( Fn::Property::D_NUM_POINTS, m_numPoints );
    m_properties["maingl"]->createInt( Fn::Property::D_NUM_LINES, m_fibs.size() );
    m_properties["maingl"]->createList( Fn::Property::D_FIBER_RENDERMODE, {"lines", "tubes"}, 0, "general" );
    if ( hasData )
    {
        m_properties["maingl"]->createList( Fn::Property::D_COLORMODE, { "global", "local", "user defined", "mri", "data" }, 0, "general" );
        m_properties["maingl"]->createList( Fn::Property::D_DATAMODE, m_dataNames, 0, "general" );
    }
    else
    {
        m_properties["maingl"]->createList( Fn::Property::D_COLORMODE, { "global", "local", "user defined", "mri" }, 0, "general" );
        m_properties["maingl"]->createList( Fn::Property::D_DATAMODE, m_dataNames, 0 );
    }
    m_properties["maingl"]->createColor( Fn::Property::D_COLOR, QColor( 255, 0, 0 ), "color" );
    m_properties["maingl"]->createFloat( Fn::Property::D_ALPHA, 1.f, 0.01f, 1.f, "color" );
    m_properties["maingl"]->createFloat( Fn::Property::D_FIBER_THICKNESS, 1.0f, 0.1f, 5.0f, "general" );
    m_properties["maingl"]->createFloat( Fn::Property::D_MIN, 0.0f );
    m_properties["maingl"]->createFloat( Fn::Property::D_MAX, 1.0f );

    if ( hasData )
    {
        m_properties["maingl"]->createInt( Fn::Property::D_COLORMAP, 1, "general" );
        m_properties["maingl"]->createFloat( Fn::Property::D_SELECTED_MIN, 0.0f, 0.0f, 1.0f, "color" );
        m_properties["maingl"]->createFloat( Fn::Property::D_SELECTED_MAX, 1.0f, 0.0f, 1.0f, "color" );
        m_properties["maingl"]->createFloat( Fn::Property::D_LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f, "color" );
        m_properties["maingl"]->createFloat( Fn::Property::D_UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f, "color" );
    }
    else
    {
        m_properties["maingl"]->createInt( Fn::Property::D_COLORMAP, 1 );
        m_properties["maingl"]->createFloat( Fn::Property::D_SELECTED_MIN, 0.0f, 0.0f, 1.0f );
        m_properties["maingl"]->createFloat( Fn::Property::D_SELECTED_MAX, 1.0f, 0.0f, 1.0f );
        m_properties["maingl"]->createFloat( Fn::Property::D_LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f );
        m_properties["maingl"]->createFloat( Fn::Property::D_UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f );
    }
    m_properties["maingl"]->createFloat( Fn::Property::D_DX, 200.0f, 0.0f, 200.0f, "special" );
    m_properties["maingl"]->createFloat( Fn::Property::D_DY, 200.0f, 0.0f, 200.0f, "special" );
    m_properties["maingl"]->createFloat( Fn::Property::D_DZ, 200.0f, 0.0f, 200.0f, "special" );
    m_properties["maingl"]->createInt( Fn::Property::D_NX, 800, 0, 1600, "special" );
    m_properties["maingl"]->createInt( Fn::Property::D_NY, 1000, 0, 2000, "special" );
    m_properties["maingl"]->createInt( Fn::Property::D_NZ, 800, 0, 1600, "special" );

    m_properties["maingl"]->createFloat( Fn::Property::D_FIBER_MORPH, 1.0f, 0.0f, 1.0f, "special" );

    m_properties["maingl"]->createBool( Fn::Property::D_LIGHT_SWITCH, true, "light" );
    m_properties["maingl"]->createFloat( Fn::Property::D_LIGHT_AMBIENT,   0.2f, 0.0f, 1.0f, "light" );
    m_properties["maingl"]->createFloat( Fn::Property::D_LIGHT_DIFFUSE,   0.45f, 0.0f, 1.0f, "light" );
    m_properties["maingl"]->createFloat( Fn::Property::D_LIGHT_SPECULAR,  0.5f, 0.0f, 1.0f, "light" );
    m_properties["maingl"]->createFloat( Fn::Property::D_MATERIAL_AMBIENT,   2.0f, 0.0f, 10.0f, "light" );
    m_properties["maingl"]->createFloat( Fn::Property::D_MATERIAL_DIFFUSE,   0.8f, 0.0f, 10.0f, "light" );
    m_properties["maingl"]->createFloat( Fn::Property::D_MATERIAL_SPECULAR,  0.61f, 0.0f, 10.0f, "light" );
    m_properties["maingl"]->createFloat( Fn::Property::D_MATERIAL_SHININESS, 2.0f, 0.0f, 200.0f, "light" );

    int maxLength = 0;
    for ( int i = 0; i < m_fibs.size(); ++i )
    {
        maxLength = qMax( maxLength, m_fibs[i].size() / 3 );
    }

    m_properties["maingl"]->createFloat( Fn::Property::D_FIBER_GROW_LENGTH, (float)maxLength, 0.0f, (float)maxLength, "special" );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_COLOR ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( colorChanged() ) );

    PropertyGroup* props2 = new PropertyGroup( *( m_properties["maingl"] ) );
    m_properties.insert( "maingl2", props2 );

    if ( hasData )
    {
        connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
                      m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
        connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
                  m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );

        connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
                  m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SLOT( setMin( QVariant ) ) );
        connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
                  m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SLOT( setMax( QVariant ) ) );

        connect( m_properties["maingl"]->getProperty( Fn::Property::D_DATAMODE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( dataModeChanged() ) );

        connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
                      m_properties["maingl2"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
        connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
                  m_properties["maingl2"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );

        connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
                  m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SLOT( setMin( QVariant ) ) );
        connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
                  m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SLOT( setMax( QVariant ) ) );

        //connect( m_properties["maingl2"]->getProperty( Fn::Property::D_DATAMODE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( dataModeChanged() ) );
    }

    m_properties["maingl"]->createList( Fn::Property::D_USE_TRANSFORM, { "user defined", "qform", "sform", "qform inverted", "sform inverted" }, 3, "transform" );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_USE_TRANSFORM ), SIGNAL( valueChanged( QVariant ) ), this,
                SLOT( transformChanged( QVariant ) ) );
    m_properties["maingl"]->createMatrix( Fn::Property::D_TRANSFORM, m_transform, "transform" );
    m_properties["maingl"]->createButton( Fn::Property::D_APPLY_TRANSFORM, "transform" );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_APPLY_TRANSFORM ), SIGNAL( valueChanged( QVariant ) ), this,
                SLOT( applyTransform() ) );

    m_properties["maingl"]->createBool( Fn::Property::D_AUTOPLAY, false );
    m_properties["maingl"]->createInt( Fn::Property::D_AUTOPLAY_INTERVAL, 25, 10, 1000 );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_AUTOPLAY ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( autoplay() ) );

    transformChanged( 3 );
}

QVector< QVector< float > > DatasetFibers::getFibs()
{
    return m_fibs;
}

QVector< QVector< QVector< float > > > DatasetFibers::getData()
{
    return m_data;
}

QVector< QVector< float > > DatasetFibers::getData( int id )
{
    return m_data[id];
}

QVector< QVector< float > > DatasetFibers::getSelectedFibs()
{
    if ( m_renderer == 0 )
    {
        return m_fibs;
    }
    else
    {
        QVector<bool>*selected = m_selector->getSelection();
        QVector<QVector<float> >out;
        int first = 0;
        for ( int i = 0; i < selected->size(); ++i )
        {
            if ( selected->at( i ) )
            {
                first = i;
                break;
            }
        }
        QVector<float>f0;
        f0 = m_fibs[first];
        QVector3D start;
        if ( Models::r()->rowCount()  > 0 )
        {
            int shape = Models::r()->data( m_selector->createIndex( 0, 0, (int)Fn::Property::R_SHAPE ), Qt::DisplayRole ).toInt();
            if ( shape >= 0 && shape <= 3 )
            {
                float x = Models::r()->data( m_selector->createIndex( 0, 0, (int)Fn::Property::R_X ), Qt::DisplayRole ).toFloat();
                float y = Models::r()->data( m_selector->createIndex( 0, 0, (int)Fn::Property::R_Y ), Qt::DisplayRole ).toFloat();
                float z = Models::r()->data( m_selector->createIndex( 0, 0, (int)Fn::Property::R_Z ), Qt::DisplayRole ).toFloat();
                start = QVector3D( x, y, z );
            }
            else
            {
                start = QVector3D( f0[0], f0[1], f0[2] );
            }
        }
        else
        {
            start = QVector3D( f0[0], f0[1], f0[2] );
        }


        for ( int i = 0; i < selected->size(); ++i )
        {
            if ( selected->at( i ) )
            {
                QVector<float>f = m_fibs[i];
                QVector3D s1( f[0], f[1], f[2] );
                int size = f.size();
                QVector3D s2( f[size - 3], f[size - 2], f[size - 1] );

                if ( ( start - s1 ).length() < ( start - s2 ).length() )
                {
                    out.push_back( m_fibs[i] );
                }
                else
                {
                    // flip fiber
                    QVector<float>f = m_fibs[i];
                    QVector<float>newfib;
                    for ( int k = 0; k < f.size() / 3; ++k )
                    {
                        newfib.push_front( f[k*3+2] );
                        newfib.push_front( f[k*3+1] );
                        newfib.push_front( f[k*3] );
                    }
                    out.push_back( newfib );
                }
            }
        }
        return out;
    }
}

QVector< QVector< QVector< float > > > DatasetFibers::getSelectedData()
{
    if ( m_renderer == 0 )
    {
        return m_data;
    }
    else
    {
        QVector<bool>*selected = m_selector->getSelection();
        QVector< QVector<QVector<float> > >out;

        for ( int k = 0; k < m_data.size(); ++k )
        {
            QVector<QVector<float> > data = m_data[k];
            QVector<QVector<float> > dataOut;
            for ( int i = 0; i < selected->size(); ++i )
            {
                if ( selected->at( i ) )
                {
                    dataOut.push_back( data[i] );
                }
            }
            out.push_back( dataOut );
        }
        return out;
    }
}

QVector<float> DatasetFibers::getDataMins()
{
    return m_dataMins;
}

QVector<float> DatasetFibers::getDataMaxes()
{
    return m_dataMaxes;
}

QVector<QString> DatasetFibers::getDataNames()
{
    return m_dataNames;
}

void DatasetFibers::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }
    float morphValue = properties( target )->get( Fn::Property::D_FIBER_MORPH ).toFloat();
    if ( morphValue != m_morphValue )
    {
        if ( !m_morphable )
        {
            makeMorphable();
        }
        qDebug() << "morphing fibers to: " << morphValue;
        morph( morphValue );
    }
    if ( m_selector == 0 )
    {
        m_selector = new FiberSelector( m_numPoints );
        m_selector->init( m_fibs );
        connect( m_selector, SIGNAL( changed() ), Models::d(), SLOT( submit() ) );
    }

    if ( properties( target )->get( Fn::Property::D_FIBER_RENDERMODE).toInt() == 0 )
    {
        if ( m_renderer == 0 )
        {
            m_renderer = new FiberRenderer( m_selector, &m_fibs, &m_customColors, &( m_data[properties( target )->get( Fn::Property::D_DATAMODE).toInt()] ), m_numPoints );
            m_renderer->setModel( Models::g() );
            m_renderer->init();
            //m_renderer->colorChanged( properties( target )->get( Fn::Property::D_COLOR ).value<QColor>() );
            connect( properties( target )->getProperty( Fn::Property::D_COLOR ), SIGNAL( valueChanged( QVariant ) ), m_renderer, SLOT( colorChanged( QVariant ) ) );
        }

        m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }
    else if ( properties( target )->get( Fn::Property::D_FIBER_RENDERMODE).toInt() == 1 )
    {
        if ( m_tubeRenderer == 0 )
        {
            m_tubeRenderer = new TubeRenderer( m_selector, &m_fibs, &(m_data[properties( target )->get( Fn::Property::D_DATAMODE).toInt()] ) );
            m_tubeRenderer->setModel( Models::g() );
            m_tubeRenderer->init();
            m_tubeRenderer->colorChanged( properties( target )->get( Fn::Property::D_COLOR ).value<QColor>() );
            connect( properties( target )->getProperty( Fn::Property::D_COLOR ), SIGNAL( valueChanged( QVariant ) ), m_tubeRenderer, SLOT( colorChanged( QVariant ) ) );
        }

        m_tubeRenderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }
}

QString DatasetFibers::getValueAsString( int x, int y, int z )
{
    return QString( "" );
}

void DatasetFibers::colorChanged()
{
    m_properties["maingl"]->set( Fn::Property::D_COLORMODE, 2 );
}

void DatasetFibers::dataModeChanged()
{
    if ( m_renderer != 0 )
    {
        m_renderer->updateExtraData( &( m_data[properties( "maingl" )->get( Fn::Property::D_DATAMODE).toInt()] ) );
    }
    if ( m_tubeRenderer != 0 )
    {
        m_tubeRenderer->updateExtraData( &( m_data[properties( "maingl" )->get( Fn::Property::D_DATAMODE).toInt()] ) );
    }
    float min = m_dataMins[ m_properties["maingl"]->get( Fn::Property::D_DATAMODE).toInt()];
    float max = m_dataMaxes[ m_properties["maingl"]->get( Fn::Property::D_DATAMODE).toInt()];

    m_properties["maingl"]->set( Fn::Property::D_MIN, min );
    m_properties["maingl"]->set( Fn::Property::D_MAX, max );
    m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN )->setMin( min );
    m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN )->setMax( max );
    m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX )->setMin( min );
    m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX )->setMax( max );
    m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN )->setValue( min );
    m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX )->setValue( max );
    m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD )->setValue( min );
    m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD )->setValue( max );
    Models::d()->submit();
}

void DatasetFibers::copyFromLoader( LoaderVTK* lv )
{
    QVector<float> points = lv->getPoints();
    QVector<int> lines = lv->getLines();
    m_numLines = lv->getNumLines();

    qDebug() << "points size:" << points.size() << "lines size:" << lines.size() << "num lines:" << m_numLines;

    int lc = 0;
    int pc = 0;
    for ( int i = 0; i < m_numLines; ++i )
    {
        QVector<float> fib;
        int lineSize = lines[lc];

        for ( int k = 0; k < lineSize * 3; ++k )
        {
            fib.push_back( points[pc++] );
        }
        lc += lineSize + 1;
        m_fibs.push_back( fib );
    }

    QVector<unsigned char> colors = lv->getPrimitiveColors();
    m_customColors.resize( m_numLines );
    if ( colors.size() == m_numLines * 3 )
    {
        for ( int i = 0; i < m_numLines; ++i )
        {
            m_customColors[i] = QColor( ( (float)colors[i * 3] ),
                                        ( (float)colors[i * 3 + 1] ),
                                        ( (float)colors[i * 3 + 2] ), 255 );
        }
    }

    QVector<QVector<float> > pointData = lv->getPointData();
    m_dataNames = lv->getPointDataNames();

    if ( pointData.size() > 0 )
    {
        qDebug() << pointData.size() << "point data fields found";
        for ( int curField = 0; curField < pointData.size(); ++curField )
        {
            QVector<float> field = pointData[curField];
            float min = std::numeric_limits<float>::max();
            float max = std::numeric_limits<float>::min();

            for ( int i = 0; i < field.size(); ++i )
            {
                float value = field[i];
                min = qMin( min, value );
                max = qMax( max, value );
            }
            m_dataMins.push_back( min );
            m_dataMaxes.push_back( max );
        }
        for ( int curField = 0; curField < pointData.size(); ++curField )
        {
            QVector<float> field = pointData[curField];

            QVector<QVector<float> > dataField;

            int lc = 0;
            int pc = 0;
            for ( int i = 0; i < m_numLines; ++i )
            {
                QVector<float> fib;
                int lineSize = lines[lc];
                for ( int k = 0; k < lineSize; ++k )
                {
                    fib.push_back( field[pc++] );
                }
                lc += lineSize + 1;
                dataField.push_back( fib );
            }

            m_data.push_back( dataField );
        }
    }
    else
    {
        QVector<QVector<float> >data0;
        QVector<float>min0;
        QVector<float>max0;
        data0.reserve( m_fibs.size() );
        for ( int i = 0; i < m_fibs.size(); ++i )
        {
            data0.push_back( QVector<float>( m_fibs[i].size() / 3 ) );
        }
        m_data.push_back( data0 );
        m_dataNames.push_back( "no data" );
    }
}

void DatasetFibers::autoplay()
{
    if ( m_properties["maingl"]->get( Fn::Property::D_AUTOPLAY ).toBool() )
    {
        int dim = 200;
        int currentframe = m_properties["maingl"]->get( Fn::Property::D_FIBER_GROW_LENGTH ).toInt();
        ++currentframe;
        if ( currentframe >= dim )
        {
            currentframe = 0;
        }
        m_properties["maingl"]->set( Fn::Property::D_FIBER_GROW_LENGTH, currentframe );

        int interval = m_properties["maingl"]->get( Fn::Property::D_AUTOPLAY_INTERVAL ).toInt();
        QTimer::singleShot( interval, this, SLOT( autoplay() ) );
        //Models::d()->submit();
    }
}

QString DatasetFibers::getSaveFilter()
{
    return QString( "fib files binary(*.fib *.vtk );;fib files ascii (*.fib *.vtk);;all files (*.*)" );
}

QString DatasetFibers::getDefaultSuffix()
{
    return QString( "fib" );
}

void DatasetFibers::transformChanged( QVariant value )
{
    QMatrix4x4 qForm;
    QMatrix4x4 sForm;

    QList<Dataset*>dsl = Models::getDatasets( Fn::DatasetType::NIFTI_ANY );

    if ( dsl.size() > 0 )
    {
        qForm = dsl.first()->properties()->get( Fn::Property::D_Q_FORM ).value<QMatrix4x4>();
        sForm = dsl.first()->properties()->get( Fn::Property::D_S_FORM ).value<QMatrix4x4>();
    }

    m_properties["maingl"]->getWidget( Fn::Property::D_TRANSFORM )->setEnabled( false );

    switch ( value.toInt() )
    {
        case 0:
            m_transform.setToIdentity();
            m_properties["maingl"]->getWidget( Fn::Property::D_TRANSFORM )->setEnabled( true );
            break;
        case 1:
            m_transform = qForm;
            break;
        case 2:
            m_transform = sForm;
            break;
        case 3:
            m_transform = qForm.inverted();
            break;
        case 4:
            m_transform = sForm.inverted();
            break;
        default:
            m_transform.setToIdentity();
            break;
    }

    m_properties["maingl"]->set( Fn::Property::D_TRANSFORM, m_transform );
}

void DatasetFibers::applyTransform()
{
    float dx = Models::getGlobal( Fn::Property::G_SLICE_DX ).toFloat();
    float dy = Models::getGlobal( Fn::Property::G_SLICE_DY ).toFloat();
    float dz = Models::getGlobal( Fn::Property::G_SLICE_DZ ).toFloat();

    int selectedMatrix = m_properties["maingl"]->get( Fn::Property::D_USE_TRANSFORM ).toInt();

    m_transform = m_properties["maingl"]->get( Fn::Property::D_TRANSFORM ).value<QMatrix4x4>();

    switch( selectedMatrix )
    {
        case 0:
        {
            for ( int i = 0; i < m_fibs.size(); ++i )
            {
                QVector<float>fib = m_fibs[i];
                for ( int k = 0; k < fib.size() / 3; ++k )
                {
                    QVector3D vert( fib[k*3], fib[k*3+1], fib[k*3+2] );
                    vert = m_transform * vert;
                    fib[k*3  ] = vert.x();
                    fib[k*3+1] = vert.y();
                    fib[k*3+2] = vert.z();
                }
                m_fibs.replace( i, fib );
            }
            break;
        }
        case 1:
        case 2:
        {
            for ( int i = 0; i < m_fibs.size(); ++i )
            {
                QVector<float>fib = m_fibs[i];
                for ( int k = 0; k < fib.size() / 3; ++k )
                {
                    QVector3D vert( fib[k*3], fib[k*3+1], fib[k*3+2] );
                    vert.setX( vert.x() / dx );
                    vert.setY( vert.y() / dy );
                    vert.setZ( vert.z() / dz );
                    vert = m_transform * vert;
                    fib[k*3  ] = vert.x();
                    fib[k*3+1] = vert.y();
                    fib[k*3+2] = vert.z();
                }
                m_fibs.replace( i, fib );
            }
            break;
        }
        case 3:
        case 4:
        {
            for ( int i = 0; i < m_fibs.size(); ++i )
            {
                QVector<float>fib = m_fibs[i];
                for ( int k = 0; k < fib.size() / 3; ++k )
                {
                    QVector3D vert( fib[k*3], fib[k*3+1], fib[k*3+2] );
                    vert = m_transform * vert;
                    fib[k*3  ] = vert.x() * dx;
                    fib[k*3+1] = vert.y() * dy;
                    fib[k*3+2] = vert.z() * dz;
                }
                m_fibs.replace( i, fib );
            }
            break;
        }
    }

    delete m_renderer;
    m_renderer = 0;
    delete m_tubeRenderer;
    m_tubeRenderer = 0;
    delete m_selector;
    m_selector = 0;
    Models::d()->submit();
}

void DatasetFibers::makeMorphable()
{
    //copy fibs to m_original_fibs
    //calculate straightened fibs
    //set morphable true

    m_orig_fibs.resize( m_fibs.size() );
    m_straight_fibs.resize( m_fibs.size() );
    for ( int i = 0; i < m_fibs.size(); ++i )
    {
        QVector<float> fib = m_fibs[i];
        m_orig_fibs.replace( i, fib );

        QVector<float> s_fib = m_fibs[i];
        int n = fib.size() / 3;

        QVector3D start( fib[0], fib[1], fib[2] );
        QVector3D end( fib[( n - 1 ) * 3], fib[( n - 1 ) * 3 + 1], fib[( n - 1 ) * 3 + 2] );
        for ( int k = 0; k < fib.size() / 3; ++k )
        {
            QVector3D vert = start + ( end - start ) * ( k / (float) n );
            s_fib[k * 3] = vert.x();
            s_fib[k * 3 + 1] = vert.y();
            s_fib[k * 3 + 2] = vert.z();
        }

        m_straight_fibs.replace( i, s_fib );
    }
    m_morphable = true;
}

void DatasetFibers::morph(float value)
{
    qDebug() << "starting morph";

    //replace fibs with interpolated fibs
    for ( int i = 0; i < m_fibs.size(); ++i )
    {
        QVector<float> fib = m_fibs[i];
        QVector<float> o_fib = m_orig_fibs[i];
        QVector<float> s_fib = m_straight_fibs[i];

        for ( int k = 0; k < o_fib.size() / 3; ++k )
        {
            QVector3D o_vert( o_fib[k * 3], o_fib[k * 3 + 1], o_fib[k * 3 + 2] );
            QVector3D s_vert( s_fib[k * 3], s_fib[k * 3 + 1], s_fib[k * 3 + 2] );
            //qDebug() << o_vert << " " << s_vert;
            QVector3D vert = o_vert * value + s_vert * ( 1 - value );
            fib[k * 3] = vert.x();
            fib[k * 3 + 1] = vert.y();
            fib[k * 3 + 2] = vert.z();
        }
        m_fibs.replace( i, fib );
    }

    m_morphValue = value;

    qDebug() << "midmorph";

    //make change known to the world?
    //no models-> submit, since called from draw routine
    //TODO: there might be nicer ways to do this...
    delete m_renderer;
    m_renderer = 0;
    delete m_tubeRenderer;
    m_tubeRenderer = 0;
    delete m_selector;
    m_selector = 0;
}

QColor DatasetFibers::getCustomColor( int fiber )
{
    return m_customColors[fiber];
}
