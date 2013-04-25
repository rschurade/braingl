/*
 * datasetglyphset.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: boettgerj
 */

#include "datasetglyphset.h"

#include "../mesh/trianglemesh2.h"

#include "qfile.h"

#include "qmath.h"

DatasetGlyphset::DatasetGlyphset( QDir filename ) :
        DatasetSurfaceset( filename ), m_prenderer( NULL ), m_vrenderer( NULL ), prevGeo( -1 ), prevGlyph( -1 ), prevCol( -1 ), prevGlyphstyle( -1 )
{
    m_properties.set( Fn::Property::THRESHOLD, minthresh, minthresh, 1.0f, true );
    m_properties.set( Fn::Property::GLYPHSTYLE,
    { "points", "vectors", "pies" }, 0, true ); //0 = points, 1 = vectors, 2 = pies
    m_properties.set( Fn::Property::GLYPHRADIUS, 0.01f, 0.0f, 0.5f, true );
    m_properties.set( Fn::Property::NORMALIZATION, 0.5f, 0.0f, 1.0f, true );
    m_properties.set( Fn::Property::PRIMSIZE, 0.5f, 0.0f, 10.0f, true );
    m_properties.set( Fn::Property::MINLENGTH, 0.0f, 0.0f, 100.0f, true );
    m_properties.set( Fn::Property::DRAW_SURFACE, true, true );
    m_properties.set( Fn::Property::DRAW_GLYPHS, true, true );
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
    minthresh = mt;
}

void DatasetGlyphset::draw( QMatrix4x4 pMatrix, QMatrix4x4 mvMatrix, int width, int height, int renderMode )
{
    qDebug() << "glyphset draw";

    if ( m_properties.get( Fn::Property::DRAW_SURFACE ).toBool() )
        DatasetSurfaceset::draw( pMatrix, mvMatrix, width, height, renderMode );

    int geoSurf = m_properties.get( Fn::Property::SURFACE ).toInt();
    int geoGlyph = m_properties.get( Fn::Property::SURFACE_GLYPH_GEOMETRY ).toInt();
    int geoCol = m_properties.get( Fn::Property::SURFACE_GLYPH_COLOR ).toInt();
    int glyphstyle = m_properties.get( Fn::Property::GLYPHSTYLE ).toInt();

    //TODO: How do we get this to work properly again?
    glEnable( GL_BLEND );
    glShadeModel( GL_SMOOTH );
    glEnable( GL_POINT_SMOOTH );
    glPointSize( m_properties.get( Fn::Property::PRIMSIZE ).toFloat() );

    if ( renderMode == 4 || renderMode == 5 ) // we are drawing opaque objects
    {
        //TODO: Make transparency right, using other rendermodes, adapt shaders?

        if ( ( glyphstyle == 0 )
                && ( ( m_prenderer == 0 ) || ( prevGeo != geoSurf ) || ( prevGlyph != geoGlyph ) || ( prevCol != geoCol )
                        || ( prevGlyphstyle != glyphstyle ) ) )
        {
            if ( m_prenderer )
                delete m_prenderer;
            m_prenderer = new PointGlyphRenderer();
            m_prenderer->init();
            makeCons();
            m_prenderer->initGeometry( consArray, consNumber );
        }

        if ( ( glyphstyle == 1 )
                && ( m_vrenderer == 0 || ( prevGeo != geoSurf ) || ( prevGlyph != geoGlyph ) || ( prevCol != geoCol )
                        || ( prevGlyphstyle != glyphstyle ) ) )
        {
            if ( m_vrenderer )
                delete m_vrenderer;
            m_vrenderer = new VectorGlyphRenderer();
            m_vrenderer->init();
            //TODO: vector fields:
            makeVecs();
            m_vrenderer->initGeometry( vecsArray, vecsNumber );
        }

        //TODO: Deal with multiple passes / switching btw. styles etc.:
        //TODO: Optimize switching of stuff
        prevGeo = geoSurf;
        prevGlyph = geoGlyph;
        prevCol = geoCol;
        prevGlyphstyle = glyphstyle;

        qDebug() << "glyphset predraw";

        if ( m_properties.get( Fn::Property::DRAW_GLYPHS ).toBool() )
        {
            if ( glyphstyle == 0 )
            {
                m_prenderer->draw( pMatrix, mvMatrix, width, height, renderMode, &m_properties );
            }
            if ( glyphstyle == 1 )
            {
                m_vrenderer->draw( pMatrix, mvMatrix, width, height, renderMode, &m_properties );
            }
        }

        qDebug() << "glyphset postdraw";
    }
}

void DatasetGlyphset::makeCons()
{
    qDebug() << "making consArray: " << minthresh;
    //if (consArray) delete[] consArray;
    int geo = m_properties.get( Fn::Property::SURFACE ).toInt();
    int glyph = m_properties.get( Fn::Property::SURFACE_GLYPH_GEOMETRY ).toInt();
    int col = m_properties.get( Fn::Property::SURFACE_GLYPH_COLOR ).toInt();

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
    int geo = m_properties.get( Fn::Property::SURFACE ).toInt();
    int glyph = m_properties.get( Fn::Property::SURFACE_GLYPH_GEOMETRY ).toInt();
    int col = m_properties.get( Fn::Property::SURFACE_GLYPH_COLOR ).toInt();

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

void DatasetGlyphset::setProperties()
{
    DatasetSurfaceset::setProperties();
    m_properties.set( Fn::Property::SURFACE_GLYPH_GEOMETRY, m_displayList, 0, true );
    m_properties.set( Fn::Property::SURFACE_GLYPH_COLOR, m_displayList, 0, true );
}
