/*
 * roimodel.cpp
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */
#include "roimodel.h"

#include "selectionbox.h"
#include "vptr.h"

#include <QtCore/QDebug>

ROIModel::ROIModel( QAbstractItemModel* globalProps ) :
    m_globalProps( globalProps ),
    m_count( 0 )
{

}

ROIModel::~ROIModel()
{
}

int ROIModel::rowCount( const QModelIndex &parent ) const
{
    //qDebug() << "rowCount():" << parent;
    if ( parent.isValid() )
    {
        if ( parent.internalId() == - 1 )
        {
            return m_boxes[parent.row()].size() - 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return m_boxes.size();
    }
}

int ROIModel::columnCount( const QModelIndex &parent ) const
{
    return 1;
}

QModelIndex ROIModel::index( int row, int column, const QModelIndex & parent ) const
{
    //qDebug() << "index():" << row << column << parent;
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
    //qDebug() << "parent():" << index;
    if ( index.internalId() == -1 )
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
    if ( index.internalId() == -1 )
    {
        roi = m_boxes[index.row()][0];
    }
    else
    {
        roi = m_boxes[index.internalId()][index.row()+1];
    }
    switch ( role )
    {
        case Qt::CheckStateRole:
        {
            return VPtr<ROI>::asPtr( roi )->properties()->get( Fn::ROI::ACTIVE );
            break;
        }
        case Qt::BackgroundColorRole:
        {
            return VPtr<ROI>::asPtr( roi )->properties()->get( Fn::ROI::COLOR );
            break;
        }
        case Qt::DisplayRole:
        {
            switch ( (Fn::ROI)index.column() )
            {
                case Fn::ROI::POINTER:
                {
                    return roi;
                    break;
                }
                default:
                    return VPtr<ROI>::asPtr( roi )->properties()->get( Fn::ROI::NAME );
                    break;
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
                return QString( Fn::ROI2String::s( (Fn::ROI)section ) );
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
            if ( index.internalId() == -1 )
            {
                return VPtr<ROI>::asPtr( m_boxes[index.row()][0] )->properties()->set( Fn::ROI::ACTIVE, !VPtr<ROI>::asPtr( m_boxes[index.row()][0] )->properties()->get( Fn::ROI::ACTIVE ).toBool() );
            }
            else
            {
                return VPtr<ROI>::asPtr( m_boxes[index.internalId()][index.row()+1] )->properties()->set( Fn::ROI::ACTIVE, !VPtr<ROI>::asPtr( m_boxes[index.internalId()][index.row()+1] )->properties()->get( Fn::ROI::ACTIVE ).toBool() );
            }
            break;
        }
        case Qt::DisplayRole:
        {
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

    Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if ( index.column() == 0 )
    {
        defaultFlags |= Qt::ItemIsUserCheckable;
    }

    return defaultFlags;
}

bool ROIModel::insertRows( int row, int count, const QModelIndex &parent )
{
    QString name = QString("new box") + QString::number( m_count++ );
    SelectionBox* newBox = new SelectionBox( name, m_globalProps );
    connect( newBox->properties(), SIGNAL( signalPropChanged() ), this, SLOT( propChanged() ) );
    //qDebug() << parent << parent.isValid();
    if ( !parent.isValid() )
    {
        // top box
        QList<QVariant>l;
        l.push_back( VPtr<ROI>::asQVariant( newBox ) );
        beginInsertRows( QModelIndex(), m_boxes.size(), m_boxes.size() );
        m_boxes.push_back( l );
        endInsertRows();
    }
    else
    {
        // child box
        if ( parent.internalId() == -1 )
        {
            beginInsertRows( parent, m_boxes.size(), m_boxes.size() );
            newBox->properties()->set( Fn::ROI::COLOR, VPtr<ROI>::asPtr( m_boxes[parent.row()][0] )->properties()->get( Fn::ROI::COLOR ) );
            m_boxes[parent.row()].push_back( VPtr<ROI>::asQVariant( newBox ) );
        }
        else
        {
            beginInsertRows( parent.parent(), m_boxes.size(), m_boxes.size() );
            newBox->properties()->set( Fn::ROI::COLOR, VPtr<ROI>::asPtr( m_boxes[parent.internalId()][0] )->properties()->get( Fn::ROI::COLOR ) );
            m_boxes[parent.internalId()].push_back( VPtr<ROI>::asQVariant( newBox ) );
        }
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
        m_boxes.removeAt( row );
        endRemoveRows();
    }
    else
    {
        beginRemoveRows( parent, row, row );
        QList<QVariant>l = m_boxes[parent.row()];
        l.removeAt( row );
        endMoveRows();
    }
    return true;
}

void ROIModel::propChanged()
{
    emit ( dataChanged( index( 0, 0 ), index( 0, 0 ) ) );

}

