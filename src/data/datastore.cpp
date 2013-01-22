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
#include "datasets/datasetbingham.h"
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
    connect( dataset->properties(), SIGNAL( signalPropChanged() ), this, SLOT( propChanged() ) );
    endInsertRows();

    updateGlobals();

    emit ( dataChanged( index( m_datasetList.size() - 1, 0 ), index( m_datasetList.size() - 1, 0 ) ) );
}

void DataStore::updateSliceGlobals()
{
    if ( m_datasetList.size() > 0 )
    {
        Dataset* ds = m_datasetList.first();
        if ( ds->properties()->get( FNPROP_TYPE ).toInt() < FNDT_MESH_ASCII )
        {
            m_globals["axial"] = ds->properties()->get( FNPROP_NZ ).toInt() / 2;
            m_globals["coronal"] = ds->properties()->get( FNPROP_NY ).toInt() / 2;
            m_globals["sagittal"] = ds->properties()->get( FNPROP_NX ).toInt() / 2;
            m_globals["max_axial"] = ds->properties()->get( FNPROP_NZ ).toInt();
            m_globals["max_coronal"] = ds->properties()->get( FNPROP_NY ).toInt();
            m_globals["max_sagittal"] = ds->properties()->get( FNPROP_NX ).toInt();
            m_globals["slice_dx"] = ds->properties()->get( FNPROP_DX ).toFloat();
            m_globals["slice_dy"] = ds->properties()->get( FNPROP_DY ).toFloat();
            m_globals["slice_dz"] = ds->properties()->get( FNPROP_DZ ).toFloat();

            emit dataChanged( index( 0, 100 ), index( 0, 108 ) );
        }
    }
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
    return m_datasetList.size();
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
    Dataset* ds = dynamic_cast<Dataset*>( m_datasetList.at( index.row() ) );

    switch ( index.column() )
    {
        case FNPROP_NAME:
            return m_datasetList.at( index.row() )->properties()->get( FNPROP_NAME );
            break;
        case FNPROP_DIM:
            return ds->properties()->get( FNPROP_DIM );
            break;
        case FNPROP_DATATYPE:
            return getNiftiDataType( ds->properties()->get( FNPROP_DATATYPE ).toInt() );
            break;
        case FNPROP_SIZE:
            QLocale::setDefault( QLocale( QLocale::English, QLocale::UnitedStates ) );
            return QString( "%L1" ).arg( ds->properties()->get( FNPROP_SIZE ).toInt() );
            break;
        case FNPROP_NX:
            return ds->properties()->get( FNPROP_NX );
            break;
        case FNPROP_NY:
            return ds->properties()->get( FNPROP_NY );
            break;
        case FNPROP_NZ:
            return ds->properties()->get( FNPROP_NZ );
            break;
        case FNPROP_DX:
            return ds->properties()->get( FNPROP_DX );
            break;
        case FNPROP_DY:
            return ds->properties()->get( FNPROP_DY );
            break;
        case FNPROP_DZ:
            return ds->properties()->get( FNPROP_DZ );
            break;
        case FNPROP_MIN:
            return ds->properties()->get( FNPROP_MIN );
            break;
        case FNPROP_MAX:
            return ds->properties()->get( FNPROP_MAX );
            break;
        case FNPROP_TYPE:
            return ds->properties()->get( FNPROP_TYPE );
            break;
        case FNPROP_CREATED_BY:
            return ds->properties()->get( FNPROP_CREATED_BY );
            break;
        default:
            break;
    }

    return QVariant();
}

QVariant DataStore::getDatasetEditables( const QModelIndex &index ) const
{
    Dataset* ds = dynamic_cast<Dataset*>( m_datasetList.at( index.row() ) );

    if ( index.column() < FNPROP_TEXTURE_GLUINT )
    {
        return ds->properties()->get( (FN_PROPERTY)index.column() );
    }

    switch ( index.column() )
    {
        case FNPROP_TEXTURE_GLUINT:
            return ds->getTextureGLuint();
            break;
        case FNPROP_DATASET_POINTER:
            return VPtr<Dataset>::asQVariant( ds );
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
            m_datasetList.at( index.row() )->properties()->set( (FN_PROPERTY)index.column(), value );
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
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
                {
                    //addDataset( DWIAlgos::qBall( dynamic_cast<DatasetDWI*>( ds ) ) );
                    addDataset( DWIAlgos::qBallSharp( dynamic_cast<DatasetDWI*>( ds ), 4 ) );
                }
                break;
            }
            case FNALGO_QBALL6:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
                {
                    addDataset( DWIAlgos::qBallSharp( dynamic_cast<DatasetDWI*>( ds ), 6 ) );
                }
                break;
            }
            case FNALGO_QBALL8:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
                {
                    addDataset( DWIAlgos::qBallSharp( dynamic_cast<DatasetDWI*>( ds ), 8 ) );
                }
                break;
            }
            case FNALGO_TENSORFIT:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
                {
                    addDataset( DWIAlgos::tensorFit( dynamic_cast<DatasetDWI*>( ds ) ) );
                }
                break;
            }
            case FNALGO_FA:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
                {
                    addDataset( DWIAlgos::calcFAFromDWI( dynamic_cast<DatasetDWI*>( ds ) ) );
                }
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_TENSOR )
                {
                    addDataset( DWIAlgos::calcFAFromTensor( dynamic_cast<DatasetTensor*>( ds ) ) );
                }
                break;
            }
            case FNALGO_EV:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_DWI )
                {
                    QList<Dataset*> ev = DWIAlgos::calcEVFromDWI( dynamic_cast<DatasetDWI*>( ds ) );
                    addDataset( ev[0] );
                    addDataset( ev[1] );
                }
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_TENSOR )
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
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_SH )
                {
                    QList<Dataset*> bings = DWIAlgos::fitBingham( dynamic_cast<DatasetSH*>( ds ) );
                    for ( int i = 0; i < bings.size(); ++i )
                    {
                        addDataset( bings[i] );
                    }
                }
                break;
            }
            case FNALGO_TENSOR_TRACK:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_TENSOR )
                {
                    QList<Dataset*> fibs = DWIAlgos::tensorTrack( dynamic_cast<DatasetTensor*>( ds ) );
                    addDataset( fibs[0] );
                }
                break;
            }
            case FNALGO_ISOSURFACE:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_SCALAR )
                {
                    QList<Dataset*> isos = ScalarAlgos::isoSurface( dynamic_cast<DatasetScalar*>( ds ) );
                    addDataset( isos[0] );
                }
                break;
            }
            case FNALGO_BINGHAM_2_TENSOR:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_BINGHAM )
                {
                    QList<Dataset*> tensors = DWIAlgos::bingham2Tensor( dynamic_cast<DatasetBingham*>( ds ) );
                    for ( int i = 0; i < tensors.size(); ++i )
                    {
                        addDataset( tensors[i] );
                    }
                }
                break;
            }
            case FNALGO_TEST:
            {
                Dataset* ds = m_datasetList.at( index.row() );
                if ( ds->properties()->get( FNPROP_TYPE ) == FNDT_NIFTI_SCALAR )
                {
                    DWIAlgos::testAlgo( ds, m_datasetList );
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
            return m_datasetList.at( section )->properties()->get( FNPROP_NAME ).toString();
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

void DataStore::setGlobal( QString key, QVariant data )
{
    m_globals[key] = data;
    emit dataChanged( index( 0, 100 ), index( 0, 108 ) );
}

void DataStore::propChanged()
{
    emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
    emit( headerDataChanged( Qt::Horizontal, 0, 0 ) );
}
