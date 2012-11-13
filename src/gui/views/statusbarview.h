/*
 * statusbarview.h
 *
 *  Created on: Nov 13, 2012
 *      Author: schurade
 */

#ifndef STATUSBARVIEW_H_
#define STATUSBARVIEW_H_

#include <QtGui/QAbstractItemView>

class StatusBarView  : public QAbstractItemView
{
    Q_OBJECT

public:
    StatusBarView();
    virtual ~StatusBarView();

    QRect visualRect( const QModelIndex &index ) const;
    void scrollTo( const QModelIndex &index, ScrollHint hint = EnsureVisible );
    QModelIndex indexAt( const QPoint &point ) const;
    QModelIndex moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
    int horizontalOffset() const;
    int verticalOffset() const;
    bool isIndexHidden( const QModelIndex &index ) const;
    void setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags );
    QRegion visualRegionForSelection( const QItemSelection &selection ) const;

public slots:
    void selectionChanged( const QItemSelection &selected, const QItemSelection &deselected );
    void dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );

private:
    int m_selected;
    QString m_globalInfo;
    QString m_datasetInfo;
    int m_x;
    int m_y;
    int m_z;


signals:
    void sigStatusChanged( QString text );
};

#endif /* STATUSBARVIEW_H_ */
