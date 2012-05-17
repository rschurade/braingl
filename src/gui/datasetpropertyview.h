/*
 * datasetpropertyview.h
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */

#ifndef DATASETPROPERTYVIEW_H_
#define DATASETPROPERTYVIEW_H_

#include <QtGui/QAbstractItemView>

class QComboBox;
class QLineEdit;
class QVBoxLayout;
class SliderWithEdit;

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

    QWidget* getWidget();

public slots:
    void selectionChanged( const QItemSelection &selected, const QItemSelection &deselected );


private slots:
    void nameEdited();
    void lowerThresholdChanged( float value );
    void upperThresholdChanged( float value );
    void colormapChanged( int index );

private:
    QModelIndex getSelectedIndex( int column );

    QItemSelection m_selected;

    QWidget* m_widget;

    QVBoxLayout* m_layout;
    QLineEdit* m_nameEdit;

    SliderWithEdit* m_lowerThresholdSlider;
    SliderWithEdit* m_upperThresholdSlider;
    SliderWithEdit* m_alphaSlider;

    QComboBox* m_colormapSelect;


};

#endif /* DATASETPROPERTYVIEW_H_ */
