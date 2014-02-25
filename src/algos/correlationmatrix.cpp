/*
 * correlationmatrix.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: boettgerj
 */

#include "correlationmatrix.h"

#include <QDebug>
#include <QtCore>
#include <QFile>
#include <QUrl>
#include <QThread>
#include <QApplication>
#include <QDomDocument>
#include <QStringList>
#include <QInputDialog>

#include <cmath>

CorrelationMatrix::CorrelationMatrix( int i ) :
        m_remote( false )
{
    init( i );
}

CorrelationMatrix::CorrelationMatrix( QString filename ) :
        m_remote( false )
{
    m_filename = filename;

    //qDebug() << "m_filename in correlation_matrix" << m_filename;

    if (filename.startsWith("http"))
    {
        //qDebug() << "special remote mode initiated!";
        m_remote = true;

        m_id = QInputDialog::getText(NULL, "ID: ", "ID: " );
        //qDebug() << "ID: " << m_id;

        m_passwd = QInputDialog::getText(NULL, "Password: ", "Password: ");
        //qDebug() << "Password: " << m_passwd;

        networkManager = new QNetworkAccessManager();
        connect( networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(serviceRequestFinished(QNetworkReply*)) );
        loadMetaData();
        init( m_n );
        return;
    }

    m_file = new QFile( filename );
    if ( !m_file->open( QIODevice::ReadOnly ) )
    {
        //qDebug() << "binary connectivity unreadable: " << filename;
    }

    //This assumes a square matrix of float32...
    init( qSqrt( m_file->size() / 4 ) );

    m_instream = new QDataStream( m_file );
    m_instream->setByteOrder( QDataStream::LittleEndian );
    m_instream->setFloatingPointPrecision( QDataStream::SinglePrecision );
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

void CorrelationMatrix::setInitialized(bool b)
{
    for ( int i = 0; i < m_n; i++ )
    {
        m_loaded[i] = b;
    }
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
                if ( !std::isnan( v ) )
                {
                    m_histogram[bin]++;
                }
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
    //qDebug() << "reading binary connectivity between " << m_n << " nodes...";
    m_file->seek( 0 );

    for (int i = 0; i < m_n; ++i)
    {
        load(i);
    }
}

void CorrelationMatrix::save( QString filename )
{
    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly ) )
    {
        //qDebug() << "error writing binary connectivity:" << filename;
    }
    QDataStream outstream( &file );
    outstream.setByteOrder( QDataStream::LittleEndian );
    outstream.setFloatingPointPrecision( QDataStream::SinglePrecision );

    for ( int i = 0; i < m_n; i++ )
    {
        for ( int j = 0; j < m_n; j++ )
        {
            outstream << getValue( i, j );
        }
    }
    file.close();
}

int CorrelationMatrix::getN()
{
    return m_n;
}

void CorrelationMatrix::init( int n )
{
    m_n = n;
    m_loaded = new bool[m_n];
    //qDebug() << "connectivity matrix size: " << m_n;
    m_values = new float*[m_n];

    for ( int i = 0; i < m_n; i++ )
    {
        m_values[i] = new float[i + 1];
        m_loaded[i] = false;
    }
}

void CorrelationMatrix::setValue( int i, int j, float v )
{
    if ( std::isnan( v ) )
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
    if ( m_remote )
    {
        loadRemote( i );
        return;
    }

    qint64 p = (qint64) 4 * (qint64) m_n * (qint64) i;
    bool success = m_file->seek( p );

    delete m_instream;
    m_instream = new QDataStream( m_file );
    m_instream->setByteOrder( QDataStream::LittleEndian );
    m_instream->setFloatingPointPrecision( QDataStream::SinglePrecision );

    if ( !success )
    {
        //qDebug() << "seek failed: " << i << " " << p;
    }

    double v;
    for ( int j = 0; j < m_n; j++ )
    {
        *m_instream >> v;
        setValue( i, j, v );
    }
    m_loaded[i] = true;
}
void CorrelationMatrix::loadMetaData()
{
    QUrl serviceUrl = QUrl( m_filename + "&metadata=true");
    QUrl postData;
    QNetworkRequest request( serviceUrl );
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );
    // HTTP Basic authentication header value: base64(username:password)
    QString concatenated = m_id+":"+m_passwd;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader( "Authorization", headerData.toLocal8Bit() );
    QNetworkReply* reply = networkManager->post( request, postData.encodedQuery() );

    while ( !reply->isFinished() )
    {
        QApplication::processEvents();
    }
    QByteArray array = reply->readAll();
    QDomDocument dom( "dom" );
    dom.setContent( array );
    QDomElement docElem = dom.documentElement();
    //qDebug() << docElem.tagName();    // << " " << docElem.text();

    QDomElement n = docElem.firstChildElement( "Matrix" );
    QDomElement n2 = n.firstChildElement( "MatrixIndicesMap" );
    QDomElement n3 = n2.firstChildElement( "BrainModel" );

    QVector<QDomElement> structures;
    QStringList structure_names;
    for ( ; !n3.isNull(); n3 = n3.nextSiblingElement() )
    {
        QString structure = n3.attribute( "BrainStructure" );
        structure_names << structure;
        structures.push_back(n3);
        //qDebug() << structure;
    }
    //qDebug() << structure_names.size();
    QString item = QInputDialog::getItem( NULL, "Structure", "Structure name", structure_names );

    //qDebug() << "picked item: " << item;
    n3 = n2.firstChildElement( "BrainModel" );
    QDomElement picked = n3;
    for ( ; !n3.isNull(); n3 = n3.nextSiblingElement() )
    {
        QString structure = n3.attribute( "BrainStructure" );
        if ( QString::compare( structure, item ) == 0 )
        {
            picked = n3;
        }
    }
    int offset = picked.attribute( "IndexOffset" ).toInt();
    //int count = picked.attribute( "IndexCount" ).toInt();
    m_n = picked.attribute( "SurfaceNumberOfNodes" ).toInt();

    QDomElement n4 = picked.firstChildElement( "NodeIndices" );
    QStringList sl = n4.text().split( " " );
    //qDebug() << sl.size();
    m_index = new int[m_n];
    for ( int l = 0; l < m_n; ++l)
    {
        m_index[l] = -1;
    }
    for ( int l = 0; l < sl.size(); ++l )
    {
        int v = sl.at( l ).toInt();
        m_index[v] = l + 1 + offset;
        //qDebug() << "setting index: " << v << " to: " << m_index[v];
    }
}

void CorrelationMatrix::loadRemote( int i )
{
    //qDebug() << "loading: " << i << "index: " << m_index[i];
    if ( m_index[i] == -1 )
    {
        for ( int j = 0; j < m_n; j++ )
        {
            setValue( i, j, 0 );
        }
        m_loaded[i] = true;
        return;
    }
    QString id = QString::number( m_index[i] - 1 );
    QUrl serviceUrl = QUrl( m_filename + "&row-index=" + id );
    QUrl postData;
    QNetworkRequest request( serviceUrl );
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );
    // HTTP Basic authentication header value: base64(username:password)
    QString concatenated = m_id+":"+m_passwd;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader( "Authorization", headerData.toLocal8Bit() );
    QNetworkReply* reply = networkManager->post( request, postData.encodedQuery() );
    while ( !reply->isFinished() )
    {
        QApplication::processEvents();
    }
    //qDebug() << "finished";
    QByteArray array = reply->readAll();
    //qDebug() << array.size();

    float values[array.size() / 4];
    QDataStream stream( array );
    stream.setByteOrder( QDataStream::LittleEndian );
    stream.setFloatingPointPrecision( QDataStream::SinglePrecision );
    for ( int k = 0; k < array.size() / 4; ++k )
    {
        stream >> values[k];
        //qDebug() << "i: " << i << " k: " << k << " value: " << values[k];
    }

    for ( int j = 0; j < m_n; j++ )
    {
        setValue( i, j, values[m_index[j]] );
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

void CorrelationMatrix::serviceRequestFinished(QNetworkReply* reply)
{
    //qDebug() << "service request finished!";
}
