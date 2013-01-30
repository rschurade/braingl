/*
 * datasetpropertyview.h
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */

#ifndef DATASETPROPERTYVIEW_H_
#define DATASETPROPERTYVIEW_H_

#include <QtGui/QAbstractItemView>

class QHBoxLayout;
class QVBoxLayout;

class DatasetPropertyView : public QAbstractItemView
{
    Q_OBJECT

public:
    DatasetPropertyView( QWidget* parent = 0 );
    virtual ~DatasetPropertyView();

    QRect visualRect( const QModelIndex &index ) const;
    void scrollTo( const QModelIndex &index, ScrollHint hint = EnsureVisible );
    QModelIndex indexAt( const QPoint &point ) const;
    QModelIndex moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
    int horizontalOffset() const;
    int verticalOffset() const;
    bool isIndexHidden( const QModelIndex &index ) const;
    void setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags );
    QRegion visualRegionForSelection( const QItemSelection &selection ) const;

    QModelIndex getSelectedIndex( int column );

private:
    QItemSelection m_selected;

signals:
    void selectedChanged();

public slots:
    void selectionChanged( const QItemSelection &selected, const QItemSelection &deselected );
};

#endif /* DATASETPROPERTYVIEW_H_ */
