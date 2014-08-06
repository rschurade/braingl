/*
 * writer.cpp
 *
 * Created on: Jun 1, 2012
 * @author Ralph Schurade
 */
#include "writer.h"
#include "writervtk.h"

#include "../data/datasets/dataset.h"
#include "../data/datasets/dataset3d.h"
#include "../data/datasets/datasetbingham.h"
#include "../data/datasets/datasetnifti.h"
#include "../data/datasets/datasetdwi.h"
#include "../data/datasets/datasetscalar.h"
#include "../data/datasets/datasettensor.h"
#include "../data/datasets/datasetsh.h"
#include "../data/datasets/datasetmesh.h"
#include "../data/datasets/datasetcons.h"
#include "../data/datasets/datasetcorrelation.h"
#include "../data/datasets/datasetisosurface.h"
#include "../data/datasets/datasetisoline.h"
#include "../data/mesh/trianglemesh2.h"

#include "../algos/fib.h"
#include "../algos/fmath.h"

#include <QBuffer>
#include <QDebug>
#include <QImage>

Writer::Writer( Dataset* dataset, QFileInfo fileName, QString filter ) :
    m_dataset( dataset ),
    m_fileName( fileName ),
    m_filter( filter )
{
}

Writer::~Writer()
{
}

bool Writer::save()
{
    int nx = m_dataset->properties().get( Fn::Property::D_NX ).toInt();
    int ny = m_dataset->properties().get( Fn::Property::D_NY ).toInt();
    int nz = m_dataset->properties().get( Fn::Property::D_NZ ).toInt();

    switch ( (Fn::DatasetType)( m_dataset->properties().get( Fn::Property::D_TYPE ).toInt() ) )
    {
        case Fn::DatasetType::NIFTI_SCALAR:
        {
            nifti_image* out = createHeader( 1 );

            std::vector<float>* data = dynamic_cast<DatasetScalar*>( m_dataset )->getData();

            out->data = data->data();
            if ( nifti_set_filenames( out, m_fileName.absoluteFilePath().toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case Fn::DatasetType::NIFTI_VECTOR:
        {
            std::vector<QVector3D>* data = dynamic_cast<Dataset3D*>( m_dataset )->getData();

            int dim = 3;
            nifti_image* out = createHeader( dim );
            std::vector<float> outData( nx * ny * nz * dim );

            int blockSize = nx * ny * nz;
            setDescrip( out, "fnav2_vec3" );

            for ( int z = 0; z < nz; ++z )
            {
                for ( int y = 0; y < ny; ++y )
                {
                    for ( int x = 0; x < nx; ++x )
                    {
                        QVector3D vData = data->at( x + y * nx + z * nx * ny );

                        outData[( x + y * nx + z * nx * ny )] = vData.x();
                        outData[( x + y * nx + z * nx * ny + blockSize )] = vData.y();
                        outData[( x + y * nx + z * nx * ny + 2 * blockSize )] = vData.z();
                    }
                }
            }
            out->data = &( outData[0] );

            if ( nifti_set_filenames( out, m_fileName.absoluteFilePath().toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case Fn::DatasetType::NIFTI_TENSOR:
        {
            nifti_image* out = createHeader( 6 );

            std::vector<Matrix>* data = dynamic_cast<DatasetTensor*>( m_dataset )->getData();

            std::vector<float> outData( nx * ny * nz * 6 );

            int blockSize = nx * ny * nz;
            setDescrip( out, "fnav2_tensor" );

            for ( int z = 0; z < nz; ++z )
            {
                for ( int y = 0; y < ny; ++y )
                {
                    for ( int x = 0; x < nx; ++x )
                    {
                        Matrix tensor = data->at( x + y * nx + z * nx * ny );
                        outData[( x + y * nx + z * nx * ny )] = tensor( 1, 1 );
                        outData[( x + y * nx + z * nx * ny ) + blockSize] = tensor( 1, 2 );
                        outData[( x + y * nx + z * nx * ny ) + blockSize * 2] = tensor( 1, 3 );
                        outData[( x + y * nx + z * nx * ny ) + blockSize * 3] = tensor( 2, 2 );
                        outData[( x + y * nx + z * nx * ny ) + blockSize * 4] = tensor( 2, 3 );
                        outData[( x + y * nx + z * nx * ny ) + blockSize * 5] = tensor( 3, 3 );
                    }
                }
            }
            out->data = &( outData[0] );

            if ( nifti_set_filenames( out, m_fileName.absoluteFilePath().toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case Fn::DatasetType::NIFTI_SH:
        {
            std::vector<ColumnVector>* data = dynamic_cast<DatasetSH*>( m_dataset )->getData();

            int dim = data->at( 0 ).Nrows();
            nifti_image* out = createHeader( dim );
            std::vector<float> outData( nx * ny * nz * dim );

            int blockSize = nx * ny * nz;
            setDescrip( out, "fnav2_qball" );

            for ( int z = 0; z < nz; ++z )
            {
                for ( int y = 0; y < ny; ++y )
                {
                    for ( int x = 0; x < nx; ++x )
                    {
                        ColumnVector vData = data->at( x + y * nx + z * nx * ny );

                        for ( int i = 0; i < dim; ++i )
                        {
                            outData[( x + y * nx + z * nx * ny + i * blockSize )] = vData( i + 1 );
                        }
                    }
                }
            }
            out->data = &( outData[0] );

            if ( nifti_set_filenames( out, m_fileName.absoluteFilePath().toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case Fn::DatasetType::NIFTI_BINGHAM:
        {
            std::vector<std::vector<float> >* data = dynamic_cast<DatasetBingham*>( m_dataset )->getData();

            int dim = data->at( 0 ).size();
            nifti_image* out = createHeader( dim );
            std::vector<float> outData( nx * ny * nz * dim );

            int blockSize = nx * ny * nz;
            setDescrip( out, "fnav2_bingham" );

            for ( int z = 0; z < nz; ++z )
            {
                for ( int y = 0; y < ny; ++y )
                {
                    for ( int x = 0; x < nx; ++x )
                    {
                        std::vector<float> vData = data->at( x + y * nx + z * nx * ny );

                        for ( int i = 0; i < dim; ++i )
                        {
                            outData[( x + y * nx + z * nx * ny + i * blockSize )] = vData[i];
                        }
                    }
                }
            }
            out->data = &( outData[0] );

            if ( nifti_set_filenames( out, m_fileName.absoluteFilePath().toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case Fn::DatasetType::NIFTI_DWI:
        {
            std::vector<ColumnVector>* data = dynamic_cast<DatasetDWI*>( m_dataset )->getData();
            std::vector<float>* b0data = dynamic_cast<DatasetDWI*>( m_dataset )->getB0Data();

            int dim = data->at( 0 ).Nrows() + 1;
            nifti_image* out = createHeader( dim );
            std::vector<float> outData( nx * ny * nz * dim );

            std::vector<float> bvals = dynamic_cast<DatasetDWI*>( m_dataset )->getBvals();
            std::vector<QVector3D> bvecs = dynamic_cast<DatasetDWI*>( m_dataset )->getBvecs();
            std::vector<float> bvalOut;
            for ( unsigned int i = 0; i < bvals.size(); ++i )
            {
                bvalOut.push_back( bvals[i] );
            }
            for ( unsigned int i = 0; i < bvecs.size(); ++i )
            {
                bvalOut.push_back( bvecs[i].x() );
                bvalOut.push_back( bvecs[i].y() );
                bvalOut.push_back( bvecs[i].z() );
            }
            char* extData = reinterpret_cast<char*>( &bvalOut[0] );
            nifti_add_extension( out, extData, bvalOut.size() * 4, 0 );

            int blockSize = nx * ny * nz;
            setDescrip( out, "braingl_dwi" );

            for ( int i = 0; i < blockSize; ++i )
            {
                outData[i] = b0data->at( i );
            }

            for ( int z = 0; z < nz; ++z )
            {
                for ( int y = 0; y < ny; ++y )
                {
                    for ( int x = 0; x < nx; ++x )
                    {
                        ColumnVector vData = data->at( x + y * nx + z * nx * ny );
                        for ( int i = 1; i < dim; ++i )
                        {
                            outData[( x + y * nx + z * nx * ny + i * blockSize )] = vData( i );
                        }
                    }
                }
            }
            out->data = &( outData[0] );

            if ( nifti_set_filenames( out, m_fileName.absoluteFilePath().toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case Fn::DatasetType::FIBERS:
        {
            if ( m_filter.endsWith( "(*.json)" ) )
            {
                saveFibJson();
            }
            else if ( m_filter.endsWith( "(*.trk)" ) )
            {
                saveFibTrk();
            }
            else
            {
                WriterVTK* vtkWriter = new WriterVTK( m_dataset, m_fileName.absoluteFilePath(), m_filter );
                vtkWriter->save();
            }
        }
            break;
        case Fn::DatasetType::ISO_LINE:
        {
            nifti_image* out = createHeader( 1 );
            addPropsToHeader( out );

            std::vector<float>* data = dynamic_cast<DatasetIsoline*>( m_dataset )->getData();

            out->data = data->data();
            if ( nifti_set_filenames( out, m_fileName.absoluteFilePath().toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem" << endl;
            }
            setDescrip( out, "braingl_isoline" );

            nifti_image_write( out );

            out->data = NULL;
            nifti_image_free( out );
        }
            break;
        case Fn::DatasetType::MESH_ISOSURFACE:
        case Fn::DatasetType::MESH_BINARY:
        case Fn::DatasetType::MESH_TIME_SERIES :
        case Fn::DatasetType::GLYPHSET :
        case Fn::DatasetType::MESH_CORRELATION:
        {
            if ( m_filter.endsWith( "(*.1D)" ) )
            {
                save1D();
            }
            else if ( m_filter.endsWith( "(*.rgb)" ) )
            {
                saveRGB();
            }
            else if ( m_filter.endsWith( "(*.roi)" ) )
            {
                saveROI();
            }
            else if ( m_filter.endsWith( "(*.obj)" ) )
            {
                saveOBJ();
            }
            else if ( m_filter.endsWith( "(*.wrl)" ) )
            {
                saveVRML();
            }
            else if ( m_filter.endsWith( "(*.json)" ) )
            {
                saveMeshJson();
            }
            else if ( m_filter.endsWith( "(*.asc)" ) )
            {
                saveMeshAsc();
            }
            else if ( m_filter.endsWith( "(*.bin)" ) )
            {
                saveBinaryConnectivity();
            }
            else if ( m_filter.endsWith( "(*.nii.gz)" ) )
            {
                nifti_image* out = createHeader( 1 );
                addPropsToHeader( out );

                std::vector<float>* data = dynamic_cast<DatasetIsosurface*>( m_dataset )->getData();

                out->data = data->data();
                if ( nifti_set_filenames( out, m_fileName.absoluteFilePath().toStdString().c_str(), 0, 1 ) )
                {
                    qDebug() << "NIfTI filename Problem" << endl;
                }
                setDescrip( out, "braingl_isosurface" );
                nifti_image_write( out );

                out->data = NULL;
                nifti_image_free( out );
            }
            else
            {
                WriterVTK* vtkWriter = new WriterVTK( m_dataset, m_fileName.absoluteFilePath(), m_filter );
                vtkWriter->save();
            }
            break;
        }
        case Fn::DatasetType::CONS :
        {
            if ( m_filter.endsWith( "(*.cxls)" ) )
            {
                saveConnexels();
            }
            else
            {
                WriterVTK* vtkWriter = new WriterVTK( m_dataset, m_fileName.absoluteFilePath(), m_filter );
                vtkWriter->save();
            }
            break;
        }
        default:
            break;
    }
    return true;
}

nifti_image* Writer::createHeader( int dim )
{
    PropertyGroup* props = &m_dataset->properties();

    nifti_image* out = nifti_simple_init_nim();

    out->nx = props->get( Fn::Property::D_NX ).toInt();
    out->ny = props->get( Fn::Property::D_NY ).toInt();
    out->nz = props->get( Fn::Property::D_NZ ).toInt();

    out->nvox = out->nx * out->ny * out->nz * dim;

    out->dx = props->get( Fn::Property::D_DX ).toFloat();
    out->dy = props->get( Fn::Property::D_DY ).toFloat();
    out->dz = props->get( Fn::Property::D_DZ ).toFloat();

    float ax = props->get( Fn::Property::D_ADJUST_X ).toFloat();
    float ay = props->get( Fn::Property::D_ADJUST_Y ).toFloat();
    float az = props->get( Fn::Property::D_ADJUST_Z ).toFloat();

    out->nifti_type = 1; // 1==NIFTI-1 (1 file)

    out->freq_dim = 1;
    out->phase_dim = 2;
    out->slice_dim = 3;

    out->nt = dim;
    out->nv = 4;
    out->ndim = 4;

    QMatrix4x4 qform;
    QMatrix4x4 sform;

    if ( props->contains( Fn::Property::D_Q_FORM ) )
    {
        qform = props->get( Fn::Property::D_Q_FORM ).value<QMatrix4x4>();
        out->qform_code = 1;
    }
    if ( props->contains( Fn::Property::D_S_FORM ) )
    {
        sform = props->get( Fn::Property::D_S_FORM ).value<QMatrix4x4>();
        out->sform_code = 1;
    }

    for ( size_t i = 0; i < 4; ++i )
    {
        for ( size_t j = 0; j < 4; ++j )
        {
            out->qto_xyz.m[i][j] = qform( i, j );
            out->sto_xyz.m[i][j] = sform( i, j );
        }
    }

    out->qto_xyz.m[0][3] = ax;
    out->qto_xyz.m[1][3] = ay;
    out->qto_xyz.m[2][3] = az;

    {
        float dx, dy, dz;
        nifti_mat44_to_quatern( out->qto_xyz, &( out->quatern_b ), &( out->quatern_c ), &( out->quatern_d ), &( out->qoffset_x ), &( out->qoffset_y ),
                &( out->qoffset_z ), &dx, &dy, &dz, &( out->qfac ) );
    }

    out->qto_ijk = nifti_mat44_inverse( out->qto_xyz );
    out->sto_ijk = nifti_mat44_inverse( out->sto_xyz );

    out->nbyper = 4;
    out->datatype = DT_FLOAT;

    return out;
}

void Writer::addPropsToHeader( nifti_image* header )
{
    PropertyGroup* props = &m_dataset->properties();

    QList<QVariant> props1 = props->getState();

    QByteArray ba;
    QBuffer writeBuffer( &ba );
    writeBuffer.open( QIODevice::WriteOnly );
    QDataStream ds(&writeBuffer);
    ds << props1;

    writeBuffer.close();

    char* extData = ba.data();
    nifti_add_extension( header, extData, ba.size(), 0 );
}

void Writer::setDescrip( nifti_image* hdr, QString descrip )
{
    if ( descrip.length() > 80 )
    {
        qDebug() << "Writer: descrip too long ";
        return;
    }
    for ( int i = 0; i < 80; ++i )
    {
        hdr->descrip[i] = 0;
    }
    for ( int i = 0; i < descrip.length(); ++i )
    {
        hdr->descrip[i] = descrip.at( i ).toLatin1();
    }
}

void Writer::saveRGB()
{
    if ( dynamic_cast<DatasetMesh*>( m_dataset ) )
    {
        DatasetMesh* dsm = dynamic_cast<DatasetMesh*>( m_dataset );
        QFile file( m_fileName.absoluteFilePath() );
        if ( !file.open( QIODevice::WriteOnly ) )
        {
            return;
        }
        QTextStream out( &file );
        float* c = dsm->getMesh()->getVertexColors();
        for ( unsigned int i = 0; i < dsm->getMesh()->numVerts(); ++i )
        {
            out << c[i * 4] << " " << c[i * 4 + 1] << " " << c[i * 4 + 2] << endl;
        }
        file.close();
    }
}

void Writer::save1D()
{
    if ( dynamic_cast<DatasetMesh*>( m_dataset ) )
    {
        DatasetMesh* dsm = dynamic_cast<DatasetMesh*>( m_dataset );
        QFile file( m_fileName.absoluteFilePath() );
        if ( !file.open( QIODevice::WriteOnly ) )
        {
            return;
        }
        QTextStream out( &file );
        for ( unsigned int i = 0; i < dsm->getMesh()->numVerts(); ++i )
        {
            float c = dsm->getMesh()->getVertexData(i);
            out << c << endl;
        }
        file.close();
    }
}

void Writer::saveROI()
{
    if ( dynamic_cast<DatasetMesh*>( m_dataset ) )
    {
        DatasetMesh* dsm = dynamic_cast<DatasetMesh*>( m_dataset );
        QFile file( m_fileName.absoluteFilePath() );
        if ( !file.open( QIODevice::WriteOnly ) )
        {
            return;
        }
        QTextStream out( &file );
        for ( unsigned int i = 0; i < dsm->getMesh()->numVerts(); ++i )
        {
            float c = dsm->getMesh()->getVertexData( i );
            if ( c > 0 )
            {
                out << i << " " << "1" << endl;
            }
        }
        file.close();
    }
}

void Writer::saveConnexels()
{
    if ( dynamic_cast<DatasetCons*>( m_dataset ) )
    {
        DatasetCons* dsc = dynamic_cast<DatasetCons*>( m_dataset );
        QFile file( m_fileName.absoluteFilePath() );
        if ( !file.open( QIODevice::WriteOnly ) )
        {
            return;
        }
        QTextStream out( &file );

        QList<Edge*> connexels = dsc->getCons()->edges;
        for ( int i = 0; i < connexels.size(); i++ )
        {
            Edge* edge = connexels.at( i );
            out << edge->fn.x() << " " << edge->fn.y() << " " << edge->fn.z() << " " << edge->tn.x() << " " << edge->tn.y() << " " << edge->tn.z() << " " << edge->m_value << endl;
        }
        file.close();
    }
}

void Writer::saveOBJ()
{
    if ( dynamic_cast<DatasetMesh*>( m_dataset ) )
    {
        DatasetMesh* dsm = dynamic_cast<DatasetMesh*>( m_dataset );
        QFile objFile( m_fileName.absoluteFilePath() );

        if ( !objFile.open( QIODevice::WriteOnly ) )
        {
            return;
        }
        QTextStream out( &objFile );
        // save obj
        out << "# Alias OBJ Model File" << endl;
        out << "# Exported from brainGL, (c) 2012-2014 Ralph Schurade" << endl;
        out << "# File units = millimeters" << endl;
        out << endl;

        QString mtlFN( m_fileName.absoluteFilePath() );
        mtlFN.replace( mtlFN.size() - 3, 3, "mtl" );
        QFileInfo fi( mtlFN );

        out << "mtllib " << fi.fileName() << endl;
        out << endl;

        out << "g Mesh1 Group1 Model " << endl;
        out << endl;

        out << "# vertices" << endl;

        int numVerts = dsm->getMesh()->numVerts();

        for ( int i = 0; i < numVerts; ++i )
        {
            QVector3D vert = dsm->getMesh()->getVertex( i );
            out << "v " << vert.x() << " " << vert.y() << " " << vert.z() << endl;
        }
        out << endl;

        out << "# vertex normals" << endl;

        for ( int i = 0; i < numVerts; ++i )
        {
            QVector3D normal = dsm->getMesh()->getVertexNormal( i );
            out << "vn " << normal.x() << " " << normal.y() << " " << normal.z() << endl;
        }
        out << endl;

        out << "# triangle faces" << endl;

        int numTris = dsm->getMesh()->numTris();
        int texSize = FMath::pow2roundup( sqrt( numTris * 4 ) );
        float texSizeF =  2 * texSize;
        qDebug() << numTris << texSize << texSizeF;
        QImage* image = new QImage( texSize * 2, texSize * 2, QImage::Format_RGB32 );

        for ( int i = 0; i < numTris; ++i )
        {
            float column = i % texSize;
            float row =  i / texSize;

            Triangle t = dsm->getMesh()->getTriangle2( i );
            float col2 = column * 2;
            float row2 = row * 2;
            image->setPixel( col2    , row2    , dsm->getMesh()->getVertexColor( t.v0 ).rgba() );
            image->setPixel( col2 + 1, row2    , dsm->getMesh()->getVertexColor( t.v2 ).rgba() );
            image->setPixel( col2    , row2 + 1, dsm->getMesh()->getVertexColor( t.v1 ).rgba() );
            image->setPixel( col2 + 1, row2 + 1, dsm->getMesh()->getVertexColor( t.v1 ).rgba() );

            float offset = 0.5 / texSizeF;

            float x = 2 * column / texSizeF + offset;
            float y = 2 * row    / texSizeF + offset;
            out << "vt " << x << " " << ( 1.0 - y ) << endl;

            x = ( 2 * column + 1 ) / texSizeF + offset;
            out << "vt " << x << " " << ( 1.0 - y ) << endl;

            x = ( 2 * column + 1 ) / texSizeF + offset;
            y = ( 2 * row + 1 )    / texSizeF + offset;
            out << "vt " << x << " " << ( 1.0 - y ) << endl;

            out << "f " << t.v0 + 1 << "/" << 3 * i + 1 << "/" << t.v0 + 1 << " "
                        << t.v2 + 1 << "/" << 3 * i + 2 << "/" << t.v2 + 1 << " "
                        << t.v1 + 1 << "/" << 3 * i + 3 << "/" << t.v1 + 1 << endl;
        }

        objFile.close();

        QString texFN( m_fileName.absoluteFilePath() );
        texFN.replace( texFN.size() - 3, 3, "png" );
        QFileInfo tfi( texFN );

        // save mtl
        QFile mtlFile( mtlFN );
        if ( !mtlFile.open( QIODevice::WriteOnly ) )
        {
            return;
        }
        QTextStream out2( &mtlFile );

        out2 << "#" << endl;
        out2 << "## Alias OBJ Material File" << endl;
        out2 << "# Exported from brainGL, (c) 2012-2014 Ralph Schurade" << endl;
        out2 << endl;
        out2 << "newmtl texture" << endl;
        out2 << "Ka 1.000000 1.000000 1.000000" << endl;
        out2 << "Kd 1.000000 1.000000 1.000000" << endl;
        out2 << "Ks 0.000000 0.000000 0.000000" << endl;

        out2 << "map_Kd " << tfi.fileName() << endl;
        out2 << endl;
        out2 << "newmtl ForegroundColor" << endl;
        out2 << "Ka 1.000000 1.000000 1.000000" << endl;
        out2 << "Kd 1.000000 1.000000 1.000000" << endl;
        out2 << "Ks 0.000000 0.000000 0.000000" << endl;
        out2 << endl;

        mtlFile.close();
        // save texture

        image->save(  texFN, "PNG" );
        delete image;
    }
}

void Writer::saveVRML()
{
    /*
    Shape {
      geometry IndexedFaceSet {
         coordIndex  [ 0, 1, 3, -1, 0, 2, 5, -1, ...]
         coord       Coordinate        { point [0.0 5.0 3.0, ...] }
         color       Color             { rgb [ 0.2 0.7 0.8, ...] }
         normal      Normal            { vector [0.0 1.0 0.0, ...] }
         texCoord    TextureCoordinate { point [0 1.0, ...] }
      }
      appearance Appearance { material Material { transparency 1.0 } }
    }
    */
    if ( dynamic_cast<DatasetMesh*>( m_dataset ) )
    {
        DatasetMesh* dsm = dynamic_cast<DatasetMesh*>( m_dataset );
        QFile wrlFile( m_fileName.absoluteFilePath() );
        if ( !wrlFile.open( QIODevice::WriteOnly ) )
        {
            return;
        }
        QTextStream out( &wrlFile );

        out << "#VRML V2.0 utf8" << endl;
        out << "Shape {" << endl;
        out << "  geometry IndexedFaceSet {" << endl;
        out << "    coordIndex  [ ";
        int numTris = dsm->getMesh()->numTris();

        for ( int i = 0; i < numTris - 1; ++i )
        {
            Triangle t = dsm->getMesh()->getTriangle2( i );
            out << t.v0 << ", " << t.v2 << ", " << t.v1 << ", -1, ";// << endl;
        }
        Triangle t = dsm->getMesh()->getTriangle2( numTris - 1 );
        out << t.v0 << ", " << t.v2 << ", " << t.v1 << ", -1 ]" << endl;

        int numVerts = dsm->getMesh()->numVerts();
        out << "    coord     Coordinate { point [ ";
        for ( int i = 0; i < numVerts - 1; ++i )
        {
            QVector3D vert = dsm->getMesh()->getVertex( i );
            out << vert.x() << " " << vert.y() << " " << vert.z() << ", ";// << endl;
        }
        QVector3D vert = dsm->getMesh()->getVertex( numVerts - 1 );
        out << vert.x() << " " << vert.y() << " " << vert.z();// << endl;
        out << " ] }" << endl;

        out << "    color     Color { color [ ";
        for ( int i = 0; i < numVerts - 1; ++i )
        {
            QColor col = dsm->getMesh()->getVertexColor( i );
            out << col.redF() << " " << col.greenF() << " " << col.blueF() << ", ";// << endl;
        }
        QColor col = dsm->getMesh()->getVertexColor( numVerts - 1 );
        out << col.redF() << " " << col.greenF() << " " << col.blueF();// << endl;
        out << " ] }" << endl;

        out << "    normal    Normal { vector [ ";
        for ( int i = 0; i < numVerts - 1; ++i )
        {
            QVector3D vert = dsm->getMesh()->getVertexNormal( i );
            out << vert.x() << " " << vert.y() << " " << vert.z() << ", ";// << endl;
        }
        QVector3D vert2 = dsm->getMesh()->getVertexNormal( numVerts - 1 );
        out << vert2.x() << " " << vert2.y() << " " << vert2.z();// << endl;
        out << " ] }" << endl;


        out << "  }" << endl;
        out << "  appearance Appearance { material Material { transparency 1.0 } }" << endl;
        out << "}";


        wrlFile.close();
    }
}

void Writer::saveMeshJson()
{
    if ( dynamic_cast<DatasetMesh*>( m_dataset ) )
    {
        DatasetMesh* dsm = dynamic_cast<DatasetMesh*>( m_dataset );
        QFile outFile( m_fileName.absoluteFilePath() );
        if ( !outFile.open( QIODevice::WriteOnly ) )
        {
            return;
        }
        QTextStream out( &outFile );

        out << "{" << endl;
        out << "  \"vertices\" :" << endl;
        out << "[";

        int numVerts = dsm->getMesh()->numVerts();

        for ( int i = 0; i < numVerts - 1; ++i )
        {
            QVector3D vert = dsm->getMesh()->getVertex( i );
            out << vert.x() << "," << vert.y() << "," << vert.z() << ",";// << endl;
        }
        QVector3D vert = dsm->getMesh()->getVertex( numVerts - 1 );
        out << vert.x() << "," << vert.y() << "," << vert.z();// << endl;

        out << "]," << endl;

        out << "  \"normals\" :" << endl;
        out << "[";

        for ( int i = 0; i < numVerts - 1; ++i )
        {
            QVector3D vert = dsm->getMesh()->getVertexNormal( i );
            out << vert.x() << "," << vert.y() << "," << vert.z() << ",";// << endl;
        }
        vert = dsm->getMesh()->getVertex( numVerts - 1 );
        out << vert.x() << "," << vert.y() << "," << vert.z();// << endl;

        out << "]," << endl;

        out << "  \"indices\" :" << endl;
        out << "[";

        int numTris = dsm->getMesh()->numTris();
        for ( int i = 0; i < numTris - 1; ++i )
        {
            Triangle t = dsm->getMesh()->getTriangle2( i );
            out << t.v0 << "," << t.v1 << "," << t.v2 << ",";// << endl;
        }
        Triangle t = dsm->getMesh()->getTriangle2( numTris - 1 );
        out << t.v0 << "," << t.v1 << "," << t.v2;

        out << "]," << endl;

        out << "  \"colors\" :" << endl;
        out << "[";
        for ( int i = 0; i < numVerts - 1; ++i )
        {
            QColor col = dsm->getMesh()->getVertexColor( i );
            out << col.redF() << "," << col.greenF() << "," << col.blueF() << ",1,";// << endl;
        }
        QColor col = dsm->getMesh()->getVertexColor( numVerts - 1 );
        out << col.redF() << "," << col.greenF() << "," << col.blueF() << ",1";// << endl;
        out << "]," << endl;

        out << "  \"correction\" : [ 0.0, 0.0, 0.0 ]" << endl;
        out << "}";

        outFile.close();
    }
}

void Writer::saveMeshAsc()
{
    if ( dynamic_cast<DatasetMesh*>( m_dataset ) )
    {
        DatasetMesh* dsm = dynamic_cast<DatasetMesh*>( m_dataset );
        QFile outFile( m_fileName.absoluteFilePath() );
        if ( !outFile.open( QIODevice::WriteOnly ) )
        {
            return;
        }
        QTextStream out( &outFile );

        out << "#!ascii version of " << m_fileName.absoluteFilePath() << endl;

        int numVerts = dsm->getMesh()->numVerts();
        int numTris = dsm->getMesh()->numTris();

        out << numVerts << " " << numTris << endl;

        for ( int i = 0; i < numVerts; ++i )
        {
            QVector3D vert = dsm->getMesh()->getVertex( i );
            out << vert.x() << " " << vert.y() << " " << vert.z() << " 0" << endl;
        }        for ( int i = 0; i < numTris; ++i )
        {
            Triangle t = dsm->getMesh()->getTriangle2( i );
            out << t.v0 << " " << t.v1 << " " << t.v2 << " 0" << endl;
        }
        outFile.close();
    }
}

void Writer::saveBinaryConnectivity()
{
    if ( dynamic_cast<DatasetCorrelation*>( m_dataset ) )
    {
        DatasetCorrelation* dsc = dynamic_cast<DatasetCorrelation*>( m_dataset );
        qDebug() << "saving binary connectivity matrix";
        dsc->saveBinaryMatrix( m_fileName.absoluteFilePath() );
    }
}

void Writer::saveFibJson()
{
    if ( dynamic_cast<DatasetFibers*>( m_dataset ) )
    {
        DatasetFibers* dsf = dynamic_cast<DatasetFibers*>( m_dataset );
        QFile outFile( m_fileName.absoluteFilePath() );
        if ( !outFile.open( QIODevice::WriteOnly ) )
        {
            return;
        }
        QTextStream out( &outFile );

        //unsigned int numVerts = dsf->numVerts();
        unsigned int numLines = dsf->numLines();
        std::vector<Fib>* fibs = dsf->getFibs();
        QVector3D vert;

        out << "{" << endl;
        out << "  \"vertices\" :" << endl;
        out << "[";

        for ( unsigned int i = 0; i < numLines - 1 ; ++i )
        {
            for ( unsigned int k = 0; k < fibs->at( i ).length();++k )
            {
                vert = fibs->at( i )[k];
                out << vert.x() << "," << vert.y() << "," << vert.z() << ",";// << endl;
            }
        }

        for ( unsigned int k = 0; k < fibs->back().length() - 1;++k )
        {
            vert = fibs->back()[k];
            out << vert.x() << "," << vert.y() << "," << vert.z() << ",";// << endl;
        }

        vert = fibs->back().lastVert();
        out << vert.x() << "," << vert.y() << "," << vert.z();// << endl;

        out << "]," << endl;

        out << "  \"normals\" :" << endl;
        out << "[";

        for ( unsigned int i = 0; i < numLines - 1 ; ++i )
        {
            for ( unsigned int k = 0; k < fibs->at( i ).length();++k )
            {
                vert = fibs->at( i ).getTangent( k );
                out << vert.x() << "," << vert.y() << "," << vert.z() << ",";// << endl;
            }
        }

        for ( unsigned int k = 0; k < fibs->back().length() - 1;++k )
        {
            vert = fibs->back().getTangent( k );
            out << vert.x() << "," << vert.y() << "," << vert.z() << ",";// << endl;
        }

        vert = fibs->back().getTangent( fibs->back().length() - 1 );
        out << vert.x() << "," << vert.y() << "," << vert.z() << "]," << endl;

        out << "  \"colors\" :" << endl;
        out << "[";

        for ( unsigned int i = 0; i < numLines - 1 ; ++i )
        {
            QColor col = fibs->at( i ).customColor();
            for ( unsigned int k = 0; k < fibs->at( i ).length();++k )
            {
                out << col.redF() << "," << col.greenF() << "," << col.blueF() << "," << "1.0,";
            }
        }

        QColor col = fibs->back().customColor();
        for ( unsigned int k = 0; k < fibs->back().length() - 1;++k )
        {

            out << col.redF() << "," << col.greenF() << "," << col.blueF() << "," << "1.0,";
        }
        out << col.redF() << "," << col.greenF() << "," << col.blueF() << "," << "1.0]," << endl;;

        out << "  \"indices\" :" << endl;
        out << "[";

        for ( unsigned int i = 0; i < numLines - 1; ++i )
        {
            out << fibs->at( i ).length() << ",";
        }
        out << fibs->back().length() << "]" << endl;

        out << "}";

        outFile.close();
    }
}

void Writer::saveFibTrk()
{
    // http://www.trackvis.org/docs/?subsect=fileformat
    if ( dynamic_cast<DatasetFibers*>( m_dataset ) )
    {
        DatasetFibers* dsf = dynamic_cast<DatasetFibers*>( m_dataset );

        QFile file( m_fileName.absoluteFilePath() );
        file.open( QIODevice::WriteOnly );
        QDataStream out( &file );
        out.setFloatingPointPrecision( QDataStream::SinglePrecision );
        char* s = "TRACK";
        out.writeRawData( s, 6 ); // ID string for track file. The first 5 characters must be "TRACK".
        out << (short) 128; // Dimension of the image volume.
        out << (short) 128;
        out << (short) 128;
        out << (float) 1.0f; // Voxel size of the image volume.
        out << (float) 1.0f;
        out << (float) 1.0f;
        out << (float) 0.0f; // Origin of the image volume. This field is not yet being used by TrackVis. That means the origin is always (0, 0, 0).
        out << (float) 0.0f;
        out << (float) 0.0f;
        out << (short) 0; // Number of scalars saved at each track point (besides x, y and z coordinates).
        for ( int i = 0; i < 200; ++i )
        {
            out << (qint8)0; // Name of each scalar. Can not be longer than 20 characters each. Can only store up to 10 names.
        }
        out << (short) 0; // Number of properties saved at each track.
        for ( int i = 0; i < 200; ++i )
        {
            out << (qint8)0; // Name of each property. Can not be longer than 20 characters each. Can only store up to 10 names.
        }
        for ( int i = 0; i < 16; ++i )
        {
            out << (float)0; // 4x4 matrix for voxel to RAS (crs to xyz) transformation. If vox_to_ras[3][3] is 0, it means the matrix is not recorded. This field is added from version 2.
        }
        for ( int i = 0; i < 444; ++i )
        {
            out << (qint8)0; // Reserved space for future version.
        }
        for ( int i = 0; i < 40; ++i )
        {
            out << (qint8)0; // see documentation in link above
        }
        out << (qint32)dsf->numLines(); // Number of tracks stored in this track file. 0 means the number was NOT stored.
        out << (qint32)2; // Version number. Current version is 2.
        out << (qint32)1000; // Size of the header. Used to determine byte swap. Should be 1000.

        std::vector<Fib>* fibs = dsf->getFibs();

        for ( unsigned int i = 0; i < fibs->size(); ++i )
        {
            Fib fib = fibs->at(i);
            out << (qint32)fib.length();
            for ( unsigned int k = 0; k < fib.length(); ++k )
            {
                QVector3D vert = fib.getVert( k );
                out << (float)vert.x();
                out << (float)vert.y();
                out << (float)vert.z();
            }
        }

        file.close();
    }
}
