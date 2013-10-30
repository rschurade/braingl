/*
 * datasetconglyphs.cpp
 *
 *  Created on: Oct 30, 2013
 *      Author: boettgerj
 */

//TODO: check for unneccessary includes:
#include "datasetconglyphs.h"

#include "datasetcons.h"

#include "../properties/propertyfloat.h"

#include "../mesh/trianglemesh2.h"

#include "../../gui/gl/meshrenderer.h"
#include "../../gui/gl/glfunctions.h"
#include "../../gui/gl/colormapfunctions.h"

#include "../../algos/colormapbase.h"
#include "../../algos/connection.h"

#include "../models.h"

#include "qfile.h"
#include "qfiledialog.h"

#include "qmath.h"

#include <qcoreapplication.h>

//a clean new implementation of only the glyphs, starting from a list of connections
// no difference glyphs, because no neighbors
// no surface switching

DatasetConGlyphs::DatasetConGlyphs( QDir filename ) :
                Dataset( filename, Fn::DatasetType::CONGLYPHS ),
                m_prenderer( NULL ),
                m_vrenderer( NULL ),
                m_pierenderer( NULL ),
                prevGlyphstyle( -1 ),
                prevColorMode( -1 ),
                prevThresh( -1 ),
                prevMinlength( -1 )
{
    float min = -1;
    float max = 1;
    m_properties["maingl"]->create( Fn::Property::D_COLORMAP, 1, "general" );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MIN, min, min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_SELECTED_MAX, max, min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_LOWER_THRESHOLD, min + ( max - min ) / 1000., min, max, "general" );
    m_properties["maingl"]->create( Fn::Property::D_UPPER_THRESHOLD, max, min, max, "general" );

    addProperties();

    qDebug() << "properties added";

    PropertyGroup* props2 = new PropertyGroup( *( m_properties["maingl"] ) );
    m_properties.insert( "maingl2", props2 );
    m_properties["maingl2"]->getProperty( Fn::Property::D_ACTIVE )->setPropertyTab( "general" );

    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( QVariant ) ),
            m_properties["maingl2"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( QVariant ) ) );
    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( QVariant ) ),
            m_properties["maingl2"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( QVariant ) ) );

    m_properties["maingl"]->create( Fn::Property::D_DRAW_GLYPHS, true, "general" );
    m_properties["maingl2"]->create( Fn::Property::D_DRAW_GLYPHS, false );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_GLYPHSTYLE ), SIGNAL( valueChanged(QVariant)), this,
            SLOT( glyphStyleChanged(QVariant) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_GLYPH_ROTATION ), SIGNAL( valueChanged(QVariant)), this,
            SLOT( rotationChanged(QVariant) ) );

    m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_ROT_X )->setHidden( true );
    m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_ROT_Y )->setHidden( true );
    m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_ROT_Z )->setHidden( true );

    qDebug() << "more properties added";
}

DatasetConGlyphs::~DatasetConGlyphs()
{
    if ( m_prenderer )
    {
        delete m_prenderer;
        m_prenderer = NULL;
    }
    if ( m_vrenderer )
    {
        delete m_vrenderer;
        m_vrenderer = NULL;
    }
    if ( m_pierenderer )
    {
        delete m_pierenderer;
        m_pierenderer = NULL;
    }
}

void DatasetConGlyphs::setCons( Connections* cons )
{
    m_cons = cons;
}

void DatasetConGlyphs::addProperties()
{
    //TODO: stuff for symmetric thresholding / inversion?
    m_properties["maingl"]->create( Fn::Property::D_THRESHOLD, 0.0f, 0.0f, 1.0f, "general" );
    m_properties["maingl"]->create( Fn::Property::D_GLYPHSTYLE,
    { "points", "vectors", "pies" }, 0, "glyphs" ); //0 = points, 1 = vectors, 2 = pies
    m_properties["maingl"]->create( Fn::Property::D_GLYPHRADIUS, 0.01f, 0.0f, 0.5f, "glyphs" );
    m_properties["maingl"]->create( Fn::Property::D_NORMALIZATION, 0.5f, 0.0f, 1.0f, "glyphs" );
    m_properties["maingl"]->getWidget( Fn::Property::D_NORMALIZATION )->setHidden( true );
    m_properties["maingl"]->create( Fn::Property::D_PRIMSIZE, 0.5f, 0.0f, 10.0f, "glyphs" );
    m_properties["maingl"]->create( Fn::Property::D_MINLENGTH, 0.0f, 0.0f, 100.0f, "general" );
    m_properties["maingl"]->create( Fn::Property::D_GLYPH_ROTATION, false, "glyphs" );
    m_properties["maingl"]->create( Fn::Property::D_GLYPH_ROT_X, 0.0f, 0.0f, 360.0f, "glyphs" );
    m_properties["maingl"]->create( Fn::Property::D_GLYPH_ROT_Y, 0.0f, 0.0f, 360.0f, "glyphs" );
    m_properties["maingl"]->create( Fn::Property::D_GLYPH_ROT_Z, 0.0f, 0.0f, 360.0f, "glyphs" );
    m_properties["maingl"]->create( Fn::Property::D_GLYPH_ALPHA, 1.0f, 0.0f, 1.0f, "glyphs" );

    ( (PropertyFloat*) m_properties["maingl"]->getProperty( Fn::Property::D_GLYPHRADIUS ) )->setDigits( 4 );
    m_properties["maingl"]->create( Fn::Property::D_GLYPH_COLORMODE,
    { "orientation", "value" }, 0, "glyphs" );
}

void DatasetConGlyphs::glyphStyleChanged( QVariant qv )
{
    if ( qv != 2 )
    {
        //Geometry-based glyphs
        m_properties["maingl"]->getWidget( Fn::Property::D_NORMALIZATION )->setHidden( true );
        m_properties["maingl"]->getWidget( Fn::Property::D_PRIMSIZE )->setHidden( false );
        m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_COLORMODE )->setDisabled( false );
    }
    else
    {
        //Piechart glyphs
        m_properties["maingl"]->getWidget( Fn::Property::D_NORMALIZATION )->setHidden( false );
        m_properties["maingl"]->getWidget( Fn::Property::D_PRIMSIZE )->setHidden( true );
        m_properties["maingl2"]->set( Fn::Property::D_GLYPH_COLORMODE, 0 );
        m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_COLORMODE )->setDisabled( false );
    }
}

void DatasetConGlyphs::rotationChanged( QVariant qv )
{
    if ( !qv.toBool() )
    {
        m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_ROT_X )->setHidden( true );
        m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_ROT_Y )->setHidden( true );
        m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_ROT_Z )->setHidden( true );
    }
    else
    {
        m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_ROT_X )->setHidden( false );
        m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_ROT_Y )->setHidden( false );
        m_properties["maingl"]->getWidget( Fn::Property::D_GLYPH_ROT_Z )->setHidden( false );
    }
}

void DatasetConGlyphs::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }

    int glyphstyle = properties( "maingl" )->get( Fn::Property::D_GLYPHSTYLE ).toInt();
    int colorMode = properties( "maingl" )->get( Fn::Property::D_GLYPH_COLORMODE ).toInt();

    float threshold = properties( "maingl" )->get( Fn::Property::D_THRESHOLD ).toFloat();
    float minlength = properties( "maingl" )->get( Fn::Property::D_MINLENGTH ).toFloat();

    //TODO: How do we get this to work properly again?
    glEnable( GL_BLEND );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_POINT_SMOOTH );
    glPointSize( properties( "maingl" )->get( Fn::Property::D_PRIMSIZE ).toFloat() );
    glLineWidth( properties( "maingl" )->get( Fn::Property::D_PRIMSIZE ).toFloat() );

    //TODO: Make transparency right, using other rendermodes, adapt shaders?

    if ( ( glyphstyle == 0 ) && ( ( m_prenderer == 0 ) || ( prevGlyphstyle != glyphstyle ) ) )
    {
        if ( m_prenderer )
        {
            delete m_prenderer;
        }
        m_prenderer = new PointGlyphRenderer();
        m_prenderer->init();
        makeCons();
        m_prenderer->initGeometry( consArray, consNumber );
    }

    if ( ( glyphstyle == 1 ) && ( m_vrenderer == 0 || ( prevGlyphstyle != glyphstyle ) ) )
    {
        if ( m_vrenderer )
        {
            delete m_vrenderer;
        }
        m_vrenderer = new VectorGlyphRenderer();
        m_vrenderer->init();
        makeVecs();
        m_vrenderer->initGeometry( vecsArray, vecsNumber );
    }

    if ( ( glyphstyle == 2 )
            && ( m_pierenderer == 0 || ( prevGlyphstyle != glyphstyle ) || ( prevThresh != threshold ) || ( prevMinlength != minlength )
                    || ( prevColorMode != colorMode ) ) )
    {
        if ( m_pierenderer )
        {
            delete m_pierenderer;
        }
        m_pierenderer = new PieGlyphRenderer();
        m_pierenderer->init();
        makePies();
        m_pierenderer->initGeometry( pieArrays, numbers, maxNodeCount );
    }

    //TODO: Deal with multiple passes / switching btw. styles etc.:
    //TODO: Optimize switching of stuff
    prevGlyphstyle = glyphstyle;
    prevThresh = threshold;
    prevMinlength = minlength;
    prevColorMode = colorMode;

    if ( glyphstyle == 0 )
    {
        m_prenderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }
    if ( glyphstyle == 1 )
    {
        m_vrenderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }
    if ( glyphstyle == 2 )
    {
        m_pierenderer->draw( pMatrix, mvMatrix, width, height, renderMode, properties( target ) );
    }
}

void DatasetConGlyphs::makeCons()
{
    //TODO: check if doubling of connections is necessary...

    //if (consArray) delete[] consArray;

    m_n = m_cons->nodes.size();
    qDebug() << "nodes: " << m_n;
    consNumber = m_cons->edges.size();
    qDebug() << consNumber << " edges";
    int offset = 13;
    consArray = new float[offset * consNumber];
    for ( int i = 0; i < consNumber; ++i )
    {
        Edge* e = m_cons->edges.at( i );
        float v = e->m_value;

        QVector3D f = e->fn;
        QVector3D t = e->tn;
        QVector3D diff = t - f;
        QVector3D dg = diff;
        QVector3D dc = diff;

        consArray[offset * i] = f.x();
        consArray[offset * i + 1] = f.y();
        consArray[offset * i + 2] = f.z();
        consArray[offset * i + 3] = v;
        consArray[offset * i + 4] = t.x();
        consArray[offset * i + 5] = t.y();
        consArray[offset * i + 6] = t.z();

        consArray[offset * i + 7] = dg.x();
        consArray[offset * i + 8] = dg.y();
        consArray[offset * i + 9] = dg.z();

        consArray[offset * i + 10] = dc.x();
        consArray[offset * i + 11] = dc.y();
        consArray[offset * i + 12] = dc.z();
    }
}

void DatasetConGlyphs::makeVecs()
{

//if (vecsArray) delete[] vecsArray;

    m_n = m_cons->nodes.size();
    qDebug() << "nodes: " << m_n;
    vecsNumber = m_cons->edges.size();
    qDebug() << vecsNumber << " vectors";
    int offset = 28;
    vecsArray = new float[offset * vecsNumber];

    for ( int i = 0; i < consNumber; ++i )
    {

        Edge* e = m_cons->edges.at( i );
        float v = e->m_value;

        QVector3D f = e->fn;
        QVector3D t = e->tn;
        QVector3D diff = t - f;
        QVector3D dg = diff;
        QVector3D dc = diff;

        vecsArray[offset * i] = f.x();
        vecsArray[offset * i + 1] = f.y();
        vecsArray[offset * i + 2] = f.z();
        vecsArray[offset * i + 3] = t.x();
        vecsArray[offset * i + 4] = t.y();
        vecsArray[offset * i + 5] = t.z();

        vecsArray[offset * i + 6] = v;
        vecsArray[offset * i + 7] = 1;

        vecsArray[offset * i + 8] = dg.x();
        vecsArray[offset * i + 9] = dg.y();
        vecsArray[offset * i + 10] = dg.z();

        vecsArray[offset * i + 11] = dc.x();
        vecsArray[offset * i + 12] = dc.y();
        vecsArray[offset * i + 13] = dc.z();

        vecsArray[offset * i + 14] = t.x();
        vecsArray[offset * i + 15] = t.y();
        vecsArray[offset * i + 16] = t.z();
        vecsArray[offset * i + 17] = f.x();
        vecsArray[offset * i + 18] = f.y();
        vecsArray[offset * i + 19] = f.z();

        vecsArray[offset * i + 20] = v;
        vecsArray[offset * i + 21] = -1;

        vecsArray[offset * i + 22] = -dg.x();
        vecsArray[offset * i + 23] = -dg.y();
        vecsArray[offset * i + 24] = -dg.z();

        vecsArray[offset * i + 25] = -dc.x();
        vecsArray[offset * i + 26] = -dc.y();
        vecsArray[offset * i + 27] = -dc.z();

    }
}

//TODO: replace with something nicer...
bool edgeCompareHue2( Connection* e1, Connection* e2 )
{
    QColor c1( e1->r * 255, e1->g * 255, e1->b * 255 );
    QColor c2( e2->r * 255, e2->g * 255, e2->b * 255 );
    return c1.hueF() > c2.hueF();
}

bool edgeCompareValue2( Connection* e1, Connection* e2 )
{
    return e1->v > e2->v;
}

void DatasetConGlyphs::makePies()
{

    qDebug() << "makePies begin";

    maxNodeCount = 0;

    float minlength = m_properties["maingl"]->get( Fn::Property::D_MINLENGTH ).toFloat();
    float threshold = m_properties["maingl"]->get( Fn::Property::D_THRESHOLD ).toFloat();

    qDebug() << "threshold: " << threshold;

    /*if ( pieArrays )
     {
     for ( int i = 0; i < pieArrays->size(); ++i )
     {
     if ( pieArrays->at( i ) != NULL )
     delete[] (float*) pieArrays->at( i );
     }
     delete pieArrays;
     pieArrays = NULL;
     delete numbers;
     numbers = NULL;
     }*/

    qDebug() << "calcPies after deletion";

    m_n = m_cons->nodes.size();
    pieArrays = new QVector<float*>( m_n, NULL );
    numbers = new QVector<int>( m_n );
    int edgesNumber = m_cons->edges.size();

//for all nodes: iterate through other nodes, if the same, count, later put in same list...
    for ( int i = 0; i < edgesNumber; ++i )
    {
        int count = 0;

        QList<Connection*> sortlist;

        for ( int j = 0; j < edgesNumber; ++j )
        {
            Edge* e = m_cons->edges.at( i );
            float v = e->m_value;

            QVector3D f = e->fn;
            QVector3D t = e->tn;
            QVector3D gdiff = t - f;
            QVector3D dc = gdiff;

            if ( gdiff.length() > minlength )
            {
                sortlist.push_back( new Connection( f, dc, v ) );
                ++count;
            }
        }
        numbers->replace( i, count );
        if ( count > maxNodeCount )
        {
            maxNodeCount = count;
        }
//qDebug() << numbers->at( i ) << " connections above threshold at node: " << i;

//Magic!:
//TODO: There is some memory leakage somewhere...
        int colorMode = m_properties["maingl"]->get( Fn::Property::D_GLYPH_COLORMODE ).toInt();
        if ( colorMode == 0 )
        {
            qSort( sortlist.begin(), sortlist.end(), edgeCompareHue2 );
        }
        else
        {
            qSort( sortlist.begin(), sortlist.end(), edgeCompareValue2 );
        }

        int offset = 9;
        float* pieNodeArray = NULL;
        if ( count > 0 )
            pieNodeArray = new float[offset * count];
        for ( int nodecount = 0; nodecount < count; ++nodecount )
        {
            Connection* c = sortlist.at( nodecount );

            //TODO: sorting by values?
            float v = c->v;

            int o = nodecount * offset;
            pieNodeArray[o] = c->fn.x();
            pieNodeArray[o + 1] = c->fn.y();
            pieNodeArray[o + 2] = c->fn.z();

            pieNodeArray[o + 3] = c->r;
            pieNodeArray[o + 4] = c->g;
            pieNodeArray[o + 5] = c->b;

            pieNodeArray[o + 6] = nodecount;
            pieNodeArray[o + 7] = count;
            pieNodeArray[o + 8] = v;
        }
        pieArrays->replace( i, pieNodeArray );
    }
}

void DatasetConGlyphs::setProperties()
{
    //TODO: copy stuff from correlation?
    //DatasetCorrelation::setProperties();
}

inline uint qHash( const QColor color )
{
    return color.red() + color.green() + color.blue();
}
