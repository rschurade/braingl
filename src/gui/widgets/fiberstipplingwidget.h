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
#include <QVector3D>

class Dataset;
class ButtonWithLabel;
class ColorWidgetWithLabel;
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

    void iso1SliderChanged();
    void iso2SliderChanged();
    void iso1ColorChanged();
    void iso2ColorChanged();
    void lineWidthChanged();

private:
    FiberStipplingGLWidget* m_view;


    SelectWithLabel* m_anatomySelect;
    SelectWithLabel* m_vectorSelect;
    SelectWithLabel* m_probTracSelect;

    QList<Dataset*> m_scalarDSL;
    QList<Dataset*> m_vectorDSL;

    SelectWithLabel* m_orientSelect;

    SliderWithEdit* m_iso1;
    SliderWithEdit* m_iso2;
    ColorWidgetWithLabel* m_iso1Color;
    ColorWidgetWithLabel* m_iso2Color;

    SliderWithEditInt* m_lineWidth;
    SliderWithEdit* m_transparencyAdjust;

    bool m_visible;

    std::vector<float>extractAnatomyAxial();
    std::vector<float>extractAnatomySagittal();
    std::vector<float>extractAnatomyCoronal();

    void extractVectorsAxial( std::vector<float>& verts, std::vector<float>& colors );
};

#endif /* FIBERSTIPPLINGWIDGET_H_ */
