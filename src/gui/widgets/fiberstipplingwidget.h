/*
 * fiberstipplingwidget.h
 *
 *  Created on: May 5, 2014
 *      Author: schurade
 */

#ifndef FIBERSTIPPLINGWIDGET_H_
#define FIBERSTIPPLINGWIDGET_H_

#include "GL/glew.h"

#include <QList>
#include <QWidget>
#include <QtOpenGL/QGLWidget>
#include <QGraphicsView>
#include <QGraphicsScene>

class Dataset;
class ButtonWithLabel;
class SelectWithLabel;
class SliderWithEdit;
class SliderWithEditInt;
class FiberStipplingGLWidget;

class FiberStipplingWidget: public QWidget
{
    Q_OBJECT

public:
    FiberStipplingWidget(  QString name, QWidget *parent = 0, const QGLWidget *shareWidget = 0 );
    virtual ~FiberStipplingWidget();

public slots:
    void update();
    void updateSelects();
    void updateIsoBounds();
    void setWidgetVisible( bool visible );
    void startAlgo();


private:
    FiberStipplingGLWidget* m_view;


    SelectWithLabel* m_anatomySelect;
    SelectWithLabel* m_vectorSelect;
    SelectWithLabel* m_probTracSelect;

    QList<Dataset*> m_scalarDSL;
    QList<Dataset*> m_vectorDSL;

    SelectWithLabel* m_orientSelect;

    SliderWithEditInt* m_resolutionMultiply;

    SliderWithEdit* m_iso1;
    SliderWithEdit* m_iso2;

    bool m_visible;
};

#endif /* FIBERSTIPPLINGWIDGET_H_ */
