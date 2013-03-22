/*
 * globalpropertyview.h
 *
 * Created on: 13.05.2012
 * @author Ralph Schurade
 */

#ifndef GLOBALPROPERTYVIEW_H_
#define GLOBALPROPERTYVIEW_H_

#include <QAbstractItemView>

class QLineEdit;
class QVBoxLayout;
class QSlider;
class SliderWithEditInt;

class GlobalPropertyView : public QAbstractItemView
{
    Q_OBJECT

public:
    GlobalPropertyView( QWidget* parent = 0 );
    virtual ~GlobalPropertyView();

    QRect visualRect( const QModelIndex &index ) const;
    void scrollTo( const QModelIndex &index, ScrollHint hint = EnsureVisible );
    QModelIndex indexAt( const QPoint &point ) const;
    QModelIndex moveCursor( CursorAction cursorAction, Qt::KeyboardModifiers modifiers );
    int horizontalOffset() const;
    int verticalOffset() const;
    bool isIndexHidden( const QModelIndex &index ) const;
    void setSelection( const QRect &rect, QItemSelectionModel::SelectionFlags flags );
    QRegion visualRegionForSelection( const QItemSelection &selection ) const;
    void init();

signals:
    void gotDataChanged();

public slots:
    void dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );
};

#endif /* GLOBALPROPERTYVIEW_H_ */
