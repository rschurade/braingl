/*
 * correlationmatrix.cpp
 *
 *  Created on: Dec 18, 2013
 *      Author: boettgerj
 */

#include "correlationmatrix.h"

#include <QApplication>
#include <QtCore>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QHttpMultiPart>
#include <QInputDialog>
#include <QStringList>
#include <QThread>
#include <QUrl>
#include <QUrlQuery>

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

    if (filename.startsWith("http"))
    {
        m_remote = true;

        m_id = QInputDialog::getText(NULL, "ID: ", "ID: " );
        m_passwd = QInputDialog::getText(NULL, "Password: ", "Password: ");
        networkManager = new QNetworkAccessManager();
        connect( networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(serviceRequestFinished(QNetworkReply*)) );
        loadMetaData();
        init( m_n );
        return;
    }

    m_file = new QFile( filename );
    if ( !m_file->open( QIODevice::ReadOnly ) )
    {
        qCritical() << "binary connectivity unreadable: " << filename;
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
}

void CorrelationMatrix::loadEverything()
{
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
        qCritical() << "error writing binary connectivity:" << filename;
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
        v = 0;
        qCritical() << "CorrelationMatrix v isNAN:" << i << " " << j;
    }
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
    }

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
        qCritical() << "seek failed: " << i << " " << p;
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
    QHttpMultiPart *multiPart = new QHttpMultiPart( QHttpMultiPart::FormDataType );
    QNetworkRequest request( serviceUrl );
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );
    // HTTP Basic authentication header value: base64(username:password)
    QString concatenated = m_id + ":" + m_passwd;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader( "Authorization", headerData.toLocal8Bit() );
    QNetworkReply* reply = networkManager->post( request, multiPart );

    while ( !reply->isFinished() )
    {
        QApplication::processEvents();
    }
    QByteArray array = reply->readAll();
    QDomDocument dom( "dom" );
    dom.setContent( array );
    QDomElement docElem = dom.documentElement();

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
    }
    QString item = QInputDialog::getItem( NULL, "Structure", "Structure name", structure_names );

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

    m_index = new int[m_n];
    for ( int l = 0; l < m_n; ++l)
    {
        m_index[l] = -1;
    }
    for ( int l = 0; l < sl.size(); ++l )
    {
        int v = sl.at( l ).toInt();
        m_index[v] = l + 1 + offset;
    }
}

void CorrelationMatrix::loadRemote( int i )
{
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
    QHttpMultiPart *multiPart = new QHttpMultiPart( QHttpMultiPart::FormDataType );
    QNetworkRequest request( serviceUrl );
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded" );
    // HTTP Basic authentication header value: base64(username:password)
    QString concatenated = m_id+":"+m_passwd;
    QByteArray data = concatenated.toLocal8Bit().toBase64();
    QString headerData = "Basic " + data;
    request.setRawHeader( "Authorization", headerData.toLocal8Bit() );
    QNetworkReply* reply = networkManager->post( request, multiPart );
    while ( !reply->isFinished() )
    {
        QApplication::processEvents();
    }
    QByteArray array = reply->readAll();

    std::vector<float> values( array.size() / 4 );
    QDataStream stream( array );
    stream.setByteOrder( QDataStream::LittleEndian );
    stream.setFloatingPointPrecision( QDataStream::SinglePrecision );
    for ( int k = 0; k < array.size() / 4; ++k )
    {
        stream >> values[k];
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
    return thr;
}

void CorrelationMatrix::serviceRequestFinished(QNetworkReply* reply)
{
}
