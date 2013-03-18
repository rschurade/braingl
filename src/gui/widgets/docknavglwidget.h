/*
 * navglwidget.h
 *
 *  Created on: 12.05.2012
 *      Author: Ralph Schurade
 */

#ifndef NAVGLWIDGET_H_
#define NAVGLWIDGET_H_

#include "GL/glew.h"

#include <QVariant>
#include <QtOpenGL/QGLWidget>

class NavGLWidget;
class NavFrame;
class SliderWithEditInt2;
class QVBoxLayout;

class DockNavGLWidget : public QWidget
{
    Q_OBJECT

public:
    DockNavGLWidget( QString name, int orient, QWidget* parent = 0, const QGLWidget *shareWidget = 0 );
    virtual ~DockNavGLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:
    QString m_name;
    NavFrame* m_glWidget;
    QVBoxLayout* m_layout;
    SliderWithEditInt2* m_slider;

public slots:
    void update();

private slots:
    void sliderChanged( int value );
    void settingChanged();

signals:
    void sliderChange( QString name, QVariant value );

};

#endif /* NAVGLWIDGET_H_ */
