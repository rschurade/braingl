/*
 * colormapwidget.h
 *
 * Created on: 12.02.2013
 * @author Ralph Schurade
 */

#ifndef COLORMAPEDITWIDGET_H_
#define COLORMAPEDITWIDGET_H_

#include "../../algos/colormapbase.h"

#include <QWidget>

class SliderWithEdit;
class ROIPropertyView;
class QVBoxLayout;
class QImage;
class QLabel;
class QPushButton;
class SelectWithLabel;
class EditWithLabel;
class CheckboxWithLabel;

class ColormapEditWidget : public QWidget
{
    Q_OBJECT

public:
    ColormapEditWidget( QWidget* parent = 0 );
    virtual ~ColormapEditWidget();

    void resizeEvent( QResizeEvent* event );

private:
    QImage* createImage( int width );
    void redrawWidget();
    void updateWidget();
    QVBoxLayout* createButtonPanel();

    ColormapBase m_colormap;
    int m_selected;

    QLabel* m_cLabel;
    EditWithLabel* m_nameEdit;
    bool m_contUpdating;
    SelectWithLabel* m_sel;

    QWidget* m_buttonPanel;

    QVector< SliderWithEdit*>m_sliders;

signals:
    void signalUpdate();

private slots:
    void sliderChanged( float value, int id );
    void colorChanged( QColor color, int id );
    void newEntry( int id );
    void removeEntry( int id );
    void save();
    void update();
    void selectionChanged( int id );
    void contUpdatingChanged( int value );
    void moveUp( int id );
    void moveDown( int id );
};

#endif /* COLORMAPEDITWIDGET_H_ */
