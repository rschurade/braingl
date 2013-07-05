/*
 * datasetglyphset.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: boettgerj
 */

#include "datasetglyphset.h"
#include "datasetcons.h"

#include "../mesh/trianglemesh2.h"

#include "../../gui/gl/meshrenderer.h"

#include "../../algos/connection.h"
#include "../../algos/connections.h"

#include "../models.h"

#include "qfile.h"
#include "qfiledialog.h"

#include "qmath.h"

#include <qcoreapplication.h>

DatasetGlyphset::DatasetGlyphset( QDir filename, float mt, float maxt = 1.0 ) :
                DatasetSurfaceset( filename, Fn::DatasetType::GLYPHSET ),
                minthresh( mt ),
                maxthresh( maxt ),
                conn( NULL ),
                m_prenderer( NULL ),
                m_vrenderer( NULL ),
                m_pierenderer( NULL ),
                prevGeo( -1 ),
                prevGlyph( -1 ),
                prevCol( -1 ),
                prevGlyphstyle( -1 ),
                prevThresh( -1 ),
                prevMinlength( -1 ),
                m_colors_name( "" ),
                pickedID( -1 )
{
    qDebug() << "minthresh set to: " << minthresh;

    m_properties["maingl"]->set( Fn::Property::D_THRESHOLD, 0.0f, minthresh, 1.0f, "general" );
    m_properties["maingl"]->set( Fn::Property::D_GLYPHSTYLE,
    { "points", "vectors", "pies" }, 0, "glyphs" ); //0 = points, 1 = vectors, 2 = pies
    m_properties["maingl"]->set( Fn::Property::D_GLYPHRADIUS, 0.01f, 0.0f, 0.5f, "glyphs" );
    m_properties["maingl"]->set( Fn::Property::D_NORMALIZATION, 0.5f, 0.0f, 1.0f, "glyphs" );
    m_properties["maingl"]->set( Fn::Property::D_PRIMSIZE, 0.5f, 0.0f, 10.0f, "glyphs" );
    m_properties["maingl"]->set( Fn::Property::D_MINLENGTH, 0.0f, 0.0f, 100.0f, "general" );
    m_properties["maingl"]->set( Fn::Property::D_DRAW_SURFACE, true, "general" );
    m_properties["maingl"]->set( Fn::Property::D_DRAW_GLYPHS, true, "general" );
    m_properties["maingl"]->set( Fn::Property::D_GLYPH_ROT_X, 0.0f, 0.0f, 360.0f, "glyphs" );
    m_properties["maingl"]->set( Fn::Property::D_GLYPH_ROT_Y, 0.0f, 0.0f, 360.0f, "glyphs" );
    m_properties["maingl"]->set( Fn::Property::D_GLYPH_ROT_Z, 0.0f, 0.0f, 360.0f, "glyphs" );
    m_properties["maingl"]->set( Fn::Property::D_GLYPH_ALPHA, 1.0f, 0.0f, 1.0f, "glyphs" );

    float min = -1.0;
    float max = 1.0;

    m_properties["maingl"]->set( Fn::Property::D_MIN, min );
    m_properties["maingl"]->set( Fn::Property::D_MAX, max );
    m_properties["maingl"]->set( Fn::Property::D_SELECTED_MIN, min, min, max, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_SELECTED_MAX, max, min, max, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_LOWER_THRESHOLD, min + ( max - min ) / 1000., min, max, "colormap" );
    m_properties["maingl"]->set( Fn::Property::D_UPPER_THRESHOLD, max, min, max, "colormap" );

    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( float ) ),
            m_properties["maingl"]->getProperty( Fn::Property::D_LOWER_THRESHOLD ), SLOT( setMax( float ) ) );
    connect( m_properties["maingl"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( float ) ),
            m_properties["maingl"]->getProperty( Fn::Property::D_UPPER_THRESHOLD ), SLOT( setMin( float ) ) );

    m_properties["maingl2"]->set( Fn::Property::D_THRESHOLD, 0.0f, minthresh, 1.0f, "general" );
    //m_properties["maingl2"]->set( Fn::Property::D_GLYPHSTYLE, { "points", "vectors", "pies" }, 0, true );
    m_properties["maingl2"]->set( Fn::Property::D_GLYPHRADIUS, 0.01f, 0.0f, 0.5f, "glyphs" );
    m_properties["maingl2"]->set( Fn::Property::D_NORMALIZATION, 0.5f, 0.0f, 1.0f, "glyphs" );
    m_properties["maingl2"]->set( Fn::Property::D_PRIMSIZE, 0.5f, 0.0f, 10.0f, "glyphs" );
    m_properties["maingl2"]->set( Fn::Property::D_MINLENGTH, 0.0f, 0.0f, 100.0f, "general" );
    m_properties["maingl2"]->set( Fn::Property::D_DRAW_SURFACE, true, "general" );
    m_properties["maingl2"]->set( Fn::Property::D_DRAW_GLYPHS, false );
    //m_properties.remove( "maingl2" );

    m_properties["maingl2"]->set( Fn::Property::D_MIN, min );
    m_properties["maingl2"]->set( Fn::Property::D_MAX, max );
    m_properties["maingl2"]->set( Fn::Property::D_SELECTED_MIN, min, min, max, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_SELECTED_MAX, max, min, max, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_LOWER_THRESHOLD, min + ( max - min ) / 1000., min, max, "colormap" );
    m_properties["maingl2"]->set( Fn::Property::D_UPPER_THRESHOLD, max, min, max, "colormap" );

    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SIGNAL( valueChanged( float ) ),
            m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SLOT( setMin( float ) ) );
    connect( m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MAX ), SIGNAL( valueChanged( float ) ),
            m_properties["maingl2"]->getProperty( Fn::Property::D_SELECTED_MIN ), SLOT( setMax( float ) ) );

}

DatasetGlyphset::~DatasetGlyphset()
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
    if ( conn )
    {
        for ( int i = 0; i < n; i++ )
        {
            delete[] conn[i];
            conn[i] = NULL;
        }
        delete[] conn;
        conn = NULL;
    }
}

void DatasetGlyphset::readConnectivity( QString filename )
{
    QFile f( filename );
    if ( !f.open( QIODevice::ReadOnly ) )
        qDebug() << "binary connectivity unreadable: " << filename;

    //This assumes a square matrix of float32...
    n = qSqrt( f.size() / 4 );
    qDebug() << "connectivity matrix size: " << n;
    conn = new float*[n];
    for ( int i = 0; i < n; i++ )
    {
        conn[i] = new float[n];
    }

    QDataStream ins( &f );
    //TODO: What's up with the byte order? Do we have to offer an option for that?
    ins.setByteOrder( QDataStream::LittleEndian );
    ins.setFloatingPointPrecision( QDataStream::SinglePrecision );
    qDebug() << "reading binary connectivity between " << n << " nodes...";

    for ( int i = 0; i < n; i++ )
    {
        for ( int j = 0; j < n; j++ )
        {
            ins >> conn[i][j];
            //qDebug() << i << j << conn[i][j];
        }
    }
    f.close();
    qDebug() << "connectivity read";
}

void DatasetGlyphset::setMinthresh( float mt )
{
    //TODO: Check how I set that...
    minthresh = mt;
}

void DatasetGlyphset::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode, QString target )
{
    if ( !properties( target )->get( Fn::Property::D_ACTIVE ).toBool() )
    {
        return;
    }

    if ( properties( target )->get( Fn::Property::D_DRAW_SURFACE ).toBool() )
    {
        DatasetSurfaceset::draw( pMatrix, mvMatrix, width, height, renderMode, target );
    }

    //TODO: Little brains...
    /*if ( ( target == "maingl2" ) && ( pickedID > 1 ) )
     {
     float* nodes = m_mesh.at( properties( target )->get( Fn::Property::D_SURFACE ).toInt() )->getVertices();
     int triangleshift = 7; //TODO: Why 7?
     for ( int i = 0; i < picked.size(); ++i )
     {
     MeshRenderer* m_renderer = new MeshRenderer( getMesh( target ) );

     m_renderer->setModel( Models::g() );
     m_renderer->init();
     QMatrix4x4 sc;
     int j = picked[i];
     for ( int p = 0; p < n; ++p )
     {
     getMesh( target )->setVertexData( p, conn[j][p] );
     }
     QVector3D f = QVector3D( nodes[j * triangleshift], nodes[j * triangleshift + 1], nodes[j * triangleshift + 2] );
     sc.translate( f );
     sc.scale( 0.01 );
     m_renderer->draw( pMatrix, mvMatrix * sc, width, height, renderMode, properties( target ) );
     delete m_renderer;
     }
     }*/

    int geoSurf = properties( "maingl" )->get( Fn::Property::D_SURFACE ).toInt();
    int geoGlyph = properties( "maingl" )->get( Fn::Property::D_SURFACE_GLYPH_GEOMETRY ).toInt();
    int geoCol = properties( "maingl" )->get( Fn::Property::D_SURFACE_GLYPH_COLOR ).toInt();
    int glyphstyle = properties( "maingl" )->get( Fn::Property::D_GLYPHSTYLE ).toInt();

    float threshold = properties( "maingl" )->get( Fn::Property::D_THRESHOLD ).toFloat();
    float minlength = properties( "maingl" )->get( Fn::Property::D_MINLENGTH ).toFloat();

    //TODO: How do we get this to work properly again?
    glEnable( GL_BLEND );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_POINT_SMOOTH );
    glPointSize( properties( "maingl" )->get( Fn::Property::D_PRIMSIZE ).toFloat() );
    glLineWidth( properties( "maingl" )->get( Fn::Property::D_PRIMSIZE ).toFloat() );

    //TODO: Make transparency right, using other rendermodes, adapt shaders?

    if ( ( glyphstyle == 0 )
            && ( ( m_prenderer == 0 ) || ( prevGeo != geoSurf ) || ( prevGlyph != geoGlyph ) || ( prevCol != geoCol )
                    || ( prevGlyphstyle != glyphstyle ) ) )
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

    if ( ( glyphstyle == 1 )
            && ( m_vrenderer == 0 || ( prevGeo != geoSurf ) || ( prevGlyph != geoGlyph ) || ( prevCol != geoCol ) || ( prevGlyphstyle != glyphstyle ) ) )
    {
        if ( m_vrenderer )
        {
            delete m_vrenderer;
        }
        m_vrenderer = new VectorGlyphRenderer();
        m_vrenderer->init();
        //TODO: vector fields:
        makeVecs();
        m_vrenderer->initGeometry( vecsArray, vecsNumber );
    }

    if ( ( glyphstyle == 2 )
            && ( m_pierenderer == 0 || ( prevGeo != geoSurf ) || ( prevGlyph != geoGlyph ) || ( prevCol != geoCol )
                    || ( prevGlyphstyle != glyphstyle ) || ( prevThresh != threshold ) || ( prevMinlength != minlength ) ) )
    {
        if ( m_pierenderer )
        {
            delete m_pierenderer;
        }
        m_pierenderer = new PieGlyphRenderer();
        m_pierenderer->init();
        makePies();
        m_pierenderer->initGeometry( pieArrays, numbers );
    }

    //TODO: Deal with multiple passes / switching btw. styles etc.:
    //TODO: Optimize switching of stuff
    prevGeo = geoSurf;
    prevGlyph = geoGlyph;
    prevCol = geoCol;
    prevGlyphstyle = glyphstyle;
    prevThresh = threshold;
    prevMinlength = minlength;

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

void DatasetGlyphset::makeCons()
{
    qDebug() << "making consArray: " << minthresh << " maxthresh: " << maxthresh;
    //if (consArray) delete[] consArray;
    int geo = m_properties["maingl"]->get( Fn::Property::D_SURFACE ).toInt();
    int glyph = m_properties["maingl"]->get( Fn::Property::D_SURFACE_GLYPH_GEOMETRY ).toInt();
    int col = m_properties["maingl"]->get( Fn::Property::D_SURFACE_GLYPH_COLOR ).toInt();

    float* nodes = m_mesh.at( geo )->getVertices();
    float* glyphnodes = m_mesh.at( glyph )->getVertices();
    float* colornodes = m_mesh.at( col )->getVertices();

    n = m_mesh.at( geo )->numVerts();
    qDebug() << "nodes: " << n;
    consNumber = 0;
    //count first, then create array...
    //TODO: Parallelize, protect from renderthread interruptions?...
    for ( int i = 0; i < n; ++i )
    {
        for ( int j = 0; j < n; ++j )
        {
            float v = conn[i][j];
            if ( ( v > minthresh ) && ( v < maxthresh ) && roi[i] )
            {
                ++consNumber;
            }
        }
    }
    qDebug() << consNumber << " connections above threshold";
    int offset = 13;
    consArray = new float[offset * consNumber];
    consNumber = 0;
    for ( int i = 0; i < n; ++i )
    {
        for ( int j = 0; j < n; ++j )
        {
            float v = conn[i][j];
            if ( ( v > minthresh ) && ( v < maxthresh ) && roi[i] )
            {

                int triangleshift = 7; //TODO: Why 7?

                QVector3D f = QVector3D( nodes[i * triangleshift], nodes[i * triangleshift + 1], nodes[i * triangleshift + 2] );
                QVector3D t = QVector3D( nodes[j * triangleshift], nodes[j * triangleshift + 1], nodes[j * triangleshift + 2] );

                QVector3D fg = QVector3D( glyphnodes[i * triangleshift], glyphnodes[i * triangleshift + 1], glyphnodes[i * triangleshift + 2] );
                QVector3D tg = QVector3D( glyphnodes[j * triangleshift], glyphnodes[j * triangleshift + 1], glyphnodes[j * triangleshift + 2] );
                QVector3D dg = tg - fg;

                QVector3D fc = QVector3D( colornodes[i * triangleshift], colornodes[i * triangleshift + 1], colornodes[i * triangleshift + 2] );
                QVector3D tc = QVector3D( colornodes[j * triangleshift], colornodes[j * triangleshift + 1], colornodes[j * triangleshift + 2] );
                QVector3D dc = tc - fc;

                consArray[offset * consNumber] = f.x();
                consArray[offset * consNumber + 1] = f.y();
                consArray[offset * consNumber + 2] = f.z();
                consArray[offset * consNumber + 3] = v;
                consArray[offset * consNumber + 4] = t.x();
                consArray[offset * consNumber + 5] = t.y();
                consArray[offset * consNumber + 6] = t.z();

                consArray[offset * consNumber + 7] = dg.x();
                consArray[offset * consNumber + 8] = dg.y();
                consArray[offset * consNumber + 9] = dg.z();

                consArray[offset * consNumber + 10] = dc.x();
                consArray[offset * consNumber + 11] = dc.y();
                consArray[offset * consNumber + 12] = dc.z();

                ++consNumber;
            }
        }
    }
}

void DatasetGlyphset::makeVecs()
{
    qDebug() << "making vecsArray: " << minthresh;
    //if (vecsArray) delete[] vecsArray;
    int geo = m_properties["maingl"]->get( Fn::Property::D_SURFACE ).toInt();
    int glyph = m_properties["maingl"]->get( Fn::Property::D_SURFACE_GLYPH_GEOMETRY ).toInt();
    int col = m_properties["maingl"]->get( Fn::Property::D_SURFACE_GLYPH_COLOR ).toInt();

    float* nodes = m_mesh.at( geo )->getVertices();
    float* glyphnodes = m_mesh.at( glyph )->getVertices();
    float* colornodes = m_mesh.at( col )->getVertices();

    n = m_mesh.at( geo )->numVerts();
    qDebug() << "nodes: " << n;
    vecsNumber = 0;
    //count first, then create array...
    //TODO: Parallelize, protect from renderthread interruptions?...
    for ( int i = 0; i < n; ++i )
    {
        for ( int j = 0; j < n; ++j )
        {
            float v = conn[i][j];
            if ( ( v > minthresh ) && ( v < maxthresh ) && roi[i] )
            {
                ++vecsNumber;
            }
        }
    }
    qDebug() << vecsNumber << " connections above threshold";
    int offset = 28;
    vecsArray = new float[offset * vecsNumber];
    vecsNumber = 0;
    for ( int i = 0; i < n; ++i )
    {
        for ( int j = 0; j < n; ++j )
        {
            float v = conn[i][j];
            if ( ( v > minthresh ) && ( v < maxthresh ) && roi[i] )
            {

                int triangleshift = 7; //TODO: Why 7?

                QVector3D f = QVector3D( nodes[i * triangleshift], nodes[i * triangleshift + 1], nodes[i * triangleshift + 2] );
                QVector3D t = QVector3D( nodes[j * triangleshift], nodes[j * triangleshift + 1], nodes[j * triangleshift + 2] );

                QVector3D fg = QVector3D( glyphnodes[i * triangleshift], glyphnodes[i * triangleshift + 1], glyphnodes[i * triangleshift + 2] );
                QVector3D tg = QVector3D( glyphnodes[j * triangleshift], glyphnodes[j * triangleshift + 1], glyphnodes[j * triangleshift + 2] );
                QVector3D dg = tg - fg;

                QVector3D fc = QVector3D( colornodes[i * triangleshift], colornodes[i * triangleshift + 1], colornodes[i * triangleshift + 2] );
                QVector3D tc = QVector3D( colornodes[j * triangleshift], colornodes[j * triangleshift + 1], colornodes[j * triangleshift + 2] );
                QVector3D dc = tc - fc;

                vecsArray[offset * vecsNumber] = f.x();
                vecsArray[offset * vecsNumber + 1] = f.y();
                vecsArray[offset * vecsNumber + 2] = f.z();
                vecsArray[offset * vecsNumber + 3] = t.x();
                vecsArray[offset * vecsNumber + 4] = t.y();
                vecsArray[offset * vecsNumber + 5] = t.z();

                vecsArray[offset * vecsNumber + 6] = v;
                vecsArray[offset * vecsNumber + 7] = 1;

                vecsArray[offset * vecsNumber + 8] = dg.x();
                vecsArray[offset * vecsNumber + 9] = dg.y();
                vecsArray[offset * vecsNumber + 10] = dg.z();

                vecsArray[offset * vecsNumber + 11] = dc.x();
                vecsArray[offset * vecsNumber + 12] = dc.y();
                vecsArray[offset * vecsNumber + 13] = dc.z();

                vecsArray[offset * vecsNumber + 14] = t.x();
                vecsArray[offset * vecsNumber + 15] = t.y();
                vecsArray[offset * vecsNumber + 16] = t.z();
                vecsArray[offset * vecsNumber + 17] = f.x();
                vecsArray[offset * vecsNumber + 18] = f.y();
                vecsArray[offset * vecsNumber + 19] = f.z();

                vecsArray[offset * vecsNumber + 20] = v;
                vecsArray[offset * vecsNumber + 21] = -1;

                vecsArray[offset * vecsNumber + 22] = -dg.x();
                vecsArray[offset * vecsNumber + 23] = -dg.y();
                vecsArray[offset * vecsNumber + 24] = -dg.z();

                vecsArray[offset * vecsNumber + 25] = -dc.x();
                vecsArray[offset * vecsNumber + 26] = -dc.y();
                vecsArray[offset * vecsNumber + 27] = -dc.z();

                ++vecsNumber;
            }
        }
    }
}

//TODO: replace with something nicer...
bool edgeCompare( Connection* e1, Connection* e2 )
{
    QColor c1( e1->r * 255, e1->g * 255, e1->b * 255 );
    QColor c2( e2->r * 255, e2->g * 255, e2->b * 255 );
    return c1.hueF() > c2.hueF();
}

void DatasetGlyphset::makePies()
{

    qDebug() << "makePies begin";

    float minlength = m_properties["maingl"]->get( Fn::Property::D_MINLENGTH ).toFloat();
    float threshold = m_properties["maingl"]->get( Fn::Property::D_THRESHOLD ).toFloat();
    if ( threshold < minthresh )
        threshold = minthresh;

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

    int geo = m_properties["maingl"]->get( Fn::Property::D_SURFACE ).toInt();
    //int glyph = m_properties["maingl"]->get( Fn::Property::D_SURFACE_GLYPH_GEOMETRY ).toInt();
    int col = m_properties["maingl"]->get( Fn::Property::D_SURFACE_GLYPH_COLOR ).toInt();

    float* nodes = m_mesh.at( geo )->getVertices();
    //float* glyphnodes = m_mesh.at( glyph )->getVertices();
    float* colornodes = m_mesh.at( col )->getVertices();

    n = m_mesh.at( geo )->numVerts();
    pieArrays = new QVector<float*>( n, NULL );
    numbers = new QVector<int>( n );

    //for all nodes in the current surface...
    //count first and throw super-threshold connections in sortable list, then create arrays...
    //TODO: Parallelize, protect from renderthread interruptions?...
    for ( int i = 0; i < n; ++i )
    {
        int count = 0;

        QList<Connection*> sortlist;

        for ( int j = 0; j < n; ++j )
        {
            float v = conn[i][j];
            if ( ( v > threshold ) && ( v < maxthresh ) && roi[i] )
            {
                int triangleshift = 7; //TODO: Why 7?

                QVector3D f = QVector3D( nodes[i * triangleshift], nodes[i * triangleshift + 1], nodes[i * triangleshift + 2] );
                QVector3D t = QVector3D( nodes[j * triangleshift], nodes[j * triangleshift + 1], nodes[j * triangleshift + 2] );
                QVector3D gdiff = t - f;

                //QVector3D fg = QVector3D( glyphnodes[i * triangleshift], glyphnodes[i * triangleshift + 1], glyphnodes[i * triangleshift + 2] );
                //QVector3D tg = QVector3D( glyphnodes[j * triangleshift], glyphnodes[j * triangleshift + 1], glyphnodes[j * triangleshift + 2] );
                //QVector3D dg = tg - fg;

                QVector3D fc = QVector3D( colornodes[i * triangleshift], colornodes[i * triangleshift + 1], colornodes[i * triangleshift + 2] );
                QVector3D tc = QVector3D( colornodes[j * triangleshift], colornodes[j * triangleshift + 1], colornodes[j * triangleshift + 2] );
                QVector3D dc = tc - fc;

                if ( gdiff.length() > minlength )
                {
                    sortlist.push_back( new Connection( f, dc, conn[i][j] ) );
                    ++count;
                }
            }
        }
        numbers->replace( i, count );
        //qDebug() << numbers->at( i ) << " connections above threshold at node: " << i;

        //Magic!:
        //TODO: There is some memory leakage somewhere...
        qSort( sortlist.begin(), sortlist.end(), edgeCompare );

        int offset = 8;
        float* pieNodeArray = NULL;
        if ( count > 0 )
            pieNodeArray = new float[offset * count];
        for ( int nodecount = 0; nodecount < count; ++nodecount )
        {
            Connection* c = sortlist.at( nodecount );

            //TODO: values for pie charts?
            //float v = c->v;

            int o = nodecount * offset;
            pieNodeArray[o] = c->fn.x();
            pieNodeArray[o + 1] = c->fn.y();
            pieNodeArray[o + 2] = c->fn.z();

            pieNodeArray[o + 3] = c->r;
            pieNodeArray[o + 4] = c->g;
            pieNodeArray[o + 5] = c->b;

            pieNodeArray[o + 6] = nodecount;
            pieNodeArray[o + 7] = count;
        }
        pieArrays->replace( i, pieNodeArray );
    }
}

QList<Dataset*> DatasetGlyphset::createConnections()
{
    float threshold = m_properties["maingl"]->get( Fn::Property::D_THRESHOLD ).toFloat();
    int geo = m_properties["maingl"]->get( Fn::Property::D_SURFACE ).toInt();
    n = m_mesh.at( geo )->numVerts();
    float* nodes = m_mesh.at( geo )->getVertices();

    //TODO: think about making upper threshold interactive...
    float minlength = m_properties["maingl"]->get( Fn::Property::D_MINLENGTH ).toFloat();
    Connections* cons = new Connections();
    for ( int i = 0; i < n; ++i )
    {
        for ( int j = i + 1; j < n; ++j )
        {
            float v = conn[i][j];
            if ( ( v > threshold ) && ( v < maxthresh ) && roi[i] )
            {
                int triangleshift = 7; //TODO: Why 7?

                QVector3D* f = new QVector3D( nodes[i * triangleshift], nodes[i * triangleshift + 1], nodes[i * triangleshift + 2] );
                QVector3D* t = new QVector3D( nodes[j * triangleshift], nodes[j * triangleshift + 1], nodes[j * triangleshift + 2] );

                Edge* aedge = new Edge( *f, *t );

                if ( aedge->length() > minlength )
                {
                    cons->nodes << *f;
                    cons->nodes << *t;
                    cons->edges << aedge;
                }
            }
        }
    }
    QList<Dataset*> l;
    l.push_back( new DatasetCons( cons ) );
    return l;
}

void DatasetGlyphset::setProperties()
{
    DatasetSurfaceset::setProperties();
    m_properties["maingl"]->set( Fn::Property::D_SURFACE_GLYPH_GEOMETRY, m_displayList, 0, "glyphs" );
    m_properties["maingl"]->set( Fn::Property::D_SURFACE_GLYPH_COLOR, m_displayList, 0, "glyphs" );
}

void DatasetGlyphset::saveRGB()
{
    if ( m_colors_name == "" )
    {
        m_colors_name = QFileDialog::getSaveFileName( NULL, "save RGB file" );
    }
    else
    {
        m_colors_name = QFileDialog::getSaveFileName( NULL, "save RGB file", m_colors_name );
    }
    QFile file( m_colors_name );
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        return;
    }
    QTextStream out( &file );
    float* c = m_mesh.at( prevGeo )->getVertexColors();
    for ( int i = 0; i < m_mesh.at( prevGeo )->numVerts(); i++ )
    {
        out << c[i * 4] << " " << c[i * 4 + 1] << " " << c[i * 4 + 2] << endl;
    }
    file.close();
}

void DatasetGlyphset::loadRGB()
{
    m_colors_name = QFileDialog::getOpenFileName( NULL, "load RGB file", m_colors_name );
    QFile file( m_colors_name );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        return;
    }
    QTextStream in( &file );
    m_renderer->beginUpdateColor();

    for ( int i = 0; i < m_mesh.at( prevGeo )->numVerts(); i++ )
    {
        float r, g, b;
        in >> r >> g >> b;
        for ( int i2 = 0; i2 < m_mesh.size(); i2++ )
        {
            m_renderer->updateColor( i, r, g, b, 1.0 );
            m_mesh.at( i2 )->setVertexColor( i, r, g, b, 1.0 );
        }
    }
    m_renderer->endUpdateColor();
    file.close();
    Models::d()->submit();
}

void DatasetGlyphset::loadROI( QString filename )
{
    qDebug() << "loading roi: " << filename;
    QFile file( filename );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qDebug() << "something went wrong opening ROI file: " << filename;
        return;
    }
    QTextStream in( &file );
    QVector<int> ids;
    while ( !in.atEnd() )
    {
        QString line = in.readLine();
        qDebug() << line << " " << ids.size();
        QStringList sl = line.split( " " );
        ids.append( sl.at( 0 ).toInt() );
    }

    qDebug() << "m_mesh.size(): " << m_mesh.size();
    roi = new bool[m_mesh.at( 0 )->numVerts()];
    for ( int i = 0; i < m_mesh.at( 0 )->numVerts(); i++ )
    {
        roi[i] = false;
        if ( ids.contains( i ) )
        {
            roi[i] = true;
        }
    }
    file.close();
}

void DatasetGlyphset::initROI()
{
    roi = new bool[m_mesh.at( 0 )->numVerts()];
    for ( int i = 0; i < m_mesh.at( 0 )->numVerts(); i++ )
    {
        roi[i] = true;
    }
}

inline uint qHash( const QColor color )
{
    return color.red() + color.green() + color.blue();
}

void DatasetGlyphset::save1Ds()
{
    QString filename = QFileDialog::getSaveFileName( NULL, "save 1D file", m_colors_name + ".col_" );

    QSet<QColor>* colors = new QSet<QColor>();
    for ( int i = 0; i < m_mesh.at( prevGeo )->numVerts(); i++ )
    {
        QColor vcolor = m_mesh.at( prevGeo )->getVertexColor( i );
        QColor erasecolor = m_properties["maingl"]->get( Fn::Property::D_COLOR ).value<QColor>();
        if ( vcolor != erasecolor )
        {
            colors->insert( vcolor );
        }
    }
    qDebug() << "writing: " << colors->size() << " pairs of 1D-files...";
    QList<QColor> color_list = colors->toList();
    for ( int fn = 0; fn < color_list.size(); ++fn )
    {
        QColor c = color_list.at( fn );
        QFile file( filename + QString::number( c.red() ) + "_" + QString::number( c.green() ) + "_" + QString::number( c.blue() ) + ".1D.roi" );
        QFile file2( filename + QString::number( c.red() ) + "_" + QString::number( c.green() ) + "_" + QString::number( c.blue() ) + ".1D" );
        if ( !file.open( QIODevice::WriteOnly ) )
        {
            qDebug() << "file open failed: " << filename;
            return;
        }
        if ( !file2.open( QIODevice::WriteOnly ) )
        {
            qDebug() << "file open failed: " << filename;
            return;
        }
        QTextStream out( &file );
        QTextStream out2( &file2 );
        for ( int i = 0; i < m_mesh.at( prevGeo )->numVerts(); i++ )
        {
            QColor vcolor = m_mesh.at( prevGeo )->getVertexColor( i );
            if ( vcolor == c )
            {
                out << i << " " << fn + 1 << endl;
                out2 << 1.0 << endl;
            }
            else
            {
                out2 << 0.0 << endl;
            }
        }
        file.close();
        file2.close();
    }
}

void DatasetGlyphset::mousePick( int pickId, QVector3D pos, Qt::KeyboardModifiers modifiers, QString target )
{

    //TODO: Extra property for radius...
    picked = getMesh( target )->pick( pos, m_properties["maingl"]->get( Fn::Property::D_PAINTSIZE ).toFloat() );

    pickedID = getMesh( target )->closestVertexIndex( pos );

    DatasetSurfaceset::mousePick( pickId, pos, modifiers, target );
    for ( int i = 0; i < n; ++i )
    {
        if ( pickedID != -1 )
        {
            for ( int m = 0; m < m_mesh.size(); m++ )
            {
                m_mesh[m]->setVertexData( i, conn[pickedID][i] );
            }
        }
    }
    Models::d()->submit();
}
