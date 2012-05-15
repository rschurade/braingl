/*
 * globalpropertyview.h
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */

#ifndef GLOBALPROPERTYVIEW_H_
#define GLOBALPROPERTYVIEW_H_

#include <QtGui/QAbstractItemView>

class QLineEdit;
class QVBoxLayout;
class QSlider;

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

    QWidget* getWidget();

public slots:
    void dataChanged( const QModelIndex &topLeft, const QModelIndex &bottomRight );

    void sagittalSliderChanged( int value );
    void coronalSliderChanged( int value );
    void axialSliderChanged( int value );

    void sagittalEditChanged( QString text );
    void coronalEditChanged( QString text );
    void axialEditChanged( QString text );

private slots:


private:
    QWidget* m_widget;

    QVBoxLayout* m_layout;

    QSlider* m_sagittalSlider;
    QSlider* m_coronalSlider;
    QSlider* m_axialSlider;

    QLineEdit* m_sagittalEdit;
    QLineEdit* m_coronalEdit;
    QLineEdit* m_axialEdit;

};

#endif /* GLOBALPROPERTYVIEW_H_ */
