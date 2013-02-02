/*
 * selectionboxmodel.cpp
 *
 *  Created on: 02.02.2013
 *      Author: Ralph
 */

#include "selectionboxmodel.h"

SelectionBoxModel::SelectionBoxModel()
{

}

SelectionBoxModel::~SelectionBoxModel()
{
}

int SelectionBoxModel::rowCount( const QModelIndex &parent ) const
{
    return 0;
}

int SelectionBoxModel::columnCount( const QModelIndex &parent ) const
{
    return 0;
}

QModelIndex SelectionBoxModel::index( int row, int column, const QModelIndex & parent ) const
{
    return QModelIndex();
}

QModelIndex SelectionBoxModel::parent( const QModelIndex & index ) const
{
    return QModelIndex();
}

QVariant SelectionBoxModel::data( const QModelIndex &index, int role ) const
{
    return 0;
}

QVariant SelectionBoxModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    return 0;
}

bool SelectionBoxModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
    return false;
}

Qt::ItemFlags SelectionBoxModel::flags( const QModelIndex& index ) const
{
    return 0;
}
