/*
 * Loader.cpp
 *
 * Created on: May 3, 2012
 * @author Ralph Schurade
 */
#include "loader.h"
#include "loaderfreesurfer.h"
#include "loadernifti.h"
#include "loadertree.h"
#include "loadervtk.h"

#include "../data/datasets/dataset3d.h"
#include "../data/datasets/datasetcons.h"
#include "../data/datasets/datasetfibers.h"
#include "../data/datasets/datasetglyphset.h"
#include "../data/datasets/datasetmesh.h"
#include "../data/datasets/datasetmeshtimeseries.h"
#include "../data/datasets/datasetscalar.h"
#include "../data/datasets/datasettree.h"

#include "../data/mesh/trianglemesh2.h"

#include "../algos/fib.h"

#include "../gui/widgets/algoStarterWidgets/loadimagewidget.h"
#include "../gui/widgets/controls/sliderwitheditint.h"
#include "../gui/widgets/controls/checkbox.h"

#include <QDebug>
#include <QDataStream>
#include <QVector3D>
#include <QtGui>
#include <QImage>
#include <QJsonDocument>

#include <cmath>

Loader::Loader( Dataset* selected ) :
    m_datasetType( Fn::DatasetType::UNKNOWN ),
    m_selectedDataset( selected ),
    m_success( false )
{
}

Loader::~Loader()
{
    m_dataset.clear();
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

    if ( m_fileName.path().endsWith( ".json" ) )
    {
        return loadJSON();
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

    if ( m_fileName.path().endsWith( ".htree" ) )
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

    if ( m_fileName.path().endsWith( ".png" ) || m_fileName.path().endsWith( ".jpg" ) )
    {
        return loadPNG();
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
        delete ln;
        return false;
    }
}



bool Loader::loadVTK()
{
    QString fn = m_fileName.path();

    LoaderVTK* lv = new LoaderVTK( fn );

    if ( !lv->load() )
    {
        qCritical() << "Couldn't load vtk file";
        QStringList status = lv->getStatus();
        for ( int i = 0; i < status.size(); ++i )
        {
            qCritical() << status[i];
        }
        return false;
    }

    if ( lv->getPrimitiveType() == 1 )
    {
        std::vector<float>* points = lv->getPoints();
        std::vector<int> triangles = lv->getPolys();

        if ( triangles[0] != 3 )
        {
            qCritical() << "*ERROR* " << fn << " can only load triangle polygon data";
            return false;
        }

        unsigned int numPoints = lv->getNumPoints();
        unsigned int numTriangles = lv->getNumPolys();

        TriangleMesh2* mesh = new TriangleMesh2( numPoints, numTriangles );

        for ( unsigned int i = 0; i < numPoints; ++i )
        {
            mesh->addVertex( points->at( i * 3 ), points->at( i * 3 + 1 ), points->at( i * 3 + 2 ) );
        }

        for ( unsigned int i = 0; i < numTriangles; ++i )
        {
            mesh->addTriangle( triangles[i * 4 + 1], triangles[i * 4 + 2], triangles[i * 4 + 3] );
        }

        std::vector<std::vector<float> > values = lv->getPointData();

        float min = std::numeric_limits<float>().max();
        float max = -std::numeric_limits<float>().max();

        if ( values.size() > 0 )
        {
            std::vector<float> data = values[0];
            if ( data.size() == numPoints )
            {
                for ( unsigned int i = 0; i < numPoints; ++i )
                {
                    min = qMin( min, data[i] );
                    max = qMax( max, data[i] );
                    mesh->setVertexData( i, data[i] );
                }
            }
        }

        std::vector<unsigned char> colors = lv->getPointColors();
        if ( colors.size() == points->size() )
        {
            for ( unsigned int i = 0; i < numPoints; ++i )
            {
                mesh->setVertexColor( i, ( (float)colors[i * 3] ) /255.,
                                         ( (float)colors[i * 3 + 1] ) /255.,
                                         ( (float)colors[i * 3 + 2] ) /255., 1.0 );
            }
        }

        mesh->finalize();
        DatasetMesh* dataset = new DatasetMesh( mesh, fn );
        if ( min != max )
        {
            dataset->properties().set( Fn::Property::D_MIN, min );
            dataset->properties().set( Fn::Property::D_MAX, max );
        }
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
        qCritical() << "unable to load: " << fn;
        return false;
    }

    std::vector<float>* points = lf->getPoints();
    std::vector<int> triangles = lf->getTriangles();
    int numPoints = points->size() / 3;
    int numTriangles = triangles.size() / 3;

    TriangleMesh2* mesh = new TriangleMesh2( numPoints, numTriangles );
    for ( int i = 0; i < numPoints; ++i )
    {
        mesh->addVertex( points->at( i * 3 ) + offset.x(), points->at( i * 3 + 1 ) + offset.y(), points->at( i * 3 + 2 ) + offset.z() );
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
        qCritical( "set file unreadable" );
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
                qCritical() << "unable to load: " << fn;
                return false;
            }

            std::vector<float>* points = lf.getPoints();
            std::vector<int> triangles = lf.getTriangles();
            int numPoints = points->size() / 3;
            int numTriangles = triangles.size() / 3;

            TriangleMesh2* mesh = new TriangleMesh2( numPoints, numTriangles );
            for ( int i = 0; i < numPoints; ++i )
            {
                mesh->addVertex( points->at( i * 3 ) + s.x(), points->at( i * 3 + 1 ) + s.y(), points->at( i * 3 + 2 ) + s.z() );
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
    m_dataset.push_back( dataset );

    return true;
}

bool Loader::loadGlyphset()
{
    QString glyphsetname = m_fileName.path();
    QFile glyphsetfile( glyphsetname );
    if ( !glyphsetfile.open( QIODevice::ReadOnly ) )
    {
        qCritical( "glyphset file unreadable" );
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

    QString connectivityName;
    if ( sl2.at( 0 ).startsWith( "http" ) )
    {
        connectivityName = sl2.at( 0 );
    }
    else
    {
        connectivityName = trunk + QDir::separator() + sl2.at( 0 );
    }
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
            qCritical() << "only two hemispheres supported";
        }
    }

    QFile setfile( trunk + QDir::separator() + datasetName );
    if ( !setfile.open( QIODevice::ReadOnly ) )
    {
        qCritical( "set file unreadable" );
    }
    QTextStream ts( &setfile );
    QString nl;

    QString onl;
    QTextStream* ots;
    std::vector<QString> others;
    if ( two )
    {
        QFile othersetfile( trunk + QDir::separator() + datasetNames.at( 1 ) );
        if ( !othersetfile.open( QIODevice::ReadOnly ) )
        {
            qCritical( "second set file unreadable" );
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
                qCritical() << "unable to load: " << fullname;
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
            std::vector<float>* points = lf.getPoints();
            std::vector<int> triangles = lf.getTriangles();
            int numPoints = points->size() / 3;
            int numTriangles = triangles.size() / 3;

            int onumPoints = 0;
            int onumTriangles = 0;
            std::vector<float>* opoints = new std::vector<float>();
            std::vector<int> otriangles;
            QVector3D* os = new QVector3D( 0, 0, 0 );
            LoaderFreesurfer olf;
            if ( two )
            {
                QStringList osl;
                osl = onl.split( " " );
                QString ofullname = trunk + QDir::separator() + osl.at( 0 );

                if ( !olf.loadASC( ofullname ) )
                {
                    qCritical() << "unable to load: " << ofullname;
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
                onumPoints = opoints->size() / 3;
                onumTriangles = otriangles.size() / 3;
            }

            TriangleMesh2* mesh = new TriangleMesh2( numPoints + onumPoints, numTriangles + onumTriangles );

            for ( int i = 0; i < numPoints; ++i )
            {
                mesh->addVertex( points->at( i * 3 ) + s.x(), points->at( i * 3 + 1 ) + s.y(), points->at( i * 3 + 2 ) + s.z() );
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
                    mesh->addVertex( opoints->at( i * 3 ) + os->x(), opoints->at( i * 3 + 1 ) + os->y(), opoints->at( i * 3 + 2 ) + os->z() );
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
    //no roi: initialize all nodes true
    dataset->initROI();
    if ( sl2.length() > 3 )
    {

        QString roiname = trunk + QDir::separator() + sl2.at( 3 );
        qDebug() << "loading ROI: " << roiname;
        dataset->loadROI( roiname, dataset->roi );
        if ( sl2.length() > 4 )
        {
            QString roiname2 = trunk + QDir::separator() + sl2.at( 4 );
            qDebug() << "loading ROI2: " << roiname2;
            dataset->loadROI( roiname2, dataset->roi2 );
        }
    }

    //3: load connectivity
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
        qCritical( "set file unreadable" );
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
                        qCritical() << "data file unreadable, skipping"  << numberString + ".txt";
                        continue;
                    }
                    std::vector<float>data;
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
                        qCritical() << "error while reading data file, skipping" << numberString + ".txt";
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
                qCritical() << "can't read count meg data files";
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
            std::vector<float>* points;
            std::vector<int> triangles;

            if ( fullname.endsWith( ".asc" ) )
            {
                LoaderFreesurfer lf;

                if ( !lf.loadASC( fullname ) )
                {
                    qCritical() << "unable to load: " << fn;
                    return false;
                }

                points = lf.getPoints();
                triangles = lf.getTriangles();

                int numPoints = points->size() / 3;
                int numTriangles = triangles.size() / 3;


                if ( numPoints > 0 && numTriangles > 0 )
                {
                    mesh = new TriangleMesh2( numPoints, numTriangles );
                    for ( int i = 0; i < numPoints; ++i )
                    {
                        mesh->addVertex( points->at( i * 3 ) + s.x(), points->at( i * 3 + 1 ) + s.y(), points->at( i * 3 + 2 ) + s.z() );
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
                    qCritical() << lv->getStatus();
                    return false;
                }

                if ( lv->getPrimitiveType() == 1 )
                {
                    points = lv->getPoints();
                    triangles = lv->getPolys();

                    if ( triangles[0] != 3 )
                    {
                        qCritical() << "*ERROR* " << fn << " can only load triangle polygon data";
                        return false;
                    }

                    int numPoints = lv->getNumPoints();
                    int numTriangles = lv->getNumPolys();

                    if ( numPoints > 0 && numTriangles > 0 )
                    {
                        mesh = new TriangleMesh2( numPoints, numTriangles );
                        for ( int i = 0; i < numPoints; ++i )
                        {
                            mesh->addVertex( points->at( i * 3 ), points->at( i * 3 + 1 ), points->at( i * 3 + 2 ) );
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
    LoaderTree lt( fn );
    if ( lt.load() )
    {
        m_dataset.push_back( lt.getDataset() );
        return true;
    }
    return false;
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

        for ( unsigned int i = 0; i < sds->getMesh()->numVerts(); i++ )
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
        qCritical() << "selected Dataset not a mesh";
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

        for ( unsigned int i = 0; i < sds->getMesh()->numVerts(); i++ )
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
        qCritical() << "selected Dataset not a mesh";
    }

    file.close();

    return true;
}

bool Loader::loadMRtrix()
{
    QFile file( m_fileName.path() );
    if ( !file.open( QIODevice::ReadOnly ) )
    {
        qCritical() << "Error opening the file";
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
                int add = 7;
                while ( qba.at( i + add ) == ' ' )
                {
                    ++add;
                }
                while ( qba.at( i + add ) != '\n' && qba.at( i + add ) != ' ' )
                {
                    offset.append( qba.at( i + add ) );
                    ++add;
                }

                qDebug() << "found file: . at " << i << "offset:" << offset.toInt();
                pc = offset.toInt() + 4;
                break;
            }
        }
    }

    float x,y,z;

    std::vector<Fib> fibs;
    Fib fib;

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
            fib.addDataField();
            continue;
        }

        fib.addVert( x,y ,z );
    }
    qba.clear();
    qba.squeeze();

    QString fn = m_fileName.path();
    QList<QString>dataNames;
    dataNames.push_back( "no data" );
    DatasetFibers* dataset = new DatasetFibers( fn, fibs, dataNames );
    m_dataset.push_back( dataset );

    return true;
}

bool Loader::loadPNG()
{
    QImage img( m_fileName.path() );

    LoadImageWidget widget; // = new LoadImageWidget();
    widget.exec();
    int orient = widget.getOrient();

    int dimX = 0;
    int dimY = 0;
    int dimZ = 0;

    int thickness = widget.m_thickness->getValue();
    int offset = widget.m_offset->getValue();

    std::vector<QVector3D>vectorData;
    std::vector<float>scalarData;

    bool whiteTransp = widget.m_whiteTransparent->checked();

    switch ( orient )
    {
        case 0:
        {
            dimX = img.width();
            dimY = img.height();
            dimZ = widget.m_thickness2->getValue();
            size_t blockSize = dimX * dimY * dimZ;

            int z = offset;

            vectorData.resize( blockSize );
            scalarData.resize( blockSize );

            for ( int i = 0; i < thickness; ++i )
            {
                for ( int y = 0; y < dimY; ++y )
                {
                    for ( int x = 0; x < dimX; ++x )
                    {
                        QRgb c = img.pixel( x, dimY - y - 1 );
                        QColor col =  QColor( c ).toRgb();
                        if ( whiteTransp )
                        {
                            if ( col.red() < 254 && col.green() < 254 && col.blue() < 254 )
                            {
                                vectorData[x + y * dimX + ( z + i ) * dimX * dimY ].setX( col.redF() );
                                vectorData[x + y * dimX + ( z + i ) * dimX * dimY ].setY( col.greenF() );
                                vectorData[x + y * dimX + ( z + i ) * dimX * dimY ].setZ( col.blueF() );
                                scalarData[x + y * dimX + ( z + i ) * dimX * dimY ] = qMax( qMax( col.redF(), col.greenF() ), col.blueF() );
                            }
                        }
                        else
                        {
                            vectorData[x + y * dimX + ( z + i ) * dimX * dimY ].setX( col.redF() );
                            vectorData[x + y * dimX + ( z + i ) * dimX * dimY ].setY( col.greenF() );
                            vectorData[x + y * dimX + ( z + i ) * dimX * dimY ].setZ( col.blueF() );
                            scalarData[x + y * dimX + ( z + i ) * dimX * dimY ] = qMax( qMax( col.redF(), col.greenF() ), col.blueF() );
                        }
                    }
                }
            }
            break;
        }
        case 1:
        {
            dimX = img.width();
            dimY = widget.m_thickness2->getValue();
            dimZ = img.height();
            size_t blockSize = dimX * dimY * dimZ;

            int y = offset;

            vectorData.resize( blockSize );
            scalarData.resize( blockSize );

            for ( int i = 0; i < thickness; ++i )
            {
                for ( int z = 0; z < dimZ; ++z )
                {
                    for ( int x = 0; x < dimX; ++x )
                    {
                        QRgb c = img.pixel( x, dimZ - z - 1 );
                        QColor col =  QColor( c ).toRgb();
                        if ( whiteTransp )
                        {
                            if ( col.red() < 254 && col.green() < 254 && col.blue() < 254 )
                            {
                                vectorData[x + ( y + i ) * dimX + z * dimX * dimY ].setX( col.redF() );
                                vectorData[x + ( y + i ) * dimX + z * dimX * dimY ].setY( col.greenF() );
                                vectorData[x + ( y + i ) * dimX + z * dimX * dimY ].setZ( col.blueF() );
                                scalarData[x + ( y + i ) * dimX + z * dimX * dimY ] = qMax( qMax( col.redF(), col.greenF() ), col.blueF() );
                            }
                        }
                        else
                        {
                            vectorData[x + ( y + i ) * dimX + z * dimX * dimY ].setX( col.redF() );
                            vectorData[x + ( y + i ) * dimX + z * dimX * dimY ].setY( col.greenF() );
                            vectorData[x + ( y + i ) * dimX + z * dimX * dimY ].setZ( col.blueF() );
                            scalarData[x + ( y + i ) * dimX + z * dimX * dimY ] = qMax( qMax( col.redF(), col.greenF() ), col.blueF() );
                        }
                    }
                }
            }

            break;
        }
        case 2:
        {
            dimX = widget.m_thickness2->getValue();
            dimY = img.width();
            dimZ = img.height();
            size_t blockSize = dimX * dimY * dimZ;

            int x = offset;

            vectorData.resize( blockSize );
            scalarData.resize( blockSize );

            for ( int i = 0; i < thickness; ++i )
            {
                for ( int z = 0; z < dimZ; ++z )
                {
                    for ( int y = 0; y < dimY; ++y )
                    {
                        QRgb c = img.pixel( dimY - y - 1, dimZ - z - 1 );
                        QColor col =  QColor( c ).toRgb();
                        if ( whiteTransp )
                        {
                            if ( col.red() < 254 && col.green() < 254 && col.blue() < 254 )
                            {
                                vectorData[( x + i ) + y * dimX + z * dimX * dimY ].setX( col.redF() );
                                vectorData[( x + i ) + y * dimX + z * dimX * dimY ].setY( col.greenF() );
                                vectorData[( x + i ) + y * dimX + z * dimX * dimY ].setZ( col.blueF() );
                                scalarData[( x + i ) + y * dimX + z * dimX * dimY ] = qMax( qMax( col.redF(), col.greenF() ), col.blueF() );
                            }
                        }
                        else
                        {
                            vectorData[( x + i ) + y * dimX + z * dimX * dimY ].setX( col.redF() );
                            vectorData[( x + i ) + y * dimX + z * dimX * dimY ].setY( col.greenF() );
                            vectorData[( x + i ) + y * dimX + z * dimX * dimY ].setZ( col.blueF() );
                            scalarData[( x + i ) + y * dimX + z * dimX * dimY ] = qMax( qMax( col.redF(), col.greenF() ), col.blueF() );
                        }
                    }
                }
            }
            break;
        }
    }

    int dims[8] = { 3, dimX, dimY, dimZ, 3, 1, 1 };
    nifti_image* header = nifti_make_new_nim( dims, NIFTI_TYPE_FLOAT32, 1 );
    header->dx = 1.0;
    header->dy = 1.0;
    header->dz = 1.0;

    Dataset3D* dataset = new Dataset3D( m_fileName.path(), vectorData, header );
    m_dataset.push_back( dataset );

    int dims2[8] = { 3, dimX, dimY, dimZ, 1, 1, 1 };
    nifti_image* header2 = nifti_make_new_nim( dims2, NIFTI_TYPE_FLOAT32, 1 );
    header->dx = 1.0;
    header->dy = 1.0;
    header->dz = 1.0;

    DatasetScalar* dataset2 = new DatasetScalar( m_fileName.path(), scalarData, header2 );
    m_dataset.push_back( dataset2 );

    return true;
}

bool Loader::loadJSON()
{
    QString fn = m_fileName.path();

    QString val;
    QFile file;
    file.setFileName( fn );
    file.open( QIODevice::ReadOnly | QIODevice::Text );
    val = file.readAll();
    file.close();

    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject set = d.object();
    QVariantList jverts = set.value( "vertices" ).toArray().toVariantList();
    QVariantList jcolors = set.value( "colors" ).toArray().toVariantList();
    QVariantList jindices = set.value( "indices" ).toArray().toVariantList();

    std::vector<Fib> fibs;
    Fib fib;

    int pc = 0;

    for ( int i = 0; i < jindices.size(); ++i )
    {
        int length = jindices[i].toInt();
        for ( int k = 0; k < length; ++k )
        {
            float x = jverts[pc++].toFloat();
            float y = jverts[pc++].toFloat();
            float z = jverts[pc++].toFloat();
            fib.addVert( x, y ,z );
        }
        fibs.push_back( fib );
        fib.clear();
        fib.addDataField();
    }

    QList<QString>dataNames;
    dataNames.push_back( "no data" );
    DatasetFibers* dataset = new DatasetFibers( fn, fibs, dataNames );
    m_dataset.push_back( dataset );

    return true;

}
