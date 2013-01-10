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
class SliderWithEditInt;
class SelectWithLabel;
class CheckboxWithLabel;
//class QxtSpanSlider;

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

    void floatChanged( float value, int id );
    void checkBoxChanged( int state, int id );
    void intChanged( int state, int id );

    void lowerThresholdChanged( float value, int id );
    void upperThresholdChanged( float value, int id );

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

    SelectWithLabel* m_orderSelect;
    SelectWithLabel* m_lodSelect;
    SelectWithLabel* m_sliceSelect;
    SliderWithEdit* m_scalingSlider;
    SliderWithEdit* m_isoValueSlider;

    SliderWithEditInt* m_bValueSlider;

    CheckboxWithLabel* m_qBallScaling;

    SliderWithEdit* m_faThresholdSlider;
    SliderWithEdit* m_evThresholdSlider;
    SliderWithEdit* m_gammaSlider;
    SliderWithEdit* m_glyphOffsetSlider;

    SelectWithLabel* m_tensorRendorMode;
};

#endif /* DATASETPROPERTYVIEW_H_ */
