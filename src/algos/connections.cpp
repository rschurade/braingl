/*
 * connections.cpp
 *
 *  Created on: May 1, 2013
 *      Author: user
 */

#include "connections.h"

#include <QtCore/QCoreApplication> //TODO: remove?
#include <qfile.h>
#include <qtextstream.h>
#include <QtDebug>
#include <QStringList>
#include "qmath.h"
#include "fib.h"

#include "../gui/gl/glfunctions.h"

Connections::Connections()
{
    params();
    prefix = "test";
}

Connections::~Connections()
{
    // TODO Auto-generated destructor stub
}

Connections::Connections( QString nname, QString ename )
{
    params();
    prefix = nname;
    QFile n( nname );
    qDebug() << nname;
    if ( !n.open( QIODevice::ReadOnly ) )
        qDebug( "nodes unreadable" );
    QTextStream ns( &n );
    QString nl;

    while ( !ns.atEnd() )
    {
        nl = ns.readLine();

        QStringList vals = nl.split( " ", QString::SkipEmptyParts );
        QVector3D* anode;
        //x,y,z
        anode = new QVector3D( ( (QString) ( vals.at( 0 ) ) ).toFloat(), ( (QString) ( vals.at( 1 ) ) ).toFloat(),
                ( (QString) ( vals.at( 2 ) ) ).toFloat() );
        qDebug() << anode->x() << anode->y() << anode->z();
        nodes << *anode;

    }
    n.close();
    qDebug() << "nodes read";

    QFile e( ename );
    if ( !e.open( QIODevice::ReadOnly ) )
        qDebug( "edges unreadable" );
    QTextStream es( &e );
    QString el;
    while ( !es.atEnd() )
    {
        int f;
        int t;
        el = es.readLine();

        QStringList evals = el.split( " ", QString::SkipEmptyParts );
        f = ( (QString) ( evals.at( 0 ) ) ).toInt();
        t = ( (QString) ( evals.at( 1 ) ) ).toInt();

        Edge* aedge;
        aedge = new Edge( nodes.at( f ), nodes.at( t ) );
        qDebug() << f << t;
        edges << aedge;
    }
    e.close();

    qDebug() << edges.length() << " edges...";
}

Connections::Connections( QString fib )
{
    params();
    prefix = fib;
    if ( fib.endsWith( ".fib" ) || fib.endsWith( ".cons" ) )
    {
        loadFib( fib );
    }
    else if ( fib.endsWith( ".cxls" ) )
    {
        loadConnexels( fib );
    }
}

void Connections::loadConnexels( QString filename )
{
//load textfile with px py pz qx qy qz value:
    QFile n( filename );
    if ( !n.open( QIODevice::ReadOnly ) )
    {
        qDebug() << "connexel file unreadable: " << filename;
    }
    QTextStream ns( &n );
    QString nl;

    while ( !ns.atEnd() )
    {
        nl = ns.readLine();
        QRegExp rx( "(\\ |\\t)" );
        QStringList vals = nl.split( rx, QString::SkipEmptyParts );
        if ( vals.size() == 7 )
        {
            QVector3D* anode1;
            QVector3D* anode2;
            //x,y,z
            anode1 = new QVector3D( ( (QString) ( vals.at( 0 ) ) ).toFloat(), ( (QString) ( vals.at( 1 ) ) ).toFloat(),
                    ( (QString) ( vals.at( 2 ) ) ).toFloat() );

            anode2 = new QVector3D( ( (QString) ( vals.at( 3 ) ) ).toFloat(), ( (QString) ( vals.at( 4 ) ) ).toFloat(),
                    ( (QString) ( vals.at( 5 ) ) ).toFloat() );
            float value = ( (QString) ( vals.at( 6 ) ) ).toFloat();
            Edge* aedge;
            aedge = new Edge( *anode1, *anode2, value );
            edges << aedge;
        }
    }
    n.close();
    qDebug() << "connexels read";
}

uint qHash(const QVector3D &v)
{
    return qHash( QString( "%1x%2x%3" ).arg(v.x()).arg(v.y()).arg(v.z()) ) ;
}

void Connections::hashEdges()
{
    for ( int i = 0; i < edges.size(); ++i )
    {
        Edge* e = edges.at( i );
        Edge* antie = new Edge(e->tn, e->fn, e->m_value);
        m_hashed_edges.insert(e->fn,e);
        m_hashed_edges.insert(antie->fn,antie);
    }
    qDebug() << "nodes mapped, " << m_hashed_edges.uniqueKeys().size() << " unique nodes";
}

void Connections::loadFib( QString fib )
{
    QFile n( fib );
    if ( !n.open( QIODevice::ReadOnly ) )
        qDebug() << "vtk unreadable: " << fib;
    QTextStream ns( &n );
    QString nl;
    QDataStream ins( &n );
    ins.setByteOrder( QDataStream::BigEndian );
    ins.setFloatingPointPrecision( QDataStream::SinglePrecision );
    nl = ns.readLine(); //skip first lines;
    nl = ns.readLine(); //TODO: Other types of stuff...
    nl = ns.readLine();
    nl = ns.readLine();
    nl = ns.readLine();
    //ns.pos();
    qDebug() << ns.pos(); //TODO: Das hier sollte nichts ausmachen, tuts aber...
    qDebug() << nl;
    QStringList vals = nl.split( " " );
    float np = ( (QString) ( vals.at( 1 ) ) ).toInt();
    qDebug() << np;
    for ( int i = 0; i < np; i++ )
    {
        QVector3D* anode;
        float x, y, z;
        ins >> x;
        ins >> y;
        ins >> z;
        anode = new QVector3D( x, y, z );
        nodes << *anode;
    }
    qDebug() << ns.pos(); //TODO: WTF, siehe oben?
    ns.seek( n.pos() + np * 3 * 4 + 1 ); //Textstream aufs Zeichen nach den Punkten...
    qDebug() << ns.pos();
    nl = ns.readLine();
    //qDebug() << nl;
    vals = nl.split( " " );
    float ncons = ( (QString) ( vals.at( 1 ) ) ).toInt();

    qDebug() << ns.pos();
    for ( int i = 0; i < ncons; i++ )
    {
        qint32 numpoints;
        ins >> numpoints;
        //qDebug() << numpoints;
        qint32* ps = new qint32[numpoints];
        for ( int pn = 0; pn < numpoints; pn++ )
        {
            ins >> ps[pn];
        }
        Edge* aedge;
        aedge = new Edge( nodes.at( ps[0] ), nodes.at( ps[numpoints - 1] ) );
        aedge->points.removeLast();
        for ( int pn = 1; pn < numpoints; pn++ )
        {
            aedge->points << nodes.at( ps[pn] );
        }
        edges << aedge;
    }
    n.close();
    qDebug() << "nodes read";
}

void Connections::params()
{
    c_thr = 0.8;
    start_i = 10;
    numcycles = 10;
    bell = 5;
    smooth = 3;
}

void Connections::subdivide( int newp )
{
    for ( int i = 0; i < edges.size(); ++i )
    {
        Edge* e = edges.at( i );

        e->subdivide( newp );
    }
}

void Connections::attract()
{
    //for all edges...
    int numThreads = GLFunctions::idealThreadCount;

    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        AttractThread* t = new AttractThread( i, bell, edges, compatibilities );
        m_athreads.push_back(t);
        connect( t, SIGNAL( progress() ), this, SLOT( attractThreadProgress() ), Qt::QueuedConnection );
        connect( t, SIGNAL( finished() ), this, SLOT( attractThreadFinished() ), Qt::QueuedConnection );
    }
    //qDebug() << "start attraction iteration, m_athreads.size: " << m_athreads.size();

    // run threads
    for ( int i = 0; i < numThreads; ++i )
    {
        m_athreads[i]->start();
    }
    for ( int i = 0; i < numThreads; ++i )
    {
        m_athreads[i]->wait();
    }
    //qDebug() << "multithreading finished";

    // delete threads
    for ( unsigned int i = 0; i < m_athreads.size(); ++i )
    {
        delete m_athreads[i];
    }
    m_athreads.clear();

    for ( int e = 0; e < edges.size(); e++ )
    {
        edges.at( e )->applyForces();
    }
    //qDebug() << "iteration finished";
}

void Connections::startBundling()
{
    //TODO: paralellize comps and forces, different threads?
    BundlingThread* t = new BundlingThread( this );
    t->start();
}

void Connections::fullAttract()
{

    qDebug() << "starting full attract";

    qDebug() << "c_thr: " << c_thr << " bell: " << bell << " smooth: " << smooth;

    calcComps();

    double spfac = 1.3;
    double spnow = 1;
    int i = start_i;
    for ( int cycle = 0; cycle < numcycles; cycle++ )
    {
        int sps = qRound( spnow );
        subdivide( sps );
        qDebug() << "starting " << i << " iterations with c_thr:" << c_thr << "segments: " << edges.first()->points.length() - 1;
        for ( int j = 0; j < i; j++ )
        {
            //qDebug() << j;
            attract();
        }
        i--;
        spnow *= spfac;
        emit( progress() );
    }
    //for further subdivision without attraction
    for ( int i = 1; i < smooth; i++ )
    {
        subdivide( qRound( spnow ) + i );
        qDebug() << "number of subd. points: " << qRound( spnow ) + i;
    }
    emit( finished() );
}

void Connections::calcComps()
{
    qDebug() << "calculating compatibilities, edges.size: " << edges.size();
    //comps = new float[edges.size() * edges.size()];

    compatibilities = new Compatibilities(edges.size());

    int numThreads = GLFunctions::idealThreadCount;

    qDebug() << "creating " << numThreads << " threads, m_compthreads.size: " << m_compthreads.size();

    // create threads
    for ( int i = 0; i < numThreads; ++i )
    {
        CompatibilitiesThread* t = new CompatibilitiesThread( i, c_thr, edges, compatibilities );
        m_compthreads.push_back(t);
        connect( t, SIGNAL( progress() ), this, SLOT( compThreadProgress() ), Qt::QueuedConnection );
        connect( t, SIGNAL( finished() ), this, SLOT( compThreadFinished() ), Qt::QueuedConnection );
    }
    //qDebug() << "start calculating compatibilities, m_compthreads.size: " << m_compthreads.size();
    // run threads
    for ( int i = 0; i < numThreads; ++i )
    {
        m_compthreads[i]->start();
    }
    for ( int i = 0; i < numThreads; ++i )
    {
        m_compthreads[i]->wait();
    }

    // delete threads
    for ( unsigned int i = 0; i < m_compthreads.size(); ++i )
    {
        delete m_compthreads[i];
    }
    m_compthreads.clear();

    qDebug() << "comp. calculated";
}

double Connections::vis_c( Edge* ep, Edge* eq )
{
    QVector3D i0 = proj( ep->fn, ep->tn, eq->fn );
    QVector3D i1 = proj( ep->fn, ep->tn, eq->tn );
    QVector3D im = ( i0 + i1 ) / 2;
    QVector3D pm = ( ep->fn + ep->tn ) / 2;

    return qMax( 1 - 2 * ( pm - im ).length() / ( i0 - i1 ).length(), 0.0 );
}

QVector3D Connections::proj( QVector3D a, QVector3D b, QVector3D p )
{
    QVector3D ba = b - a;
    QVector3D pa = p - a;
    return a + ba * QVector3D::dotProduct( ba, pa ) / ba.lengthSquared();
}

void Connections::writeVTK()
{
    qDebug() << "writing file";

    QFile file( "brain_dti.vtk" );
    if ( !file.open( QIODevice::WriteOnly ) )
        qDebug() << "Error opening file for writing";
    QTextStream out( &file );

    int n = edges.size();
    int m = edges.at( 0 )->points.size();

    out << "# vtk DataFile Version 3.0" << endl;
    out << "I am a header! Yay!" << endl;
    out << "ASCII" << endl;
    out << "DATASET POLYDATA" << endl;
    out << "POINTS " << m * n << " float" << endl;

    for ( int e = 0; e < n; e++ )
    {
        Edge* ed = edges.at( e );
        for ( int p = 0; p < ed->points.size(); p++ )
        {
            QVector3D po = ed->points.at( p );
            out << (float) po.x() << " " << (float) po.y() << " " << (float) po.z() << endl;
        }
    }

    out << "LINES " << n << " " << n * ( m + 1 ) << endl;
    int i = 0;
    for ( int e = 0; e < n; e++ )
    {
        out << m;
        for ( int p = 0; p < m; p++ )
        {
            out << " " << i++;
        }
        out << endl;
    }

    file.close();
    qDebug() << "file written";
}

QList<Dataset*> Connections::createDatasetFibers()
{
    int n = edges.size();
    //int m = edges.at( 0 )->points.size();

    std::vector<Fib> fibers;
    for ( int e = 0; e < n; e++ )
    {
        Edge* ed = edges.at( e );
        float v = ed->m_value;
        Fib fib;

        for ( int p = 0; p < ed->points.size(); p++ )
        {
            QVector3D po = ed->points.at( p );
            fib.addVert( po.x(), po.y(), po.z(), v );
        }
        fibers.push_back( fib );

    }

    QList<QString> value_names;
    value_names.push_back( "connectivity" );

    //out << "LINES " << n << " " << n * ( m + 1 ) << endl;

    /*int i = 0;
     std::vector<std::vector<int> > lines;
     for ( int e = 0; e < n; e++ )
     {
     std::vector<int> line;
     for ( int p = 0; p < m; p++ )
     {
     line.push_back( i++ );
     }
     lines.push_back(line);
     }*/

    DatasetFibers* ds = new DatasetFibers( QString( "new" ), fibers, value_names );
    QList<Dataset*> l;
    l.push_back( ds );
    return l;

}

void Connections::writeBinaryVTK()
{
    qDebug() << "writing binary vtk file";
    writeBinaryVTK( name() );
}

void Connections::writeBinaryVTK( QString name )
{

    qDebug() << "writing: " << name << ".fib";
    QFile file( name + ".fib" );
    if ( !file.open( QIODevice::WriteOnly ) )
        qDebug() << "error opening file for writing";
    QDataStream out( &file );
    QTextStream outt( &file );

    out.setByteOrder( QDataStream::BigEndian );
    out.setFloatingPointPrecision( QDataStream::SinglePrecision );

    int n = edges.size();
    int m = edges.at( 0 )->points.size();

    outt << "# vtk DataFile Version 3.0" << endl;
    outt << "I am a header! Yay!" << endl;
    outt << "BINARY" << endl;
    outt << "DATASET POLYDATA" << endl;
    outt << "POINTS " << m * n << " float" << endl;

    for ( int e = 0; e < n; e++ )
    {
        Edge* ed = edges.at( e );
        for ( int p = 0; p < ed->points.size(); p++ )
        {
            QVector3D po = ed->points.at( p );
            out << (float) po.x() << (float) po.y() << (float) po.z();
        }
    }
    outt << endl;

    outt << "LINES " << n << " " << n * ( m + 1 ) << endl;
    int i = 0;
    for ( int e = 0; e < n; e++ )
    {
        out << m;
        for ( int p = 0; p < m; p++ )
        {
            out << i++;
        }
    }
    outt << endl;

    file.close();
    qDebug() << "file written";
}

void Connections::writeSegments()
{
    qDebug() << "writing segments file";

    QFile file( "segments" );
    file.open( QIODevice::WriteOnly );
    QTextStream out( &file );

    int n = edges.size();

    for ( int e = 0; e < n; e++ )
    {
        Edge* ed = edges.at( e );
        for ( int p = 0; p < ed->points.size(); p++ )
        {
            QVector3D po = ed->points.at( p );
            out << (float) po.x() << " " << (float) po.y() << " " << (float) po.z() << " " << e << endl;
        }
    }

    file.close();
    qDebug() << "file written";
}

QString Connections::name()
{
    return prefix + "_c_thr" + QString::number( c_thr, 'f', 4 ) + "_start_i" + QString( "%1" ).arg( start_i, 4, 10, QLatin1Char( '0' ) )
            + "_numcycles" + QString( "%1" ).arg( numcycles, 2, 10, QLatin1Char( '0' ) );
}

void Connections::setCthr( float value, int )
{
    c_thr = value;
}

void Connections::setBell( float value, int )
{
    bell = value;
}

void Connections::setSmooth( int value, int )
{
    smooth = value;
}

void Connections::attractThreadFinished()
{

}

void Connections::attractThreadProgress()
{

}
void Connections::compThreadFinished()
{
    //qDebug() << "compthread finished";
}

void Connections::compThreadProgress()
{
    //qDebug() << "compthread progress";
}
