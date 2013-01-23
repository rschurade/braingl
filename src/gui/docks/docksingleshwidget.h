/*
 * docksingleshwidget.h
 *
 *  Created on: 12.07.2012
 *      Author: Ralph
 */

#ifndef DOCKSINGLESHWIDGET_H_
#define DOCKSINGLESHWIDGET_H_

#include <QtCore/QVariant>
#include <QtOpenGL/QGLWidget>

#include "fndockwidget.h"

class DataStore;
class SingleSHWidget;
class QVBoxLayout;

class DockSingleSHWidget : public FNDockWidget
{
    Q_OBJECT

public:
    DockSingleSHWidget( DataStore* dataStore, QString name, QWidget* parent = 0, const QGLWidget *shareWidget = 0 );
    virtual ~DockSingleSHWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:
    QString m_name;

    SingleSHWidget* m_glWidget;
    QVBoxLayout* m_layout;
};

#endif /* DOCKSINGLESHWIDGET_H_ */
