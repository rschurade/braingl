/*
 * navglwidget.h
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */

#ifndef NAVGLWIDGET_H_
#define NAVGLWIDGET_H_

#include "../../thirdparty/glew/include/glew.h"

#include <QtCore/QVariant>
#include <QtOpenGL/QGLWidget>

class QAbstractItemModel;
class NavGLWidget;
class NavFrame;
class SliderWithEditInt2;
class QVBoxLayout;

class DockNavGLWidget : public QWidget
{
    Q_OBJECT

public:
    DockNavGLWidget( QAbstractItemModel* model, QString name, int orient, QWidget* parent = 0, const QGLWidget *shareWidget = 0 );
    virtual ~DockNavGLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:
    QString m_name;
    QAbstractItemModel* m_model;
    NavFrame* m_glWidget;
    QVBoxLayout* m_layout;
    SliderWithEditInt2* m_slider;

private slots:
    void sliderChanged( int value );
    void settingChanged();

signals:
    void sliderChange( QString name, QVariant value );

};

#endif /* NAVGLWIDGET_H_ */
