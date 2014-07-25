/*
 * datasetfibers.cpp
 *
 *  Created on: Jan 16, 2014
 *      Author: schurade
 */

#include "datasetfibers.h"
#include "fiberselector.h"

#include "../models.h"
#include "../loadervtk.h"

#include "../properties/propertybool.h"
#include "../properties/propertycolor.h"
#include "../properties/propertyint.h"
#include "../properties/propertyfloat.h"
#include "../properties/propertypath.h"
#include "../properties/propertystring.h"
#include "../properties/propertyselection.h"

#include "../../gui/gl/fiberrenderer.h"
#include "../../gui/gl/tuberenderer.h"

DatasetFibers::DatasetFibers( QDir filename, Fn::DatasetType type ) :
    Dataset( filename, type ),
    m_kdVerts( 0 ),
    m_renderer( 0 ),
    m_tubeRenderer( 0 ),
    m_selector( 0 ),
    m_numPoints( 0 ),
    m_numLines( 0 )
{
}

DatasetFibers::DatasetFibers( QDir filename, std::vector<Fib> fibs, QList<QString> dataNames ) :
    Dataset( filename, Fn::DatasetType::FIBERS ),
    m_fibs( fibs ),
    m_dataNames( dataNames ),
    m_kdVerts( 0 ),
    m_renderer( 0 ),
    m_tubeRenderer( 0 ),
    m_selector( 0 ),
    m_numPoints( 0 ),
    m_numLines( 0 )
{
    createProps();
}

DatasetFibers::DatasetFibers( QDir filename, LoaderVTK* lv ) :
    Dataset( filename, Fn::DatasetType::FIBERS ),
    m_kdVerts( 0 ),
    m_renderer( 0 ),
    m_tubeRenderer( 0 ),
    m_selector( 0 ),
    m_numPoints( 0 ),
    m_numLines( 0 )
{
    copyFromLoader( lv );
    createProps();
}

DatasetFibers::~DatasetFibers()
{
}

void DatasetFibers::createProps()
{
    m_numLines = m_fibs.size();
    for ( unsigned int i = 0; i < m_fibs.size(); ++i )
    {
        m_numPoints += m_fibs[i].length();
    }
    bool hasData =(  m_dataNames[0] != "no data" );
    qDebug() << "num points:" << m_numPoints << "num lines:" << m_fibs.size();

    m_properties["maingl"].createInt( Fn::Property::D_NUM_POINTS, m_numPoints );
    m_properties["maingl"].createInt( Fn::Property::D_NUM_LINES, m_fibs.size() );
    m_properties["maingl"].createList( Fn::Property::D_FIBER_RENDERMODE, {"lines", "tubes"}, 0, "general" );
    if ( hasData )
    {
        m_properties["maingl"].createList( Fn::Property::D_COLORMODE, { "global", "local", "user defined", "mri", "data" }, 0, "general" );
        m_properties["maingl"].createList( Fn::Property::D_DATAMODE, m_dataNames, 0, "general" );
    }
    else
    {
        m_properties["maingl"].createList( Fn::Property::D_COLORMODE, { "global", "local", "user defined", "mri" }, 0, "general" );
        m_properties["maingl"].createList( Fn::Property::D_DATAMODE, m_dataNames, 0 );
    }
    m_properties["maingl"].createColor( Fn::Property::D_COLOR, QColor( 255, 0, 0 ), "color" );
    m_properties["maingl"].createFloat( Fn::Property::D_ALPHA, 1.f, 0.01f, 1.f, "color" );
    m_properties["maingl"].createFloat( Fn::Property::D_FIBER_THICKNESS, 1.0f, 0.1f, 5.0f, "general" );
    m_properties["maingl"].createFloat( Fn::Property::D_MIN, 0.0f );
    m_properties["maingl"].createFloat( Fn::Property::D_MAX, 1.0f );

    if ( hasData )
    {
        m_properties["maingl"].createInt( Fn::Property::D_COLORMAP, 1, "general" );
        m_properties["maingl"].createFloat( Fn::Property::D_SELECTED_MIN, 0.0f, 0.0f, 1.0f, "color" );
        m_properties["maingl"].createFloat( Fn::Property::D_SELECTED_MAX, 1.0f, 0.0f, 1.0f, "color" );
        m_properties["maingl"].createFloat( Fn::Property::D_LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f, "color" );
        m_properties["maingl"].createFloat( Fn::Property::D_UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f, "color" );
    }
    else
    {
        m_properties["maingl"].createInt( Fn::Property::D_COLORMAP, 1 );
        m_properties["maingl"].createFloat( Fn::Property::D_SELECTED_MIN, 0.0f, 0.0f, 1.0f );
        m_properties["maingl"].createFloat( Fn::Property::D_SELECTED_MAX, 1.0f, 0.0f, 1.0f );
        m_properties["maingl"].createFloat( Fn::Property::D_LOWER_THRESHOLD, 0.0f, 0.0f, 1.0f );
        m_properties["maingl"].createFloat( Fn::Property::D_UPPER_THRESHOLD, 1.0f, 0.0f, 1.0f );
    }
    m_properties["maingl"].createFloat( Fn::Property::D_DX, 2000.0f, 0.0f, 2000.0f, "special" );
    m_properties["maingl"].createFloat( Fn::Property::D_DY, 2000.0f, 0.0f, 2000.0f, "special" );
    m_properties["maingl"].createFloat( Fn::Property::D_DZ, 2000.0f, 0.0f, 2000.0f, "special" );
    m_properties["maingl"].createInt( Fn::Property::D_NX, 800, 0, 2000, "special" );
    m_properties["maingl"].createInt( Fn::Property::D_NY, 1000, 0, 2000, "special" );
    m_properties["maingl"].createInt( Fn::Property::D_NZ, 800, 0, 2000, "special" );

    m_properties["maingl"].createFloat( Fn::Property::D_FIBER_MORPH, 1.0f, 0.0f, 1.0f, "special" );

    m_properties["maingl"].createBool( Fn::Property::D_LIGHT_SWITCH, true, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_LIGHT_AMBIENT,   0.2f, 0.0f, 1.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_LIGHT_DIFFUSE,   0.45f, 0.0f, 1.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_LIGHT_SPECULAR,  0.5f, 0.0f, 1.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_AMBIENT,   2.0f, 0.0f, 10.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_DIFFUSE,   0.8f, 0.0f, 10.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_SPECULAR,  0.61f, 0.0f, 10.0f, "light" );
    m_properties["maingl"].createFloat( Fn::Property::D_MATERIAL_SHININESS, 2.0f, 0.0f, 200.0f, "light" );

    unsigned int maxLength = 0;
    for ( unsigned int i = 0; i < m_fibs.size(); ++i )
    {
        maxLength = qMax( maxLength, m_fibs[i].length() );
    }

    m_properties["maingl"].createFloat( Fn::Property::D_FIBER_GROW_LENGTH, (float)maxLength, 0.0f, (float)maxLength, "special" );

    connect( m_properties["maingl"].getProperty( Fn::Property::D_COLOR ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( colorChanged() ) );

    PropertyGroup props2( m_properties["maingl"] );
    m_properties.insert( "maingl2", props2 );

    if ( hasData )
    {
//        connect( m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
//                      m_properties["maingl"].getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
//        connect( m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
//                  m_properties["maingl"].getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );

        connect( m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
                  m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MAX ), SLOT( setMin( QVariant ) ) );
        connect( m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
                  m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MIN ), SLOT( setMax( QVariant ) ) );

        connect( m_properties["maingl"].getProperty( Fn::Property::D_DATAMODE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( dataModeChanged() ) );

//        connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
//                      m_properties["maingl2"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
//        connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
//                  m_properties["maingl2"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );

        connect( m_properties["maingl2"].getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
                  m_properties["maingl2"].getProperty( Fn::Property::D_SELECTED_MAX ), SLOT( setMin( QVariant ) ) );
        connect( m_properties["maingl2"].getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
                  m_properties["maingl2"].getProperty( Fn::Property::D_SELECTED_MIN ), SLOT( setMax( QVariant ) ) );

        //connect( m_properties["maingl2"]->getProperty( Fn::Property::D_DATAMODE ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( dataModeChanged() ) );
    }

    m_properties["maingl"].createList( Fn::Property::D_USE_TRANSFORM, { "user defined", "qform", "sform", "qform inverted", "sform inverted" }, 0, "transform" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_USE_TRANSFORM ), SIGNAL( valueChanged( QVariant ) ), this,
                SLOT( transformChanged( QVariant ) ) );
    m_transform.setToIdentity();
    m_properties["maingl"].createMatrix( Fn::Property::D_TRANSFORM, m_transform, "transform" );
    m_properties["maingl"].createButton( Fn::Property::D_APPLY_TRANSFORM, "transform" );
    connect( m_properties["maingl"].getProperty( Fn::Property::D_APPLY_TRANSFORM ), SIGNAL( valueChanged( QVariant ) ), this,
                SLOT( applyTransform() ) );

//    m_properties["maingl"].createBool( Fn::Property::D_AUTOPLAY, false );
//    m_properties["maingl"].createInt( Fn::Property::D_AUTOPLAY_INTERVAL, 25, 10, 1000 );
//    connect( m_properties["maingl"].getProperty( Fn::Property::D_AUTOPLAY ), SIGNAL( valueChanged( QVariant ) ), this, SLOT( autoplay() ) );
}

std::vector<Fib>* DatasetFibers::getFibs()
{
    return &m_fibs;
}

std::vector<Fib> DatasetFibers::getSelectedFibs()
{
    if ( m_renderer == 0 )
    {
        return m_fibs;
    }
    else
    {
        std::vector<bool>*selected = m_selector->getSelection();
        std::vector<Fib>out;
        int first = 0;
        for ( unsigned int i = 0; i < selected->size(); ++i )
        {
            if ( selected->at( i ) )
            {
                first = i;
                break;
            }
        }
        Fib f0 = m_fibs[first];
        QVector3D start;
        if ( Models::r()->rowCount()  > 0 )
        {
            int shape = Models::r()->data( m_selector->createIndex( 0, 0, (int)Fn::Property::D_SHAPE ), Qt::DisplayRole ).toInt();
            if ( shape >= 0 && shape <= 3 )
            {
                float x = Models::r()->data( m_selector->createIndex( 0, 0, (int)Fn::Property::D_X ), Qt::DisplayRole ).toFloat();
                float y = Models::r()->data( m_selector->createIndex( 0, 0, (int)Fn::Property::D_Y ), Qt::DisplayRole ).toFloat();
                float z = Models::r()->data( m_selector->createIndex( 0, 0, (int)Fn::Property::D_Z ), Qt::DisplayRole ).toFloat();
                start = QVector3D( x, y, z );
            }
            else
            {
                start = f0.firstVert();
            }
        }
        else
        {
            start = f0.firstVert();
        }


        for ( unsigned int i = 0; i < selected->size(); ++i )
        {
            if ( selected->at( i ) )
            {
                Fib f = m_fibs[i];
                QVector3D s1 = f.firstVert();
                QVector3D s2 = f.lastVert();

                if ( ( start - s1 ).length() < ( start - s2 ).length() )
                {
                    out.push_back( m_fibs[i] );
                }
                else
                {
                    f.invert();
                    out.push_back( f );
                }
            }
        }
        return out;
    }
}

QList<QString> DatasetFibers::getDataNames()
{
    return m_dataNames;
}

void DatasetFibers::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target ).get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }

    if ( m_resetRenderer )
    {
        delete m_renderer;
        m_renderer = 0;
        delete m_tubeRenderer;
        m_tubeRenderer = 0;
        delete m_selector;
        m_selector = 0;
        if ( !m_kdVerts )
        {
            m_kdVerts = new std::vector<float>();
        }
        m_kdVerts->clear();
        m_kdVerts->reserve( m_numPoints * 3 );
        for ( unsigned int i = 0; i < m_fibs.size(); ++i )
        {
            for ( unsigned int k = 0; k < m_fibs[i].length(); ++k )
            {
                m_kdVerts->push_back( m_fibs[i][k].x() );
                m_kdVerts->push_back( m_fibs[i][k].y() );
                m_kdVerts->push_back( m_fibs[i][k].z() );
            }

        }
        m_resetRenderer = false;
    }

    if ( m_selector == 0 )
    {
        m_selector = new FiberSelector( m_kdVerts, m_numPoints );
        m_selector->init( m_fibs );
        connect( m_selector, SIGNAL( changed() ), Models::d(), SLOT( submit() ) );
    }

    if ( properties( target ).get( Fn::Property::D_FIBER_RENDERMODE).toInt() == 0 )
    {
        if ( m_renderer == 0 )
        {
            m_renderer = new FiberRenderer( m_selector, &m_fibs, m_numPoints );
            m_renderer->init();
            connect( properties( target ).getProperty( Fn::Property::D_COLOR ), SIGNAL( valueChanged( QVariant ) ), m_renderer, SLOT( colorChanged() ) );
        }

        m_renderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }
    else if ( properties( target ).get( Fn::Property::D_FIBER_RENDERMODE).toInt() == 1 )
    {
        if ( m_tubeRenderer == 0 )
        {
            m_tubeRenderer = new TubeRenderer( m_selector, &m_fibs );
            m_tubeRenderer->init();
            connect( properties( target ).getProperty( Fn::Property::D_COLOR ), SIGNAL( valueChanged( QVariant ) ), m_tubeRenderer, SLOT( colorChanged() ) );
        }

        m_tubeRenderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }
}

void DatasetFibers::transformChanged( QVariant value )
{
    QMatrix4x4 qForm;
    QMatrix4x4 sForm;

    QList<Dataset*>dsl = Models::getDatasets( Fn::DatasetType::NIFTI_ANY );

    if ( dsl.size() > 0 )
    {
        qForm = dsl.first()->properties().get( Fn::Property::D_Q_FORM ).value<QMatrix4x4>();
        sForm = dsl.first()->properties().get( Fn::Property::D_S_FORM ).value<QMatrix4x4>();
    }

    m_properties["maingl"].getWidget( Fn::Property::D_TRANSFORM )->setEnabled( false );

    switch ( value.toInt() )
    {
        case 0:
            m_transform.setToIdentity();
            m_properties["maingl"].getWidget( Fn::Property::D_TRANSFORM )->setEnabled( true );
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

    m_properties["maingl"].set( Fn::Property::D_TRANSFORM, m_transform );
    Models::d()->submit();
}

void DatasetFibers::applyTransform()
{
    m_transform = m_properties["maingl"].get( Fn::Property::D_TRANSFORM ).value<QMatrix4x4>();

    for ( unsigned int i = 0; i < m_fibs.size(); ++i )
    {
        Fib fib = m_fibs[i];
        for ( unsigned int k = 0; k < fib.length(); ++k )
        {
            QVector3D vert = fib.getVert( k );
            vert = m_transform * vert;
            fib.setVert( k, vert );
        }
        m_fibs[i] = fib;
    }

    m_resetRenderer = true;
    transformChanged( 0 );
    Models::d()->submit();
}

QString DatasetFibers::getSaveFilter()
{
    return QString( "fib files binary(*.fib *.vtk);;fib files ascii (*.fib *.vtk);;fib files json (*.json);;trackvis (*.trk);;all files (*.*)" );
}

QString DatasetFibers::getDefaultSuffix()
{
    return QString( "fib" );
}

void DatasetFibers::copyFromLoader( LoaderVTK* lv )
{
    m_kdVerts = lv->getPoints();
    std::vector<int> lines = lv->getLines();
    m_numLines = lv->getNumLines();

    qDebug() << "points size:" << m_kdVerts->size() << "lines size:" << lines.size() << "num lines:" << m_numLines;

    int lc = 0;
    int pc = 0;
    float x, y, z;
    for ( unsigned int i = 0; i < m_numLines; ++i )
    {
        Fib fib;
        int lineSize = lines[lc];

        for ( int k = 0; k < lineSize; ++k )
        {
            x = m_kdVerts->at( pc++ );
            y = m_kdVerts->at( pc++ );
            z = m_kdVerts->at( pc++ );
            fib.addVert( x, y, z );
        }
        lc += lineSize + 1;
        m_fibs.push_back( fib );
    }

    std::vector<unsigned char> colors = lv->getPrimitiveColors();

    if ( colors.size() == m_numLines * 3 )
    {
        for ( unsigned int i = 0; i < m_numLines; ++i )
        {
            m_fibs[i].setCustomColor( QColor( ( (float)colors[i * 3] ),
                                              ( (float)colors[i * 3 + 1] ),
                                              ( (float)colors[i * 3 + 2] ), 255 ) );
        }
    }

    std::vector<std::vector<float> > pointData = lv->getPointData();
    m_dataNames = lv->getPointDataNames();

    if ( pointData.size() > 0 )
    {
        qDebug() << pointData.size() << "point data fields found";
        for ( unsigned int curField = 0; curField < pointData.size(); ++curField )
        {
            std::vector<float> field = pointData[curField];
            float min = std::numeric_limits<float>::max();
            float max = std::numeric_limits<float>::min();

            for ( unsigned int i = 0; i < field.size(); ++i )
            {
                float value = field[i];
                min = qMin( min, value );
                max = qMax( max, value );
            }
            qDebug() << m_dataNames[curField] << min << " " << max;
            m_dataMins.push_back( min );
            m_dataMaxes.push_back( max );
        }
        for ( unsigned int curField = 0; curField < pointData.size(); ++curField )
        {
            std::vector<float> field = pointData[curField];
            int lc = 0;
            int pc = 0;
            for ( unsigned int i = 0; i < m_numLines; ++i )
            {
                Fib fib = m_fibs[i];
                fib.addDataField();
                int lineSize = fib.length();
                for ( int k = 0; k < lineSize; ++k )
                {
                    fib.setData( curField, k, field[pc++] );
                }
                m_fibs[i] = fib;
                lc += lineSize + 1;
            }
        }
    }
    else
    {
        m_dataNames.push_back( "no data" );
    }
}

void DatasetFibers::colorChanged()
{
    std::vector<bool>*selected = m_selector->getSelection();
    for ( unsigned int i = 0; i < m_numLines; ++i )
    {
        if ( selected->at( i ) )
        {
            m_fibs[ i ].setCustomColor( m_properties["maingl"].getProperty( Fn::Property::D_COLOR )->getValue().value<QColor>() );
        }
    }
    if ( m_renderer != 0 )
    {
        m_renderer->colorChanged();
    }
    if ( m_tubeRenderer != 0 )
    {
        m_tubeRenderer->colorChanged();
    }
}

void DatasetFibers::dataModeChanged()
{
    if ( m_renderer != 0 )
    {
        m_renderer->setExtraData( properties( "maingl" ).get( Fn::Property::D_DATAMODE).toInt() );
    }
    if ( m_tubeRenderer != 0 )
    {
        m_tubeRenderer->setExtraData( properties( "maingl" ).get( Fn::Property::D_DATAMODE).toInt() );
    }
    float min = m_dataMins[ m_properties["maingl"].get( Fn::Property::D_DATAMODE).toInt()];
    float max = m_dataMaxes[ m_properties["maingl"].get( Fn::Property::D_DATAMODE).toInt()];

    m_properties["maingl"].set( Fn::Property::D_MIN, min );
    m_properties["maingl"].set( Fn::Property::D_MAX, max );
    m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MIN )->setMin( min );
    m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MIN )->setMax( max );
    m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MAX )->setMin( min );
    m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MAX )->setMax( max );
    m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MIN )->setValue( min );
    m_properties["maingl"].getProperty( Fn::Property::D_SELECTED_MAX )->setValue( max );

    m_properties["maingl"].getProperty( Fn::Property::D_LOWER_THRESHOLD )->setMin( min );
    m_properties["maingl"].getProperty( Fn::Property::D_LOWER_THRESHOLD )->setMax( max );
    m_properties["maingl"].getProperty( Fn::Property::D_UPPER_THRESHOLD )->setMin( min );
    m_properties["maingl"].getProperty( Fn::Property::D_UPPER_THRESHOLD )->setMax( max );
    m_properties["maingl"].getProperty( Fn::Property::D_LOWER_THRESHOLD )->setValue( min );
    m_properties["maingl"].getProperty( Fn::Property::D_UPPER_THRESHOLD )->setValue( max );
    Models::d()->submit();
}

std::vector<float> DatasetFibers::getDataMins()
{
    return m_dataMins;
}

std::vector<float> DatasetFibers::getDataMaxes()
{
    return m_dataMaxes;
}

void DatasetFibers::setDataMins( std::vector<float> mins )
{
    m_dataMins = mins;
}

void DatasetFibers::setDataMaxes( std::vector<float> maxes )
{
    m_dataMaxes = maxes;
}

unsigned int DatasetFibers::numVerts()
{
    return m_numPoints;
}

unsigned int DatasetFibers::numLines()
{
    return m_numLines;
}
