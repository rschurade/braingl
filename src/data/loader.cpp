/*
 * Loader.cpp
 *
 * Created on: May 3, 2012
 * @author Ralph Schurade
 */
#include "loader.h"
#include "loadernifti.h"
#include "loadervtk.h"
#include "loaderfreesurfer.h"

#include "datasets/datasetglyphset.h"
#include "datasets/datasetcons.h"
#include "datasets/datasetmeshtimeseries.h"
#include "datasets/datasettree.h"
#include "datasets/datasetmesh.h"
#include "datasets/datasetfibers.h"

#include "mesh/trianglemesh2.h"

#include <QDebug>
#include <QDataStream>
#include <QVector3D>
#include <QtGui>

#include <cmath>

Loader::Loader( Dataset* selected ) :
    m_header( 0 ),
    m_datasetType( Fn::DatasetType::UNKNOWN ),
    m_selectedDataset( selected ),
    m_success( false )
{
}

Loader::~Loader()
{
    m_dataset.clear();
    m_dataset.squeeze();
}

void Loader::setFilename( QDir fn )
{
    m_fileName = fn;
}

bool Loader::succes()
{
    return m_success;
}

Dataset* Loader::getDataset( int id )
{
    return m_dataset[id];
}

int Loader::getNumDatasets()
{
    return m_dataset.size();
}

bool Loader::load()
{
    QString fn = m_fileName.path();

    if ( fn.endsWith( ".nii.gz" ) || fn.endsWith( ".nii" ) || fn.endsWith( ".hdr" ) || fn.endsWith( ".ima" ) || fn.endsWith( ".img" ) )
    {

        return loadNifti();
    }

    if ( m_fileName.path().endsWith( ".fib" ) )
    {
        return loadVTK();
    }

    if ( m_fileName.path().endsWith( ".tck" ) )
    {
        return loadMRtrix();
    }

    if ( m_fileName.path().endsWith( ".vtk" ) )
    {
        return loadVTK();
    }

    if ( m_fileName.path().endsWith( ".asc" ) )
    {
        //TODO: Deal with offsets: Check if orig is always the same size?
        return loadASC( QVector3D( 128, 128, 128 ) );
    }

    if ( m_fileName.path().endsWith( ".set" ) )
    {
        return loadSet();
    }

    if ( m_fileName.path().endsWith( ".glyphset" ) )
    {
        return loadGlyphset();
    }

    if ( m_fileName.path().endsWith( ".cons" ) || m_fileName.path().endsWith( ".cxls" ) )
    {
        return loadCons();
    }

    if ( m_fileName.path().endsWith( ".meg" ) )
    {
        return loadMEG();
    }

    if ( m_fileName.path().endsWith( ".dtree" ) )
    {
        return loadTree();
    }

    if ( m_fileName.path().endsWith( ".rgb" ) )
    {
        return loadRGB();
    }

    if ( m_fileName.path().endsWith( ".1D" ) )
    {
        return load1D();
    }

    return false;
}

bool Loader::loadNifti()
{
    LoaderNifti* ln = new LoaderNifti( m_fileName );

    if( ln->load() )
    {
        m_dataset = ln->getDataset();
        m_datasetType = ln->getDatasetType();
        delete ln;
        return true;
    }
    else
    {
        return false;
    }
}



bool Loader::loadVTK()
{
    QString fn = m_fileName.path();

    LoaderVTK* lv = new LoaderVTK( fn );

    if ( !lv->load() )
    {
        QStringList status = lv->getStatus();
        for ( int i = 0; i < status.size(); ++i )
        {
            qDebug() << status[i];
        }
        return false;
    }

    if ( lv->getPrimitiveType() == 1 )
    {
        QVector<float> points = lv->getPoints();
        QVector<int> triangles = lv->getPolys();

        if ( triangles[0] != 3 )
        {
            qDebug() << "*ERROR* " << fn << " can only load triangle polygon data";
            return false;
        }

        int numPoints = lv->getNumPoints();
        int numTriangles = lv->getNumPolys();

        TriangleMesh2* mesh = new TriangleMesh2( numPoints, numTriangles );

        for ( int i = 0; i < numPoints; ++i )
        {
            mesh->addVertex( points[i * 3], points[i * 3 + 1], points[i * 3 + 2] );
        }

        for ( int i = 0; i < numTriangles; ++i )
        {
            mesh->addTriangle( triangles[i * 4 + 1], triangles[i * 4 + 2], triangles[i * 4 + 3] );
        }

        QVector<QVector<float> > values = lv->getPointData();

        float min = std::numeric_limits<float>().max();
        float max = std::numeric_limits<float>().min();

        if ( values.size() > 0 )
        {
            QVector<float> data = values[0];
            if ( data.size() == numPoints )
            {
                for ( int i = 0; i < numPoints; ++i )
                {
                    min = qMin( min, data[i] );
                    max = qMax( max, data[i] );
                    mesh->setVertexData( i, data[i] );
                }
            }
        }

        QVector<unsigned char> colors = lv->getPointColors();
        if ( colors.size() == points.size() )
        {
            for ( int i = 0; i < numPoints; ++i )
            {
                mesh->setVertexColor( i, ( (float)colors[i * 3] ) /255.,
                                         ( (float)colors[i * 3 + 1] ) /255.,
                                         ( (float)colors[i * 3 + 2] ) /255., 1.0 );
            }
        }

        mesh->finalize();
        DatasetMesh* dataset = new DatasetMesh( mesh, fn );
        dataset->properties()->set( Fn::Property::D_MIN, min );
        dataset->properties()->set( Fn::Property::D_MAX, max );
        m_dataset.push_back( dataset );
        delete lv;
        return true;
    }

    if ( lv->getPrimitiveType() == 2 )
    {

        DatasetFibers* dataset = new DatasetFibers( fn, lv );
        m_dataset.push_back( dataset );

        delete lv;
        return true;
    }

    return false;
}

bool Loader::loadASC( QVector3D offset )
{
    QString fn = m_fileName.path();

    LoaderFreesurfer* lf = new LoaderFreesurfer();

    if ( !lf->loadASC( fn ) )
    {
        qDebug() << "unable to load: " << fn;
        return false;
    }

    QVector<float> points = lf->getPoints();
    QVector<int> triangles = lf->getTriangles();
    int numPoints = points.size() / 3;
    int numTriangles = triangles.size() / 3;

    TriangleMesh2* mesh = new TriangleMesh2( numPoints, numTriangles );
    for ( int i = 0; i < numPoints; ++i )
    {
        mesh->addVertex( points[i * 3] + offset.x(), points[i * 3 + 1] + offset.y(), points[i * 3 + 2] + offset.z() );
    }
    for ( int i = 0; i < numTriangles; ++i )
    {
        //TODO: Check orientation change (0,2,1)...
        mesh->addTriangle( triangles[i * 3], triangles[i * 3 + 2], triangles[i * 3 + 1] );
    }
    mesh->finalize();
    DatasetMesh* dataset = new DatasetMesh( mesh, fn );
    m_dataset.push_back( dataset );

    return true;
}

bool Loader::loadSet()
{
    QString fn = m_fileName.path();

    DatasetMesh* dataset = new DatasetMesh( fn, Fn::DatasetType::MESH_BINARY );

    qDebug() << "loading surface set: " << fn;

    QFile setfile( fn );
    if ( !setfile.open( QIODevice::ReadOnly ) )
    {
        qDebug( "set file unreadable" );
    }
    QTextStream ts( &setfile );
    QString nl;
    //TODO: Will windows have a problem with this?
    QString trunk = QFileInfo( fn ).path();
    while ( !ts.atEnd() )
    {
        nl = ts.readLine();
        //For commenting out stuff in the setfiles
        if ( !nl.startsWith( "#" ) )
        {
            QStringList sl = nl.split( " " );
            QString fullname = trunk + QDir::separator() + sl.at( 0 );
            float x = 0;
            float y = 0;
            float z = 0;
            if ( sl.length() > 1 )
                x = sl.at( 1 ).toFloat();
            if ( sl.length() > 2 )
                y = sl.at( 2 ).toFloat();
            if ( sl.length() > 3 )
                z = sl.at( 3 ).toFloat();
            QVector3D s( x, y, z );

            LoaderFreesurfer lf;

            if ( !lf.loadASC( fullname ) )
            {
                qDebug() << "unable to load: " << fn;
                return false;
            }

            QVector<float> points = lf.getPoints();
            QVector<int> triangles = lf.getTriangles();
            int numPoints = points.size() / 3;
            int numTriangles = triangles.size() / 3;

            TriangleMesh2* mesh = new TriangleMesh2( numPoints, numTriangles );
            for ( int i = 0; i < numPoints; ++i )
            {
                mesh->addVertex( points[i * 3] + s.x(), points[i * 3 + 1] + s.y(), points[i * 3 + 2] + s.z() );
            }
            for ( int i = 0; i < numTriangles; ++i )
            {
                //TODO: Check orientation change (0,2,1)...
                mesh->addTriangle( triangles[i * 3], triangles[i * 3 + 2], triangles[i * 3 + 1] );
            }
            mesh->finalize();

            dataset->addMesh( mesh, sl.at( 0 ) );
        }
    }
    qDebug() << "surfaces read...";
    dataset->initProperties();
    dataset->setProperties();
    dataset->finalizeProperties();
    qDebug() << "properties set";
    m_dataset.push_back( dataset );

    return true;
}

bool Loader::loadGlyphset()
{
    QString glyphsetname = m_fileName.path();
    QFile glyphsetfile( glyphsetname );
    if ( !glyphsetfile.open( QIODevice::ReadOnly ) )
    {
        qDebug( "glyphset file unreadable" );
    }
    QTextStream gts( &glyphsetfile );
    //TODO: Will windows have a problem with this?
    QString trunk = QFileInfo( glyphsetname ).path();

    //glyphsetfile has three lines: 1: nifti (skip), 2: surfaceset(s), 3: connectivity matrix

    //1: TODO: skip nifti for now
    QString gnl = gts.readLine();
    qDebug() << "skipping: " << gnl;

    //2: load surfaceset
    gnl = gts.readLine();
    QStringList datasetNames = gnl.split( " " );
    bool two = ( datasetNames.length() > 1 );
    QString datasetName = datasetNames.at( 0 );

    gnl = gts.readLine();
    QStringList sl2 = gnl.split( " " );

    QString connectivityName = trunk + QDir::separator() + sl2.at( 0 );
    float mt = 0.8;
    if ( sl2.length() > 1 )
    {
        mt = sl2.at( 1 ).toFloat();
        qDebug() << "minimum threshold: " << mt;
    }
    else
    {
        qDebug() << "no minimum threshold in glyphset file, default of " << mt << " used.";
    }
    float maxt = 1.0;
    if ( sl2.length() > 2 )
    {
        maxt = sl2.at( 2 ).toFloat();
    }

    DatasetGlyphset* dataset = new DatasetGlyphset( glyphsetname, mt, maxt );

    qDebug() << "loading glyph set: " << datasetName;
    if ( two )
    {
        qDebug() << "...and loading glyph set: " << datasetNames.at( 1 );
        if ( datasetNames.length() > 2 )
        {
            qDebug() << "only two hemispheres supported";
        }
    }

    QFile setfile( trunk + QDir::separator() + datasetName );
    if ( !setfile.open( QIODevice::ReadOnly ) )
    {
        qDebug( "set file unreadable" );
    }
    QTextStream ts( &setfile );
    QString nl;

    QString onl;
    QTextStream* ots;
    QVector<QString> others;
    if ( two )
    {
        QFile othersetfile( trunk + QDir::separator() + datasetNames.at( 1 ) );
        if ( !othersetfile.open( QIODevice::ReadOnly ) )
        {
            qDebug( "second set file unreadable" );
        }
        ots = new QTextStream( &othersetfile );
        qDebug() << "ots initialized";
        while ( !ots->atEnd() )
        {
            onl = ots->readLine();
            others.push_back( onl );
        }

    }

    int k = 0;
    while ( !ts.atEnd() )
    {
        nl = ts.readLine();
        qDebug() << "!" << nl;
        if ( two )
        {
            onl = others.at( k );
            qDebug() << onl;
            k++;
        }

        //For commenting out stuff in the setfiles
        if ( !nl.startsWith( "#" ) )
        {
            QStringList sl = nl.split( " " );
            QString fullname = trunk + QDir::separator() + sl.at( 0 );

            LoaderFreesurfer lf;

            if ( !lf.loadASC( fullname ) )
            {
                qDebug() << "unable to load: " << fullname;
                return false;
            }

            float x = 0;
            float y = 0;
            float z = 0;
            if ( sl.length() > 1 )
                x = sl.at( 1 ).toFloat();
            if ( sl.length() > 2 )
                y = sl.at( 2 ).toFloat();
            if ( sl.length() > 3 )
                z = sl.at( 3 ).toFloat();
            QVector3D s( x, y, z );
            QVector<float> points = lf.getPoints();
            QVector<int> triangles = lf.getTriangles();
            int numPoints = points.size() / 3;
            int numTriangles = triangles.size() / 3;

            int onumPoints = 0;
            int onumTriangles = 0;
            QVector<float> opoints;
            QVector<int> otriangles;
            QVector3D* os = new QVector3D( 0, 0, 0 );
            if ( two )
            {
                QStringList osl;
                osl = onl.split( " " );
                QString ofullname = trunk + QDir::separator() + osl.at( 0 );

                LoaderFreesurfer olf;

                if ( !olf.loadASC( ofullname ) )
                {
                    qDebug() << "unable to load: " << ofullname;
                    return false;
                }
                float ox = 0;
                float oy = 0;
                float oz = 0;

                if ( osl.length() > 1 )
                    ox = osl.at( 1 ).toFloat();
                if ( osl.length() > 2 )
                    oy = osl.at( 2 ).toFloat();
                if ( osl.length() > 3 )
                    oz = osl.at( 3 ).toFloat();

                os = new QVector3D( ox, oy, oz );
                opoints = olf.getPoints();
                otriangles = olf.getTriangles();
                onumPoints = opoints.size() / 3;
                onumTriangles = otriangles.size() / 3;
            }

            TriangleMesh2* mesh = new TriangleMesh2( numPoints + onumPoints, numTriangles + onumTriangles );

            for ( int i = 0; i < numPoints; ++i )
            {
                mesh->addVertex( points[i * 3] + s.x(), points[i * 3 + 1] + s.y(), points[i * 3 + 2] + s.z() );
            }
            for ( int i = 0; i < numTriangles; ++i )
            {
                //TODO: Check orientation change (0,2,1)...
                mesh->addTriangle( triangles[i * 3], triangles[i * 3 + 2], triangles[i * 3 + 1] );
            }

            if ( two )
            {
                dataset->m_tris_middle = numTriangles;
                dataset->m_points_middle = numPoints;
                dataset->m_is_split = true;

                for ( int i = 0; i < onumPoints; ++i )

                {
                    mesh->addVertex( opoints[i * 3] + os->x(), opoints[i * 3 + 1] + os->y(), opoints[i * 3 + 2] + os->z() );
                }
                for ( int i = 0; i < onumTriangles; ++i )
                {
                    //TODO: Check orientation change (0,2,1)...
                    mesh->addTriangle( otriangles[i * 3] + onumPoints, otriangles[i * 3 + 2] + onumPoints, otriangles[i * 3 + 1] + onumPoints );
                }
            }

            mesh->finalize();

            dataset->addMesh( mesh, sl.at( 0 ) );
        }
    }

    //fourth thing on the line: name of roi...
    if ( sl2.length() > 3 )
    {
        QString roiname = trunk + QDir::separator() + sl2.at( 3 );
        qDebug() << "loading ROI: " << roiname;
        dataset->loadROI( roiname );
    }
    else
    {
        qDebug() << "no ROI defined...";
        //ROI = all nodes
        dataset->initROI();
    }

    //3: load connectivity: put this into seperate loader / dataset / here
    qDebug() << "loading connectivity";
    dataset->readConnectivity( connectivityName );

    //TODO: init conn.-crap...
    //dataset->setMinthresh( mt );

    dataset->setProperties();
    dataset->addSecondSurfaceSelector();
    m_dataset.push_back( dataset );

    return true;
}

bool Loader::loadCons()
{
    QString filename = m_fileName.path();
    DatasetCons* dataset = new DatasetCons( filename );
    m_dataset.push_back( dataset );
    return true;
}

bool Loader::loadMEG()
{
    QString fn = m_fileName.path();

    DatasetMeshTimeSeries* dataset = new DatasetMeshTimeSeries( fn, Fn::DatasetType::MESH_TIME_SERIES );

    qDebug() << "loading meg set: " << fn;

    QFile setfile( fn );
    if ( !setfile.open( QIODevice::ReadOnly ) )
    {
        qDebug( "set file unreadable" );
    }
    QTextStream ts( &setfile );
    QString nl;
    //TODO: Will windows have a problem with this?
    QString trunk = QFileInfo( fn ).path();
    while ( !ts.atEnd() )
    {
        nl = ts.readLine();
        //For commenting out stuff in the setfiles
        if ( nl.startsWith( "#meg") )
        {
            QStringList sl = nl.split( " " );
            bool ok;
            int count = sl[1].toInt( &ok );
            if ( ok )
            {
                qDebug() << count << "meg data files in set definition";
                for ( int i = 1; i <= count; ++i )
                {
                    QString numberString = QString::number( i );
                    int nss = numberString.size();
                    for ( int k = 0; k < 3 - nss; ++k )
                    {
                        numberString = "0" + numberString;
                    }
                    QFile dataFile( trunk + QDir::separator() + numberString + ".txt" );
                    if ( !dataFile.open( QIODevice::ReadOnly ) )
                    {
                        qDebug() << "data file unreadable, skipping"  << numberString + ".txt";
                        continue;
                    }
                    QVector<float>data;
                    QTextStream ds( &dataFile );
                    while( !ds.atEnd() )
                    {
                        nl = ds.readLine();
                        float val = nl.toFloat( &ok );
                        if ( ok )
                        {
                            data.push_back( val );
                        }
                        else
                        {
                            break;
                        }
                    }
                    if ( !ok )
                    {
                        qDebug() << "error while reading data file, skipping" << numberString + ".txt";
                        continue;
                    }
                    else
                    {
                        dataset->addData( data );
                    }
                }
            }
            else
            {
                qDebug() << "can't read count meg data files";
            }
        }
        else if ( !nl.startsWith( "#" ) )
        {
            QStringList sl = nl.split( " " );
            QString fullname = trunk + QDir::separator() + sl.at( 0 );
            float x = 0;
            float y = 0;
            float z = 0;
            if ( sl.length() > 1 )
                x = sl.at( 1 ).toFloat();
            if ( sl.length() > 2 )
                y = sl.at( 2 ).toFloat();
            if ( sl.length() > 3 )
                z = sl.at( 3 ).toFloat();
            QVector3D s( x, y, z );

            TriangleMesh2* mesh;
            QVector<float> points;
            QVector<int> triangles;

            if ( fullname.endsWith( ".asc" ) )
            {
                LoaderFreesurfer lf;

                if ( !lf.loadASC( fullname ) )
                {
                    qDebug() << "unable to load: " << fn;
                    return false;
                }

                points = lf.getPoints();
                triangles = lf.getTriangles();

                int numPoints = points.size() / 3;
                int numTriangles = triangles.size() / 3;


                if ( numPoints > 0 && numTriangles > 0 )
                {
                    mesh = new TriangleMesh2( numPoints, numTriangles );
                    for ( int i = 0; i < numPoints; ++i )
                    {
                        mesh->addVertex( points[i * 3] + s.x(), points[i * 3 + 1] + s.y(), points[i * 3 + 2] + s.z() );
                    }
                    for ( int i = 0; i < numTriangles; ++i )
                    {
                        mesh->addTriangle( triangles[i * 3], triangles[i * 3 + 2], triangles[i * 3 + 1] );
                    }
                    mesh->finalize();

                    dataset->addMesh( mesh, sl.at( 0 ) );
                }
            }

            if ( fullname.endsWith( ".vtk" ) )
            {
                LoaderVTK* lv = new LoaderVTK( fullname );

                if ( !lv->load() )
                {
                    qDebug() << lv->getStatus();
                    return false;
                }

                if ( lv->getPrimitiveType() == 1 )
                {
                    points = lv->getPoints();
                    triangles = lv->getPolys();

                    if ( triangles[0] != 3 )
                    {
                        qDebug() << "*ERROR* " << fn << " can only load triangle polygon data";
                        return false;
                    }

                    int numPoints = lv->getNumPoints();
                    int numTriangles = lv->getNumPolys();

                    if ( numPoints > 0 && numTriangles > 0 )
                    {
                        mesh = new TriangleMesh2( numPoints, numTriangles );
                        for ( int i = 0; i < numPoints; ++i )
                        {
                            mesh->addVertex( points[i * 3], points[i * 3 + 1], points[i * 3 + 2] );
                        }

                        for ( int i = 0; i < numTriangles; ++i )
                        {
                            mesh->addTriangle( triangles[i * 4 + 1], triangles[i * 4 + 2], triangles[i * 4 + 3] );
                        }

                        mesh->finalize();

                        dataset->addMesh( mesh, sl.at( 0 ) );
                    }
                }
            }
        }
    }
    dataset->setProperties();
    m_dataset.push_back( dataset );

    return true;
}

bool Loader::loadTree()
{
    QString fn = m_fileName.path();

    DatasetTree* dataset = new DatasetTree( fn );

    qDebug() << "loading tree: " << fn;

    QFile setfile( fn );
    if ( !setfile.open( QIODevice::ReadOnly ) )
    {
        qDebug( "set file unreadable" );
    }
    QTextStream ts( &setfile );
    QString nl;

    QString dims;
    QVector<QString>coordinates;
    QVector<QString>clusters;

    while ( !ts.atEnd() )
    {
        nl = ts.readLine();
        if ( nl.startsWith( "#imagesize" ) )
        {
            dims = ts.readLine();
        }

        if ( nl.startsWith( "#coordinates" ) )
        {
            nl = ts.readLine();
            while( nl != "#endcoordinates" )
            {
                coordinates.push_back( nl );
                nl = ts.readLine();
            }
        }

        if ( nl.startsWith( "#clusters" ) )
        {
            nl = ts.readLine();
            while( nl != "#endclusters" )
            {
                clusters.push_back( nl );
                nl = ts.readLine();
            }
        }
    }
    dataset->importTree( dims, coordinates, clusters );
    m_dataset.push_back( dataset );
    return true;
}

bool Loader::loadRGB()
{
    QFile file( m_fileName.path() );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        return false;
    }
    QTextStream in( &file );

    if ( dynamic_cast<DatasetMesh*>( m_selectedDataset ) )
    {
        DatasetMesh* sds = dynamic_cast<DatasetMesh*>( m_selectedDataset );

        for ( int i = 0; i < sds->getMesh()->numVerts(); i++ )
        {
            float r, g, b;
            in >> r >> g >> b;
            for ( int i2 = 0; i2 < sds->getNumberOfMeshes(); i2++ )
            {
                sds->getMesh( i2 )->setVertexColor( i, r, g, b, 1.0 );
            }
        }
    }
    else
    {
        qDebug() << "selected Dataset not a mesh";
    }

    file.close();

    return true;
}

bool Loader::load1D()
{
    QFile file( m_fileName.path() );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        return false;
    }
    QTextStream in( &file );

    if ( dynamic_cast<DatasetMesh*>( m_selectedDataset ) )
    {
        DatasetMesh* sds = dynamic_cast<DatasetMesh*>( m_selectedDataset );

        for ( int i = 0; i < sds->getMesh()->numVerts(); i++ )
        {
            float v;
            in >> v;
            for ( int m = 0; m < sds->getNumberOfMeshes(); m++ )
            {
                sds->getMesh( m )->setVertexData( i, v );
            }
        }
    }
    else
    {
        qDebug() << "selected Dataset not a mesh";
    }

    file.close();

    return true;
}

bool Loader::loadMRtrix()
{
    QFile file( m_fileName.path() );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qDebug() << "Error opening the file";
    }
    QByteArray qba;

    QDataStream dataStream( &qba, QIODevice::WriteOnly );
    dataStream << file.readAll();
    file.close();

    int pc = 0;
    QString offset;
    for ( int i = 0; i < qba.size(); ++i )
    {
        char cur = qba.at( i );
        QString test;
        if ( cur == 'f' )
        {
            test.append( 'f' );
            test.append( qba.at( i + 1 ) );
            test.append( qba.at( i + 2 ) );
            test.append( qba.at( i + 3 ) );
            test.append( qba.at( i + 4 ) );
            test.append( qba.at( i + 5 ) );
            test.append( qba.at( i + 6 ) );


            if ( test == "file: ." )
            {
                offset.append( qba.at( i + 7 ) );
                offset.append( qba.at( i + 8 ) );
                offset.append( qba.at( i + 9 ) );
                offset.append( qba.at( i + 10 ) );

                qDebug() << "found file: . at " << i << "offset:" << offset.toInt();
                pc = offset.toInt() + 4;
                break;
            }
        }
    }

    float x,y,z;

    QVector<QVector<float> >fibs;
    QVector<float>fib;

    union {
      float f;
      uchar b[4];
    } u;

    while( pc < qba.size() )
    {
        u.b[0] = qba.at( pc++ );
        u.b[1] = qba.at( pc++ );
        u.b[2] = qba.at( pc++ );
        u.b[3] = qba.at( pc++ );
        x = u.f;

        u.b[0] = qba.at( pc++ );
        u.b[1] = qba.at( pc++ );
        u.b[2] = qba.at( pc++ );
        u.b[3] = qba.at( pc++ );
        y = u.f;
        u.b[0] = qba.at( pc++ );
        u.b[1] = qba.at( pc++ );
        u.b[2] = qba.at( pc++ );
        u.b[3] = qba.at( pc++ );
        z = u.f;

        if ( std::isinf( x ) )
        {
            break;
        }

        if ( std::isnan( x ) )
        {
            fibs.push_back( fib );
            fib.clear();
            continue;
        }

        fib.push_back( x );
        fib.push_back( y );
        fib.push_back( z );
    }
    qba.clear();
    qba.squeeze();

    QString fn = m_fileName.path();
    DatasetFibers* dataset = new DatasetFibers( fn, fibs );
    m_dataset.push_back( dataset );

    return true;
}
