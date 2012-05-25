/*
 * navglwidget.h
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */

#ifndef DOCKCOMBINEDNAVGLWIDGET_H_
#define DOCKCOMBINEDNAVGLWIDGET_H_

#include <QtCore/QVariant>
#include <QtGui/QDockWidget>
#include <QtOpenGL/QGLWidget>

class DataStore;
class CombinedNavGLWidget;
class QSlider;
class QLineEdit;
class QVBoxLayout;
class LineEdit;


class DockCombinedNavGLWidget : public QDockWidget
{
    Q_OBJECT

public:
    DockCombinedNavGLWidget( DataStore* dataStore, QString name, QWidget* parent = 0, const QGLWidget *shareWidget = 0 );
    virtual ~DockCombinedNavGLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:
    QString m_name;

    CombinedNavGLWidget* m_glWidget;
    QVBoxLayout* m_layout;

private slots:

signals:

};

#endif /* NAVGLWIDGET_H_ */
