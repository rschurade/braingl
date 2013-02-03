/*
 * selectionboxmodel.cpp
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */
#include "selectionboxmodel.h"

#include "selectionbox.h"

#include <QtCore/QDebug>

SelectionBoxModel::SelectionBoxModel( QAbstractItemModel* globalProps ) :
    m_count( 0 )
{

}

SelectionBoxModel::~SelectionBoxModel()
{
}

int SelectionBoxModel::rowCount( const QModelIndex &parent ) const
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

int SelectionBoxModel::columnCount( const QModelIndex &parent ) const
{
    return 2;
}

QModelIndex SelectionBoxModel::index( int row, int column, const QModelIndex & parent ) const
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

QModelIndex SelectionBoxModel::parent( const QModelIndex & index ) const
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

QVariant SelectionBoxModel::data( const QModelIndex &index, int role ) const
{
    //qDebug() << "data";
    switch ( role )
    {
        case Qt::CheckStateRole:
        {
            if ( index.internalId() == -1 )
            {
                return m_boxes[index.row()][0]->properties()->get( Fn::Property::ACTIVE );
            }
            else
            {
                return m_boxes[index.internalId()][index.row()+1]->properties()->get( Fn::Property::ACTIVE );
            }
            break;
        }
        case Qt::DisplayRole:
        {
            if ( index.internalId() == -1 )
            {
                return m_boxes[index.row()][0]->properties()->get( Fn::Property::NAME );
            }
            else
            {
                return m_boxes[index.internalId()][index.row()+1]->properties()->get( Fn::Property::NAME );
            }
            break;
        }
        default:
            break;
    }
    return QVariant();
}

QVariant SelectionBoxModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    switch ( role )
    {
        case Qt::DisplayRole:
        {
            if ( orientation == Qt::Horizontal )
            {
                return QString( Fn::Property2String::s( (Fn::Property)section ) );
            }
            break;
        }
    }
    return QVariant();
}

bool SelectionBoxModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    switch ( role )
    {
        case Qt::CheckStateRole:
        {
            if ( index.internalId() == -1 )
            {
                return m_boxes[index.row()][0]->properties()->set( Fn::Property::ACTIVE, !m_boxes[index.row()][0]->properties()->get( Fn::Property::ACTIVE ).toBool() );
            }
            else
            {
                return m_boxes[index.internalId()][index.row()+1]->properties()->set( Fn::Property::ACTIVE, !m_boxes[index.internalId()][index.row()+1]->properties()->get( Fn::Property::ACTIVE ).toBool() );
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

Qt::ItemFlags SelectionBoxModel::flags( const QModelIndex& index ) const
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

bool SelectionBoxModel::insertRows( int row, int count, const QModelIndex &parent )
{
    //qDebug() << parent << parent.isValid();
    if ( !parent.isValid() )
    {
        QList<SelectionBox*>l;
        SelectionBox* newBox = new SelectionBox( QString("new top box") + QString::number( m_count++ ) );
        l.push_back( newBox );

        beginInsertRows( QModelIndex(), m_boxes.size(), m_boxes.size() );
        m_boxes.push_back( l );
        endInsertRows();
    }
    else
    {
        SelectionBox* newBox = new SelectionBox( QString("new child box") + QString::number( m_count++ ) );
        beginInsertRows( QModelIndex(), m_boxes.size(), m_boxes.size() );
        if ( parent.internalId() == -1 )
        {
            m_boxes[parent.row()].push_back( newBox );
        }
        else
        {
            m_boxes[parent.internalId()].push_back( newBox );
        }
        endInsertRows();
    }

    return true;
}
