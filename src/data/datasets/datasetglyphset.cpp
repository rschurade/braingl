/*
 * datasetglyphset.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: boettgerj
 */

#include "datasetglyphset.h"
#include "datasetcons.h"

#include "../mesh/trianglemesh2.h"

#include "../../algos/connection.h"
#include "../../algos/connections.h"

#include "qfile.h"

#include "qmath.h"

#include <qcoreapplication.h>

DatasetGlyphset::DatasetGlyphset( QDir filename, float mt ) :
                DatasetSurfaceset( filename, Fn::DatasetType::GLYPHSET ),
                minthresh( mt ),
                m_prenderer( NULL ),
                m_vrenderer( NULL ),
                m_pierenderer( NULL ),
                prevGeo( -1 ),
                prevGlyph( -1 ),
                prevCol( -1 ),
                prevGlyphstyle( -1 ),
                prevThresh( -1 ),
                prevMinlength( -1 )
{
    qDebug() << "minthresh set to: " << minthresh;

    m_properties["maingl"]->set( Fn::Property::THRESHOLD, 0.0f, minthresh, 1.0f, true );
    m_properties["maingl"]->set( Fn::Property::GLYPHSTYLE,
    { "points", "vectors", "pies" }, 0, true ); //0 = points, 1 = vectors, 2 = pies
    m_properties["maingl"]->set( Fn::Property::GLYPHRADIUS, 0.01f, 0.0f, 0.5f, true );
    m_properties["maingl"]->set( Fn::Property::NORMALIZATION, 0.5f, 0.0f, 1.0f, true );
    m_properties["maingl"]->set( Fn::Property::PRIMSIZE, 0.5f, 0.0f, 10.0f, true );
    m_properties["maingl"]->set( Fn::Property::MINLENGTH, 0.0f, 0.0f, 100.0f, true );
    m_properties["maingl"]->set( Fn::Property::DRAW_SURFACE, true, true );
    m_properties["maingl"]->set( Fn::Property::DRAW_GLYPHS, true, true );
    m_properties["maingl"]->set( Fn::Property::GLYPH_ROT_X, 0.0f, 0.0f, 360.0f, true );
    m_properties["maingl"]->set( Fn::Property::GLYPH_ROT_Y, 0.0f, 0.0f, 360.0f, true );
    m_properties["maingl"]->set( Fn::Property::GLYPH_ROT_Z, 0.0f, 0.0f, 360.0f, true );
    m_properties["maingl"]->set( Fn::Property::GLYPH_ALPHA, 1.0f, 0.0f, 1.0f, true );

    /*m_properties["maingl2"]->set( Fn::Property::THRESHOLD, 0.0f, minthresh, 1.0f, true );
    m_properties["maingl2"]->set( Fn::Property::GLYPHSTYLE,
    { "points", "vectors", "pies" }, 0, true ); //0 = points, 1 = vectors, 2 = pies
    m_properties["maingl2"]->set( Fn::Property::GLYPHRADIUS, 0.01f, 0.0f, 0.5f, true );
    m_properties["maingl2"]->set( Fn::Property::NORMALIZATION, 0.5f, 0.0f, 1.0f, true );
    m_properties["maingl2"]->set( Fn::Property::PRIMSIZE, 0.5f, 0.0f, 10.0f, true );
    m_properties["maingl2"]->set( Fn::Property::MINLENGTH, 0.0f, 0.0f, 100.0f, true );
    m_properties["maingl2"]->set( Fn::Property::DRAW_SURFACE, true, true );
    m_properties["maingl2"]->set( Fn::Property::DRAW_GLYPHS, true, true );*/
    m_properties.remove("maingl2");
}

DatasetGlyphset::~DatasetGlyphset()
{
    // TODO Auto-generated destructor stub
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
    if ( !properties( target )->get( Fn::Property::ACTIVE ).toBool() )
    {
        return;
    }

    if ( properties( target )->get( Fn::Property::DRAW_SURFACE ).toBool() )
    {
        DatasetSurfaceset::draw( pMatrix, mvMatrix, width, height, renderMode, target );
    }

    int geoSurf = properties( target )->get( Fn::Property::SURFACE ).toInt();
    int geoGlyph = properties( target )->get( Fn::Property::SURFACE_GLYPH_GEOMETRY ).toInt();
    int geoCol = properties( target )->get( Fn::Property::SURFACE_GLYPH_COLOR ).toInt();
    int glyphstyle = properties( target )->get( Fn::Property::GLYPHSTYLE ).toInt();
     
    float threshold = properties( target )->get( Fn::Property::THRESHOLD ).toFloat();
    float minlength = properties( target )->get( Fn::Property::MINLENGTH ).toFloat();
    
    //TODO: How do we get this to work properly again?
    glEnable( GL_BLEND );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_POINT_SMOOTH );
    glPointSize( properties( target )->get( Fn::Property::PRIMSIZE ).toFloat() );
    glLineWidth( properties( target )->get( Fn::Property::PRIMSIZE ).toFloat() );

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

    if ( properties( target )->get( Fn::Property::DRAW_GLYPHS ).toBool() )
    {
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
}

void DatasetGlyphset::makeCons()
{
    qDebug() << "making consArray: " << minthresh;
    //if (consArray) delete[] consArray;
    int geo = m_properties["maingl"]->get( Fn::Property::SURFACE ).toInt();
    int glyph = m_properties["maingl"]->get( Fn::Property::SURFACE_GLYPH_GEOMETRY ).toInt();
    int col = m_properties["maingl"]->get( Fn::Property::SURFACE_GLYPH_COLOR ).toInt();

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
            if ( conn[i][j] > minthresh )
                ++consNumber;
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
            if ( v > minthresh )
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
    int geo = m_properties["maingl"]->get( Fn::Property::SURFACE ).toInt();
    int glyph = m_properties["maingl"]->get( Fn::Property::SURFACE_GLYPH_GEOMETRY ).toInt();
    int col = m_properties["maingl"]->get( Fn::Property::SURFACE_GLYPH_COLOR ).toInt();

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
            if ( conn[i][j] > minthresh )
                ++vecsNumber;
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
            if ( v > minthresh )
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

    float minlength = m_properties["maingl"]->get( Fn::Property::MINLENGTH ).toFloat();
    float threshold = m_properties["maingl"]->get( Fn::Property::THRESHOLD ).toFloat();
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

    int geo = m_properties["maingl"]->get( Fn::Property::SURFACE ).toInt();
    //int glyph = m_properties["maingl"]->get( Fn::Property::SURFACE_GLYPH_GEOMETRY ).toInt();
    int col = m_properties["maingl"]->get( Fn::Property::SURFACE_GLYPH_COLOR ).toInt();

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
            if ( conn[i][j] > threshold )
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
    int geo = m_properties["maingl"]->get( Fn::Property::SURFACE ).toInt();
    n = m_mesh.at( geo )->numVerts();
    float* nodes = m_mesh.at( geo )->getVertices();

    float thresh = m_properties["maingl"]->get( Fn::Property::THRESHOLD ).toFloat();
    float minlength = m_properties["maingl"]->get( Fn::Property::MINLENGTH ).toFloat();
    Connections* cons = new Connections();
    for ( int i = 0; i < n; ++i )
    {
        for ( int j = i + 1; j < n; ++j )
        {
            float v = conn[i][j];
            if ( v > thresh )
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
    m_properties["maingl"]->set( Fn::Property::SURFACE_GLYPH_GEOMETRY, m_displayList, 0, true );
    m_properties["maingl"]->set( Fn::Property::SURFACE_GLYPH_COLOR, m_displayList, 0, true );
}

