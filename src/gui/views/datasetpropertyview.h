/*
 * datasetpropertyview.h
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */

#ifndef DATASETPROPERTYVIEW_H_
#define DATASETPROPERTYVIEW_H_

#include <QtGui/QAbstractItemView>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class SliderWithEdit;
class SelectWithLabel;
class CheckboxWithLabel;
class QxtSpanSlider;

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
    void interpolationStateChanged( int state );
    void activeStateChanged( int state );

    void alphaChanged( float value );

    void lodChanged( int index );
    void renderSliceChanged( int index );
    void scalingChanged( float value );

    void lower1Changed( int value );
    void upper1Changed( int value );
    void lower2Changed( int value );
    void upper2Changed( int value );



private:
    QModelIndex getSelectedIndex( int column );
    void updateWidgetVisibility();

    QItemSelection m_selected;

    QWidget* m_widget;

    QVBoxLayout* m_layout;
    QLineEdit* m_nameEdit;

    SliderWithEdit* m_lowerThresholdSlider;
    SliderWithEdit* m_upperThresholdSlider;
    SliderWithEdit* m_alphaSlider;

    SelectWithLabel* m_colormapSelect;

    CheckboxWithLabel* m_textureInterpolation;
    CheckboxWithLabel* m_textureActive;

    SelectWithLabel* m_lodSelect;
    SelectWithLabel* m_sliceSelect;
    SliderWithEdit* m_scalingSlider;

    QxtSpanSlider* m_spanSlider1;
    QxtSpanSlider* m_spanSlider2;
};

#endif /* DATASETPROPERTYVIEW_H_ */
