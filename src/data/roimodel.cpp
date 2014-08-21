/*
 * roimodel.cpp
 *
 * Created on: 02.02.2013
 * @author Ralph Schurade
 */
#include "roimodel.h"

#include "models.h"

#include "roibox.h"
#include "vptr.h"

#include "../gui/gl/glfunctions.h"

#include <QDebug>

ROIModel::ROIModel() :
    m_count( 0 )
{

}

ROIModel::~ROIModel()
{
}

int ROIModel::rowCount( const QModelIndex &parent ) const
{
    if ( parent.isValid() )
    {
        if ( (int)parent.internalId() == - 1 )
        {
            return m_rois[parent.row()].size() - 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return m_rois.size();
    }
}

int ROIModel::columnCount( const QModelIndex &parent ) const
{
    return 1;
}

QModelIndex ROIModel::index( int row, int column, const QModelIndex & parent ) const
{
    if ( parent.isValid() )
    {
        return createIndex( row, column, parent.row() );
    }
    else
    {
        return createIndex( row, column, -1 );
    }
}

QModelIndex ROIModel::parent( const QModelIndex & index ) const
{
    if ( (int)index.internalId() == -1 )
    {
        return QModelIndex();
    }
    else
    {
        return createIndex( index.internalId(), index.column(), -1 );
    }
}

QVariant ROIModel::data( const QModelIndex &index, int role ) const
{
    QVariant roi;
    if ( (int)index.internalId() == -1 )
    {
        if ( index.row() < m_rois.size() )
        {
            if ( m_rois[index.row()].size() > 0 )
            {
                roi = m_rois[index.row()][0];
            }
        }
    }
    else
    {
        if ( (int)index.internalId() < m_rois.size() )
        {
            if ( m_rois[index.internalId()].size() > index.row()+1 )
            {
                roi = m_rois[index.internalId()][index.row()+1];
            }
        }

    }
    switch ( role )
    {
        case Qt::CheckStateRole:
        {
            return VPtr<ROI>::asPtr( roi )->properties()->get( Fn::Property::D_ACTIVE );
            break;
        }
        case Qt::BackgroundColorRole:
        {
            return VPtr<ROI>::asPtr( roi )->properties()->get( Fn::Property::D_COLOR );
            break;
        }
        case Qt::DisplayRole:
        {
            switch ( (Fn::Property)index.column() )
            {
                case Fn::Property::D_POINTER:
                {
                    return roi;
                    break;
                }
                default:
                {
                    return VPtr<ROI>::asPtr( roi )->properties()->get( (Fn::Property)index.column() );
                    break;
                }
            }
            break;
        }
        default:
            break;
    }
    return QVariant();
}

QVariant ROIModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    switch ( role )
    {
        case Qt::DisplayRole:
        {
            if ( orientation == Qt::Horizontal )
            {
                return QString( Fn::Prop2String::s( (Fn::Property)section ) );
            }
            break;
        }
    }
    return QVariant();
}

bool ROIModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    switch ( role )
    {
        case Qt::CheckStateRole:
        {
            if ( (int)index.internalId() == -1 )
            {
                VPtr<ROI>::asPtr( m_rois[index.row()][0] )->properties()->set( Fn::Property::D_ACTIVE, !VPtr<ROI>::asPtr( m_rois[index.row()][0] )->properties()->get( Fn::Property::D_ACTIVE ).toBool() );
            }
            else
            {
                VPtr<ROI>::asPtr( m_rois[index.internalId()][index.row()+1] )->properties()->set( Fn::Property::D_ACTIVE, !VPtr<ROI>::asPtr( m_rois[index.internalId()][index.row()+1] )->properties()->get( Fn::Property::D_ACTIVE ).toBool() );
            }
            break;
        }
        case Qt::DisplayRole:
        {
            if ( index.column() == (int)Fn::Property::D_POINTER )
            {
                if ( (int)index.internalId() == -1 )
                {
                    m_rois[index.row()][0] = value;
                }
                else
                {
                    qDebug() << index.internalId() << index.row()+1;
                    m_rois[index.internalId()][index.row()+1] = value;
                }

                emit( dataChanged( QModelIndex(), QModelIndex() ) );
                return true;
            }

            if ( index.column() == (int)Fn::Property::D_UPDATED )
            {
                emit( dataChanged( QModelIndex(), QModelIndex() ) );
                return true;
            }
            if ( (int)index.internalId() == -1 )
            {
                VPtr<ROI>::asPtr( m_rois[index.row()][0] )->properties()->set( (Fn::Property)index.column(), value );
            }
            else
            {
                VPtr<ROI>::asPtr( m_rois[index.internalId()][index.row()+1] )->properties()->set( (Fn::Property)index.column(), value );
            }
            break;
        }
    }

    emit( dataChanged( index, index ) );

    return true;
}

Qt::ItemFlags ROIModel::flags( const QModelIndex& index ) const
{
    if ( !index.isValid() )
    {
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled ;
    }

    Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
    if ( index.parent().isValid() )
    {
        defaultFlags |= Qt::ItemIsDragEnabled;
    }
    else
    {
        if ( m_rois.size() > index.row() && m_rois[index.row()].size() == 1 )
        {
            defaultFlags |= Qt::ItemIsDragEnabled;
        }
    }

    if ( index.column() == 0 )
    {
        defaultFlags |= Qt::ItemIsUserCheckable;
    }

    return defaultFlags;
}

bool ROIModel::insertRows( int row, int count, const QModelIndex &parent )
{
    ROI* newROI;
    if ( count == 0 )
    {
        //newROI = new ROIBox();
        newROI = GLFunctions::roi;
    }
    else
    {
        return false;
    }
    connect( newROI, SIGNAL( signalPropChanged( int ) ), this, SLOT( propChanged( int ) ) );

    if ( parent.isValid() )
    {
        // child box
        if ( parent.parent().isValid() )
        {
            // child box selected
            beginInsertRows( parent.parent(), m_rois[parent.parent().row()].size(), m_rois[parent.parent().row()].size() );
            newROI->properties()->set( Fn::Property::D_COLOR, VPtr<ROI>::asPtr( m_rois[parent.parent().row()][0] )->properties()->get( Fn::Property::D_COLOR ) );
            m_rois[parent.parent().row()].push_back( VPtr<ROI>::asQVariant( newROI ) );
            endInsertRows();
        }
        else
        {
            // top box selected
            beginInsertRows( parent, m_rois[parent.row()].size(), m_rois[parent.row()].size() );
            newROI->properties()->set( Fn::Property::D_COLOR, VPtr<ROI>::asPtr( m_rois[parent.row()][0] )->properties()->get( Fn::Property::D_COLOR ) );
            m_rois[parent.row()].push_back( VPtr<ROI>::asQVariant( newROI ) );
            endInsertRows();
        }
    }
    else
    {
        // nothing selected, insert top box
        QList<QVariant>l;
        l.push_back( VPtr<ROI>::asQVariant( newROI ) );
        beginInsertRows( QModelIndex(), m_rois.size(), m_rois.size() );
        m_rois.push_back( l );
        endInsertRows();

    }
    emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );
    return true;
}

bool ROIModel::removeRows( int row, int count, const QModelIndex &parent )
{
    if ( !parent.isValid() )
    {
        beginRemoveRows( QModelIndex(), row, row );
        m_rois.removeAt( row );
        endRemoveRows();
    }
    else
    {
        beginRemoveRows( parent, row, row );
        m_rois[parent.row()].removeAt( row + 1 );
        endRemoveRows();
    }

    emit ( dataChanged( QModelIndex(), QModelIndex() ) );
    return true;
}

void ROIModel::propChanged( int value )
{
    bool found = false;
    int ii = -1;
    int kk = -1;
    for ( int i = 0; i < m_rois.size(); ++i )
    {
        for ( int k = 0; k < m_rois[i].size(); ++k )
        {
            if ( value == VPtr<ROI>::asPtr( m_rois[i][k] )->properties()->get( Fn::Property::D_ID ).toInt() )
            {
                found = true;
                kk = k;
                break;
            }
        }
        if ( found )
        {
            ii = i;
            break;
        }
    }
    if ( found )
    {
        if ( kk == 0 )
        {

            QModelIndex parent;
            QModelIndex index = this->index( ii, 0, parent );
            emit ( dataChanged( index, index ) );
        }
        else
        {
            QModelIndex parent = this->index( ii, 0, QModelIndex() );
            QModelIndex index = this->index( kk - 1, 0, parent );
            emit ( dataChanged( index, index ) );
        }
    }
}

QModelIndexList ROIModel::match( const QModelIndex &start, int role, const QVariant &value, int hits, Qt::MatchFlags flags ) const
{
    QModelIndexList l;

    if ( role == Qt::DisplayRole )
    {
        for ( int i = 0; i < m_rois.size(); ++i )
        {
            for ( int k = 0; k < m_rois[i].size(); ++k )
            {
                if ( value.toInt() == VPtr<ROI>::asPtr( m_rois[i][k] )->properties()->get( Fn::Property::D_PICK_ID ).toInt() )
                {
                    if ( k == 0 )
                    {
                        l.push_back( createIndex( i, 0, -1 ) );
                    }
                    else
                    {
                        l.push_back( createIndex( k-1, 0, i ) );
                    }
                }
            }
        }
    }

    return l;
}

ROI* ROIModel::getRoi( int branch, int pos )
{
    if ( branch < m_rois.size() && pos < m_rois[branch].size() )
    {
        return VPtr<ROI>::asPtr( m_rois[branch][pos] );
    }
    else
    {
        return NULL;
    }
}
