/*
 * writer.cpp
 *
 *  Created on: Jun 1, 2012
 *      Author: schurade
 */

#include <QtCore/QDebug>

#include "datasets/datasetnifti.h"

#include "writer.h"

Writer::Writer( Dataset* dataset, QString fileName ) :
    m_dataset( dataset ),
    m_fileName( fileName )
{
}

Writer::~Writer()
{
}

bool Writer::save()
{
    switch ( m_dataset->getProperty( "type" ).toInt() )
    {
        case FNDT_NIFTI_SCALAR:
        case FNDT_NIFTI_VECTOR:
        {
            /*
            qDebug() << "writing nifti file";
            DatasetNifti* dsNifti = dynamic_cast<DatasetNifti*>( m_dataset );
            nifti_image* out =  nifti_copy_nim_info( dsNifti->getHeader() );
            out->data = dsNifti->getData();
            if( nifti_set_filenames( out, m_fileName.toStdString().c_str(), 0, 1 ) )
            {
                qDebug() << "NIfTI filename Problem";
            }

            nifti_image_write( out );
            break;
            */
        }
    }
    return true;
}
