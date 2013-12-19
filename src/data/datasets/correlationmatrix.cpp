/*
 * correlationmatrix.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: boettgerj
 */

#include "correlationmatrix.h"

#include "qdebug.h"
#include "qmath.h"

CorrelationMatrix::CorrelationMatrix( int i )
{
    init( i );
}

CorrelationMatrix::CorrelationMatrix( QString filename )
{
    m_filename = filename;

    m_file = new QFile( filename );
    if ( !m_file->open( QIODevice::ReadOnly ) )
    {
        qDebug() << "binary connectivity unreadable: " << filename;
    }

    //This assumes a square matrix of float32...
    init( qSqrt( m_file->size() / 4 ) );

    m_instream = new QDataStream( m_file );
    m_instream->setByteOrder( QDataStream::LittleEndian );
    m_instream->setFloatingPointPrecision( QDataStream::SinglePrecision );

    //loadEverything();

    //qDebug() << "finishedReading";

}

CorrelationMatrix::~CorrelationMatrix()
{
    for ( int i = 0; i < m_n; i++ )
    {
        delete[] m_values[i];
        m_values[i] = NULL;
    }
    delete[] m_values;
    m_values = NULL;

    m_file->close();
}

void CorrelationMatrix::makeHistogram(bool* roi)
{
    m_nbins = 2000;
    m_histogram = new int[m_nbins];
    m_perc_histogram = new float[m_nbins];
    for ( int i = 0; i < m_nbins; i++ )
    {
        m_histogram[i] = 0;
    }
    for ( int i = 0; i < m_n; ++i )
    {
        if ( roi[i] )
        {
            for ( int j = 0; j < m_n; ++j )
            {
                float v = getValue( i, j );
                int bin = qFloor( m_nbins * ( v + 1 ) / 2  );
                if (!isnan(v)) m_histogram[bin]++;
            }
        }
    }
    for (int i = m_nbins-1; i > 0; i--)
    {
        m_histogram[i - 1] += m_histogram[i];
    }

    for ( int i = 0; i < m_nbins; i++ )
    {
        m_perc_histogram[i] = m_histogram[i]/(float)m_histogram[0];
    }
    for ( int i = 0; i < m_nbins; i++ )
    {
        //qDebug() << (2*i/(float)m_nbins)-1 << ": " << m_perc_histogram[i];
    }
}

void CorrelationMatrix::loadEverything()
{
    qDebug() << "reading binary connectivity between " << m_n << " nodes...";
    m_file->seek( 0 );

    /*float v;
    for ( int i = 0; i < m_n; i++ )
    {
        //qDebug() << i;
        m_instream->skipRawData( 4 * i );
        for ( int j = i; j < m_n; j++ )
        {
            *m_instream >> v;
            //setValue( i, j, v );
            m_values[j][i] = v;
            //qDebug() << i << j << getValue(i,j);
        }
        m_loaded[i] = true;
    }*/

    for (int i = 0; i < m_n; ++i)
    {
        load(i);
    }
}

int CorrelationMatrix::getN()
{
    return m_n;
}

void CorrelationMatrix::init( int n )
{
    m_n = n;
    m_loaded = new bool[m_n];
    qDebug() << "connectivity matrix size: " << m_n;
    m_values = new float*[m_n];

    for ( int i = 0; i < m_n; i++ )
    {
        m_values[i] = new float[i + 1];
        m_loaded[i] = false;
    }
}

void CorrelationMatrix::setValue( int i, int j, float v )
{
    if ( isnan( v ) )
    {
        //v = 0;
        //qDebug() << i << " " << j;
    }
    //qDebug() << "setValue: " << i << " " << j << " " << v;
    if ( i > j )
    {
        m_values[i][j] = v;
    }
    else
    {
        m_values[j][i] = v;
    }
}

float CorrelationMatrix::getValue( int i, int j )
{
    if ( !m_loaded[i] )
    {
        load( i );
        //qDebug() << "loading: " << i;
    }
    //qDebug() << "getValue: " << i << " " << j;
    if ( i > j )
    {
        return m_values[i][j];
    }
    else
    {
        return m_values[j][i];
    }
}

void CorrelationMatrix::load( int i )
{
    qint64 p = (qint64)4 * (qint64)m_n * (qint64)i;
    bool success = m_file->seek( p );

    delete m_instream;
    m_instream = new QDataStream( m_file );
    m_instream->setByteOrder( QDataStream::LittleEndian );
    m_instream->setFloatingPointPrecision( QDataStream::SinglePrecision );

    if ( !success )
    {
        qDebug() << "seek failed: " << i << " " << p;
    }

    double v;
    for ( int j = 0; j < m_n; j++ )
    {
        *m_instream >> v;
        setValue( i, j, v );
    }
    m_loaded[i] = true;
}

float CorrelationMatrix::percFromThresh( float t )
{
    int bin = qFloor( m_nbins * ( t + 1 ) / 2 );
    //qDebug() << bin << " " << m_perc_histogram[bin];
    return m_perc_histogram[bin];
}

float CorrelationMatrix::threshFromPerc( float p )
{
    int bin = 0;
    for ( int i = 0; i < m_nbins; ++i )
    {
        if ( m_perc_histogram[i] > p )
        {
            bin = i;
        }
    }
    float thr = ( 2 * bin / (float) m_nbins ) - 1;
    //qDebug() << "bin: " << bin << " thr: " << thr;
    return thr;
}
