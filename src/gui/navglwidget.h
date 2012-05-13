/*
 * navglwidget.h
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */

#ifndef NAVGLWIDGET_H_
#define NAVGLWIDGET_H_

#include <QtGui/QDockWidget>

class QGLWidget;
class QSlider;
class QVBoxLayout;

class NavGLWidget : public QDockWidget
{
    Q_OBJECT

public:
    NavGLWidget( QString name, QWidget* parent = 0 );
    virtual ~NavGLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:
    QGLWidget* m_glWidget;
    QVBoxLayout* m_layout;
    QSlider* m_slider;

private slots:
    void sliderChanged( int value );

};

#endif /* NAVGLWIDGET_H_ */
