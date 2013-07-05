/*
 * roipropertyview.h
 *
 * Created on: 03.02.2013
 * @author Ralph Schurade
 */

#ifndef ROIPROPERTYVIEW_H_
#define ROIPROPERTYVIEW_H_

#include <QAbstractItemView>

class QHBoxLayout;
class QVBoxLayout;

class ROIPropertyView : public QAbstractItemView
{
    Q_OBJECT

public:
    ROIPropertyView( QWidget* parent = 0 );
    virtual ~ROIPropertyView();

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
    void nothingSelected();

public slots:
    void selectionChanged( const QItemSelection &selected, const QItemSelection &deselected );
    void dataChanged( const QModelIndex  &topLeft , const QModelIndex  &bottomRight );
};

#endif /* ROIPROPERTYVIEW_H_ */
