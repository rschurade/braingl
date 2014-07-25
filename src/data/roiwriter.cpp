/*
 * roiwriter.cpp
 *
 * Created on: Jul 20, 2014
 * @author Ralph Schurade
 */
#include "roiwriter.h"

#include "roiarea.h"

#include <QBuffer>
#include <QDebug>
#include <QImage>

RoiWriter::RoiWriter( ROIArea* roi, QFileInfo fileName ) :
    m_roi( roi ),
    m_fileName( fileName )
{
}

RoiWriter::~RoiWriter()
{
}

bool RoiWriter::save()
{
    nifti_image* out = createHeader( 1 );

    std::vector<float>* data = m_roi->data();

    out->data = data->data();
    if ( nifti_set_filenames( out, m_fileName.absoluteFilePath().toStdString().c_str(), 0, 1 ) )
    {
        qDebug() << "NIfTI filename Problem" << endl;
        return false;
    }
    setDescrip( out, "braingl_roi" );

    nifti_image_write( out );

    out->data = NULL;
    nifti_image_free( out );
    return true;
}

nifti_image* RoiWriter::createHeader( int dim )
{
    PropertyGroup* props = m_roi->properties();

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

    QList<QVariant> props1 = props->getState();

    QByteArray ba;
    QBuffer writeBuffer( &ba );
    writeBuffer.open( QIODevice::WriteOnly );
    QDataStream ds(&writeBuffer);
    ds << props1;

    writeBuffer.close();

    char* extData = ba.data();
    nifti_add_extension( out, extData, ba.size(), 0 );
    qDebug() << ba.size();

    return out;
}

void RoiWriter::setDescrip( nifti_image* hdr, QString descrip )
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

