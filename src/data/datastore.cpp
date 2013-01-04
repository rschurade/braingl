/*
 * datastore.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */
#include <QtCore/QLocale>
#include <QtCore/QDebug>

#include "datasets/datasetscalar.h"
#include "datasets/dataset3d.h"
#include "datasets/datasetdwi.h"
#include "datasets/datasettensor.h"
#include "datasets/datasetsh.h"
#include "loader.h"
#include "writer.h"
#include "vptr.h"
#include "enums.h"

#include "dwialgos.h"
#include "scalaralgos.h"

#include "datastore.h"

DataStore::DataStore()
{
    m_globals["axial"] = 0;
    m_globals["coronal"] = 0;
    m_globals["sagittal"] = 0;
    m_globals["max_axial"] = 1;
    m_globals["max_coronal"] = 1;
    m_globals["max_sagittal"] = 1;
    m_globals["min_axial"] = 0;
    m_globals["min_coronal"] = 0;
    m_globals["min_sagittal"] = 0;
    m_globals["slice_dx"] = 1.0;
    m_globals["slice_dy"] = 1.0;
    m_globals["slice_dz"] = 1.0;
    m_globals["lastPath"] = "";
    m_globals["showAxial"] = true;
    m_globals["showCoronal"] = true;
    m_globals["showSagittal"] = true;

}

DataStore::~DataStore()
{
}

void DataStore::addDataset( Dataset* dataset )
{
    beginInsertRows( QModelIndex(), m_datasetList.size(), m_datasetList.size() );
    m_datasetList.push_back( dataset );
    endInsertRows();

    // TODO dont just use the first dataset
    // check if there is maybe first texture added
    if ( m_datasetList.size() == 1 )
    {
        Dataset* ds = m_datasetList.first();
        int dt = ds->getProperty( "type" ).toInt();
        if ( dt == FNDT_NIFTI_SCALAR || dt == FNDT_NIFTI_VECTOR || dt == FNDT_NIFTI_TENSOR || dt == FNDT_NIFTI_SH || dt == FNDT_NIFTI_DWI || dt == FNDT_NIFTI_BINGHAM )
        {
            m_globals["axial"] = ds->getProperty( "nz" ).toInt() / 2;
            m_globals["coronal"] = ds->getProperty( "ny" ).toInt() / 2;
            m_globals["sagittal"] = ds->getProperty( "nx" ).toInt() / 2;
        }
    }
    updateGlobals();

    emit ( dataChanged( index( m_datasetList.size() - 1, 0 ), index( m_datasetList.size() - 1, 0 ) ) );
}

bool DataStore::load( QDir fileName )
{
    Loader loader( fileName );
    if ( loader.load() )
    {
        for ( int i = 0; i < loader.getNumDatasets(); ++i )
        {
            addDataset( loader.getDataset( i ) );
        }
    }
    return loader.succes();
}

bool DataStore::save( int index, QString fileName )
{
    if ( index >= 0 && index < m_datasetList.size() )
    {
        Writer writer( m_datasetList[index], fileName );
        return writer.save();
    }
    return false;
}

int DataStore::rowCount( const QModelIndex &parent ) const
{
    return m_datasetList.count();
}

int DataStore::columnCount( const QModelIndex &parent ) const
{
    return 14;
}

QVariant DataStore::data( const QModelIndex &index, int role ) const
{
    //qDebug() << "row: " << index.row() << "column: " << index.column() << "role: " << role;
    if ( !index.isValid() )
        return QVariant();

//    if ( index.row() < 0 || index.row() > m_datasetList.size() - 1 )
//        return QVariant();

    switch ( role )
    {
        case Qt::DisplayRole:
            return getDatasetInfo( index );
            break;
        case Qt::UserRole:
            return getGlobal( index );
            break;
        case Qt::EditRole:
            return getDatasetEditables( index );
            break;
        default:
            break;
    }

    return QVariant();
}

QVariant DataStore::getDatasetInfo( const QModelIndex &index ) const
{
    //FN_DATASET_TYPE type = static_cast<FN_DATASET_TYPE>( m_datasetList.at( index.row() )->getProperty( "type" ).toInt() );

    Dataset* ds = dynamic_cast<Dataset*>( m_datasetList.at( index.row() ) );

    switch ( index.column() )
    {
        case FNDSP_NAME:
            return m_datasetList.at( index.row() )->getProperty( "name" ).toString();
            break;
        case FNDSP_DIM:
            return ds->getProperty( "nt" ).toInt();
            break;
        case FNDSP_DATATYPE:
            return getNiftiDataType( ds->getProperty( "datatype" ).toInt() );
            break;
        case FNDSP_SIZE:
            QLocale::setDefault( QLocale( QLocale::English, QLocale::UnitedStates ) );
            return QString( "%L1" ).arg( ds->getProperty( "size" ).toInt() );
            break;
        case FNDSP_NX:
            return ds->getProperty( "nx" ).toInt();
            break;
        case FNDSP_NY:
            return ds->getProperty( "ny" ).toInt();
            break;
        case FNDSP_NZ:
            return ds->getProperty( "nz" ).toInt();
            break;
        case FNDSP_DX:
            return ds->getProperty( "dx" ).toFloat();
            break;
        case FNDSP_DY:
            return ds->getProperty( "dy" ).toFloat();
            break;
        case FNDSP_DZ:
            return ds->getProperty( "dz" ).toFloat();
            break;
        case FNDSP_MIN:
            return ds->getProperty( "min" ).toFloat();
            break;
        case FNDSP_MAX:
            return ds->getProperty( "max" ).toFloat();
            break;
        case FNDSP_TYPE:
            return ds->getProperty( "type" ).toInt();
            break;
        case FNDSP_CREATED_BY:
            return ds->getProperty( "createdBy" ).toInt();
            break;
        default:
            break;
    }

    return QVariant();
}

QVariant DataStore::getDatasetEditables( const QModelIndex &index ) const
{
    Dataset* ds = dynamic_cast<Dataset*>( m_datasetList.at( index.row() ) );

    switch ( index.column() )
    {
        case 0:
            return m_datasetList.at( index.row() )->getProperty( "name" ).toString();
            break;
        case 1:
        {
            return ds->getTextureGLuint();
        }
        case 2:
        {
            return VPtr<Dataset>::asQVariant( ds );
        }
        case FNDSE_LOWER_THRESHOLD:
            return ds->getProperty( "lowerThreshold" ).toFloat();
            break;
        case FNDSE_UPPER_THRESHOLD:
            return ds->getProperty( "upperThreshold" ).toFloat();
            break;
        case FNDSE_COLORMAP:
            return ds->getProperty( "colormap" ).toInt();
            break;
        case FNDSE_INTERPOLATION:
            return ds->getProperty( "interpolation" ).toBool();
            break;
        case FNDSE_ALPHA:
            return ds->getProperty( "alpha" ).toFloat();
            break;
        case FNDSE_ISO_VALUE:
            return ds->getProperty( "isoValue" ).toFloat();
            break;
        case FNDSE_ACTIVE:
            return ds->getProperty( "active" ).toBool();
            break;
        case FNDSE_ORDER:
            return ds->getProperty( "order" ).toInt();
            break;
        case FNDSE_LOD:
            return ds->getProperty( "lod" ).toInt();
            break;
        case FNDSE_SCALING:
            return ds->getProperty( "scaling" ).toFloat();
            break;
        case FNDSE_FA_THRESHOLD:
            return ds->getProperty( "faThreshold" ).toFloat();
            break;
        case FNDSE_EV_THRESHOLD:
            return ds->getProperty( "evThreshold" ).toFloat();
            break;
        case FNDSE_GAMMA:
            return ds->getProperty( "gamma" ).toFloat();
            break;
        case FNDSE_OFFSET:
            return ds->getProperty( "offset" ).toFloat();
            break;
        case FNDSE_RENDER_SLICE:
            return ds->getProperty( "renderSlice" ).toInt();
            break;
        case FNDSE_RENDER_LOWER_X:
            return ds->getProperty( "renderLowerX" ).toInt();
            break;
        case FNDSE_RENDER_UPPER_X:
            return ds->getProperty( "renderUpperX" ).toInt();
            break;
        case FNDSE_RENDER_LOWER_Y:
            return ds->getProperty( "renderLowerY" ).toInt();
            break;
        case FNDSE_RENDER_UPPER_Y:
            return ds->getProperty( "renderUpperY" ).toInt();
            break;
        case FNDSE_RENDER_LOWER_Z:
            return ds->getProperty( "renderLowerZ" ).toInt();
            break;
        case FNDSE_RENDER_UPPER_Z:
            return ds->getProperty( "renderUpperZ" ).toInt();
            break;
        case FNDSE_MINMAX_SCALING:
            return ds->getProperty( "minmaxScaling" ).toBool();
        case FNDSE_BVALUE:
            return ds->getProperty( "bValue" ).toInt();
            break;
        case FNDSE_CALC_LOWER_X:
            return ds->getProperty( "calcLowerX" ).toInt();
            break;
        case FNDSE_CALC_UPPER_X:
            return ds->getProperty( "calcUpperX" ).toInt();
            break;
        case FNDSE_CALC_LOWER_Y:
            return ds->getProperty( "calcLowerY" ).toInt();
            break;
        case FNDSE_CALC_UPPER_Y:
            return ds->getProperty( "calcUpperY" ).toInt();
            break;
        case FNDSE_CALC_LOWER_Z:
            return ds->getProperty( "calcLowerZ" ).toInt();
            break;
        case FNDSE_CALC_UPPER_Z:
            return ds->getProperty( "calcUpperZ" ).toInt();
            break;
        case FNDSE_TENSOR_RENDERMODE:
            return ds->getProperty( "tensorRenderMode" ).toInt();
            break;
    }

    return QVariant();
}

QVariant DataStore::getGlobal( const QModelIndex &index ) const
{
    switch ( index.column() )
    {
        case FNGLOBAL_SAGITTAL:
            return m_globals["sagittal"];
            break;
        case FNGLOBAL_CORONAL:
            return m_globals["coronal"];
            break;
        case FNGLOBAL_AXIAL:
            return m_globals["axial"];
            break;
        case FNGLOBAL_MAX_SAGITTAL:
            return m_globals["max_sagittal"];
            break;
        case FNGLOBAL_MAX_CORONAL:
            return m_globals["max_coronal"];
            break;
        case FNGLOBAL_MAX_AXIAL:
            return m_globals["max_axial"];
            break;
        case FNGLOBAL_SLICE_DX:
            return m_globals["slice_dx"];
            break;
        case FNGLOBAL_SLICE_DY:
            return m_globals["slice_dy"];
            break;
        case FNGLOBAL_SLICE_DZ:
            return m_globals["slice_dz"];
            break;
        case FNGLOBAL_LAST_PATH:
            return m_globals["lastPath"];
            break;
        case FNGLOBAL_SHOW_SAGITTAL:
            return m_globals["showSagittal"];
            break;
        case FNGLOBAL_SHOW_CORONAL:
            return m_globals["showCoronal"];
            break;
        case FNGLOBAL_SHOW_AXIAL:
            return m_globals["showAxial"];
            break;
        case FNGLOBAL_VIEW:
            return m_globals["view"];
            break;
        case FNGLOBAL_MOVEX:
            return m_globals["moveX"];
            break;
        case FNGLOBAL_MOVEY:
            return m_globals["moveY"];
            break;
        case FNGLOBAL_BBX:
            return m_globals["bbX"];
            break;
        case FNGLOBAL_BBY:
            return m_globals["bbY"];
            break;
        case FNGLOBAL_ZOOM:
            return m_globals["zoom"];
            break;
    }
    return QVariant();
}

bool DataStore::setData( const QModelIndex &index, const QVariant &value, int role )
{
    //qDebug() << "row: " << index.row() << "column: " << index.column() << "role: " << role;
    if ( !index.isValid() )
    {
        return false;
    }

    if ( role == Qt::EditRole )
    {
        if ( index.row() >= 0 && index.row() < m_datasetList.size() )
        {
            switch ( index.column() )
            {
                case 0:
                    m_datasetList.at( index.row() )->setProperty( "name", value.toString() );
                    break;

                case FNDSE_LOWER_THRESHOLD:
                    m_datasetList.at( index.row() )->setProperty( "lowerThreshold", value.toFloat() );
                    break;
                case FNDSE_UPPER_THRESHOLD:
                    m_datasetList.at( index.row() )->setProperty( "upperThreshold", value.toFloat() );
                    break;
                case FNDSE_COLORMAP:
                    m_datasetList.at( index.row() )->setProperty( "colormap", value.toInt() );
                    break;
                case FNDSE_INTERPOLATION:
                    m_datasetList.at( index.row() )->setProperty( "interpolation", value.toBool() );
                    break;
                case FNDSE_ALPHA:
                    m_datasetList.at( index.row() )->setProperty( "alpha", value.toFloat() );
                    break;
                case FNDSE_ISO_VALUE:
                    m_datasetList.at( index.row() )->setProperty( "isoValue", value.toFloat() );
                    break;
                case FNDSE_ACTIVE:
                    m_datasetList.at( index.row() )->setProperty( "active", value.toBool() );
                    break;
                case FNDSE_ORDER:
                    m_datasetList.at( index.row() )->setProperty( "order", value.toInt() );
                    break;
                case FNDSE_LOD:
                    m_datasetList.at( index.row() )->setProperty( "lod", value.toInt() );
                    break;
                case FNDSE_SCALING:
                    m_datasetList.at( index.row() )->setProperty( "scaling", value.toFloat() );
                    break;
                case FNDSE_FA_THRESHOLD:
                    m_datasetList.at( index.row() )->setProperty( "faThreshold", value.toFloat() );
                    break;
                case FNDSE_EV_THRESHOLD:
                    m_datasetList.at( index.row() )->setProperty( "evThreshold", value.toFloat() );
                    break;
                case FNDSE_GAMMA:
                    m_datasetList.at( index.row() )->setProperty( "gamma", value.toFloat() );
                    break;
                case FNDSE_OFFSET:
                    m_datasetList.at( index.row() )->setProperty( "offset", value.toFloat() );
                    break;
                case FNDSE_RENDER_SLICE:
                    m_datasetList.at( index.row() )->setProperty( "renderSlice", value.toInt() );
                    break;
                case FNDSE_RENDER_LOWER_X:
                    m_datasetList.at( index.row() )->setProperty( "renderLowerX", value.toInt() );
                    break;
                case FNDSE_RENDER_UPPER_X:
                    m_datasetList.at( index.row() )->setProperty( "renderUpperX", value.toInt() );
                    break;
                case FNDSE_RENDER_LOWER_Y:
                    m_datasetList.at( index.row() )->setProperty( "renderLowerY", value.toInt() );
                    break;
                case FNDSE_RENDER_UPPER_Y:
                    m_datasetList.at( index.row() )->setProperty( "renderUpperY", value.toInt() );
                    break;
                case FNDSE_RENDER_LOWER_Z:
                    m_datasetList.at( index.row() )->setProperty( "renderLowerZ", value.toInt() );
                    break;
                case FNDSE_RENDER_UPPER_Z:
                    m_datasetList.at( index.row() )->setProperty( "renderUpperZ", value.toInt() );
                    break;
                case FNDSE_MINMAX_SCALING:
                    m_datasetList.at( index.row() )->setProperty( "minmaxScaling", value.toBool() );
                    break;
                case FNDSE_BVALUE:
                    m_datasetList.at( index.row() )->setProperty( "bValue", value.toInt() );
                    break;
                case FNDSE_CALC_LOWER_X:
                    m_datasetList.at( index.row() )->setProperty( "calcLowerX", value.toInt() );
                    break;
                case FNDSE_CALC_UPPER_X:
                    m_datasetList.at( index.row() )->setProperty( "calcUpperX", value.toInt() );
                    break;
                case FNDSE_CALC_LOWER_Y:
                    m_datasetList.at( index.row() )->setProperty( "calcLowerY", value.toInt() );
                    break;
                case FNDSE_CALC_UPPER_Y:
                    m_datasetList.at( index.row() )->setProperty( "calcUpperY", value.toInt() );
                    break;
                case FNDSE_CALC_LOWER_Z:
                    m_datasetList.at( index.row() )->setProperty( "calcLowerZ", value.toInt() );
                    break;
                case FNDSE_CALC_UPPER_Z:
                    m_datasetList.at( index.row() )->setProperty( "calcUpperZ", value.toInt() );
                    break;
                case FNDSE_TENSOR_RENDERMODE:
                    m_datasetList.at( index.row() )->setProperty( "tensorRenderMode", value.toInt() );
                    break;
            }
        }
        emit( dataChanged( index, index ) );
        emit( headerDataChanged( Qt::Horizontal, 0, 0 ) );
        return true;
    }

    if ( role == Qt::UserRole && index.column() < 100 )
    {
        int algo = index.column();
        switch ( algo )
        {
            case FNALGO_QBALL4:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_DWI )
                {
                    //addDataset( DWIAlgos::qBall( dynamic_cast<DatasetDWI*>( ds ) ) );
                    addDataset( DWIAlgos::qBallSharp( dynamic_cast<DatasetDWI*>( ds ), 4 ) );
                }
                break;
            }
            case FNALGO_QBALL6:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_DWI )
                {
                    addDataset( DWIAlgos::qBallSharp( dynamic_cast<DatasetDWI*>( ds ), 6 ) );
                }
                break;
            }
            case FNALGO_QBALL8:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_DWI )
                {
                    addDataset( DWIAlgos::qBallSharp( dynamic_cast<DatasetDWI*>( ds ), 8 ) );
                }
                break;
            }
            case FNALGO_TENSORFIT:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_DWI )
                {
                    addDataset( DWIAlgos::tensorFit( dynamic_cast<DatasetDWI*>( ds ) ) );
                }
                break;
            }
            case FNALGO_FA:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_DWI )
                {
                    addDataset( DWIAlgos::calcFAFromDWI( dynamic_cast<DatasetDWI*>( ds ) ) );
                }
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_TENSOR )
                {
                    addDataset( DWIAlgos::calcFAFromTensor( dynamic_cast<DatasetTensor*>( ds ) ) );
                }
                break;
            }
            case FNALGO_EV:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_DWI )
                {
                    QList<Dataset*> ev = DWIAlgos::calcEVFromDWI( dynamic_cast<DatasetDWI*>( ds ) );
                    addDataset( ev[0] );
                    addDataset( ev[1] );
                }
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_TENSOR )
                {
                    QList<Dataset*> ev = DWIAlgos::calcEVFromTensor( dynamic_cast<DatasetTensor*>( ds ) );
                    addDataset( ev[0] );
                    addDataset( ev[1] );
                }
                break;
            }
            case FNALGO_BINGHAM:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_SH )
                {
                    QList<Dataset*> bings = DWIAlgos::fitBingham( dynamic_cast<DatasetSH*>( ds ) );
                    addDataset( bings[0] );
                    addDataset( bings[1] );
                    addDataset( bings[2] );
                    addDataset( bings[3] );
                }
                break;
            }
            case FNALGO_TENSOR_TRACK:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_TENSOR )
                {
                    QList<Dataset*> fibs = DWIAlgos::tensorTrack( dynamic_cast<DatasetTensor*>( ds ) );
                    addDataset( fibs[0] );
                }
                break;
            }
            case FNALGO_ISOSURFACE:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->getProperty( "type" ) == FNDT_NIFTI_SCALAR )
                {
                    QList<Dataset*> isos = ScalarAlgos::isoSurface( dynamic_cast<DatasetScalar*>( ds ) );
                    addDataset( isos[0] );
                }
                break;
            }
        }
    }

    if ( role == Qt::UserRole )
    {
        switch ( index.column() )
        {
            case FNGLOBAL_SAGITTAL:
                m_globals["sagittal"] = value.toInt();
                break;
            case FNGLOBAL_CORONAL:
                m_globals["coronal"] = value.toInt();
                break;
            case FNGLOBAL_AXIAL:
                m_globals["axial"] = value.toInt();
                break;
            case FNGLOBAL_MAX_SAGITTAL:
                m_globals["max_sagittal"] = value.toInt();
                break;
            case FNGLOBAL_MAX_CORONAL:
                m_globals["max_coronal"] = value.toInt();
                break;
            case FNGLOBAL_MAX_AXIAL:
                m_globals["max_axial"] = value.toInt();
                break;
            case FNGLOBAL_SLICE_DX:
                m_globals["slice_dx"] = value.toInt();
                break;
            case FNGLOBAL_SLICE_DY:
                m_globals["slice_dy"] = value.toInt();
                break;
            case FNGLOBAL_SLICE_DZ:
                m_globals["slice_dz"] = value.toInt();
                break;
            case FNGLOBAL_CORONAL_AXIAL:
                m_globals["coronal"] = value.toPoint().x();
                m_globals["axial"] = value.toPoint().y();
                break;
            case FNGLOBAL_SAGITTAL_AXIAL:
                m_globals["sagittal"] = value.toPoint().x();
                m_globals["axial"] = value.toPoint().y();
                break;
            case FNGLOBAL_SAGITTAL_CORONAL:
                m_globals["sagittal"] = value.toPoint().x();
                m_globals["coronal"] = value.toPoint().y();
                break;
            case FNGLOBAL_LAST_PATH:
                m_globals["lastPath"] = value.toString();
                break;
            case FNGLOBAL_SHOW_SAGITTAL:
                m_globals["showSagittal"] = value.toBool();
                break;
            case FNGLOBAL_SHOW_CORONAL:
                m_globals["showCoronal"] = value.toBool();
                break;
            case FNGLOBAL_SHOW_AXIAL:
                m_globals["showAxial"] = value.toBool();
                break;
            case FNGLOBAL_VIEW:
                m_globals["view"] = value.toInt();
                break;
            case FNGLOBAL_ZOOM:
                m_globals["zoom"] = value.toFloat();
                break;
            case FNGLOBAL_MOVEX:
                m_globals["moveX"] = value.toFloat();
                break;
            case FNGLOBAL_MOVEY:
                m_globals["moveY"] = value.toFloat();
                break;
            case FNGLOBAL_BBX:
                m_globals["bbX"] = value.toFloat();
                break;
            case FNGLOBAL_BBY:
                m_globals["bbY"] = value.toFloat();
                break;

        }
        // zoom - bby are updated in the render loop, emiting their changes causes an infinite event loop and seg fault
        if ( index.column() < FNGLOBAL_ZOOM )
        {
            emit( dataChanged( index, index ) );
        }

        return true;
    }

    return false;
}

QVariant DataStore::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if ( role == Qt::DisplayRole )
    {
        if ( orientation == Qt::Horizontal )
        {
            switch ( section )
            {
                case 0:
                    return QString( "name" );
                    break;
                case 1:
                    return QString( "dim" );
                    break;
                case 2:
                    return QString( "data type" );
                    break;
                case 3:
                    return QString( "size in byte" );
                    break;
                case 4:
                    return QString( "nx" );
                    break;
                case 5:
                    return QString( "ny" );
                    break;
                case 6:
                    return QString( "nz" );
                    break;
                case 7:
                    return QString( "dx" );
                    break;
                case 8:
                    return QString( "dy" );
                    break;
                case 9:
                    return QString( "dz" );
                    break;
                case 10:
                    return QString( "min" );
                    break;
                case 11:
                    return QString( "max" );
                    break;
                case 12:
                    return QString( "internal type" );
                    break;
                case 13:
                    return QString( "created by" );
                    break;
            }
        }
        else
        {
            return m_datasetList.at( section )->getProperty( "name" ).toString();
        }
    }
    if ( role == Qt::UserRole )
    {
        if ( orientation == Qt::Horizontal )
        {
            switch ( section )
            {
                case 1:
                    return QString( "GLuint" );
                    break;
                default:
                    break;
            }
        }
    }
    return QVariant();
}

QModelIndex DataStore::index( int row, int column, const QModelIndex & parent ) const
{
    if ( row < m_datasetList.size() )
    {
        return createIndex( row, column, m_datasetList.at( row ) );
    }
    else if ( m_datasetList.size() == 0 && column > 100 )
    {
        return createIndex( row, column, 0 );
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex DataStore::parent( const QModelIndex & index ) const
{
    return QModelIndex();
}

void DataStore::moveItemUp( int row )
{
    if ( row > 0 && row < m_datasetList.size() )
    {
        beginMoveRows( index( row, 0 ), row, row, index( row - 1, 0 ), row - 1 );
        m_datasetList.swap( row, row - 1 );
        endMoveRows();
        updateGlobals();
        emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
    }
}

void DataStore::moveItemDown( int row )
{
    if ( row >= 0 && row < m_datasetList.size() - 1 )
    {
        beginMoveRows( index( row, 0 ), row, row, index( row + 1, 0 ), row + 1 );
        m_datasetList.swap( row, row + 1 );
        endMoveRows();
        updateGlobals();
        emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
    }
}

void DataStore::deleteItem( int row )
{
    if ( row >= 0 && row < m_datasetList.size() )
    {
        beginRemoveRows( index( row, 0 ), row, row );
        Dataset* toDelete = m_datasetList.at( row );
        m_datasetList.removeAt( row );
        endRemoveRows();
        beginResetModel();
        reset();
        endResetModel();

        delete toDelete;
        updateGlobals();
        emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
    }
}

QString DataStore::getNiftiDataType( const int type ) const
{
    switch ( type )
    {
        case 0:
            return QString( "unknown" );
            break;
        case 1:
            return QString( "binary" );
            break;
        case 2:
            return QString( "unsigned char" );
            break;
        case 4:
            return QString( "signed short" );
            break;
        case 8:
            return QString( "signed int" );
            break;
        case 16:
            return QString( "float" );
            break;
        case 32:
            return QString( "complex" );
            break;
        case 64:
            return QString( "double" );
            break;
        case 128:
            return QString( "RGB" );
            break;
        default:
            return QString( "unknown" );
            break;
    }
    return QString( "unknown" );
}

void DataStore::updateGlobals()
{
    updateSliceGlobals();
}

void DataStore::updateSliceGlobals()
{
    if ( m_datasetList.size() > 0 )
    {
        Dataset* ds = m_datasetList.first();
        if ( ds->getProperty( "type" ).toInt() == FNDT_NIFTI_SCALAR )
        {
            m_globals["max_axial"] = ds->getProperty( "nz" ).toInt();
            m_globals["max_coronal"] = ds->getProperty( "ny" ).toInt();
            m_globals["max_sagittal"] = ds->getProperty( "nx" ).toInt();
            m_globals["slice_dx"] = ds->getProperty( "dx" ).toFloat();
            m_globals["slice_dy"] = ds->getProperty( "dy" ).toFloat();
            m_globals["slice_dz"] = ds->getProperty( "dz" ).toFloat();

            emit dataChanged( index( 0, 100 ), index( 0, 108 ) );
        }
    }
}

void DataStore::setGlobal( QString key, QVariant data )
{
    //qDebug() << "set global: " << key << "," << data;
    m_globals[key] = data;
    emit dataChanged( index( 0, 100 ), index( 0, 108 ) );
}
