/*
 * navglwidget.h
 *
 *  Created on: 12.05.2012
 *      Author: Ralph
 */

#ifndef NAVGLWIDGET_H_
#define NAVGLWIDGET_H_

#include <QtCore/QVariant>
#include <QtGui/QDockWidget>
#include <QtOpenGL/QGLWidget>

class DataStore;
class NavGLWidget;
class QSlider;
class QLineEdit;
class QVBoxLayout;
class LineEdit;


class DockNavGLWidget : public QDockWidget
{
    Q_OBJECT

public:
    DockNavGLWidget( DataStore* dataStore, QString name, QWidget* parent = 0, const QGLWidget *shareWidget = 0 );
    virtual ~DockNavGLWidget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:
    NavGLWidget* m_glWidget;
    QVBoxLayout* m_layout;
    QSlider* m_slider;
    QLineEdit* m_lineEdit;

    QString m_name;

private slots:
    void sliderChanged( int value );
    void settingChanged( QString name, QVariant data );

signals:
    void sliderChange( QString name, QVariant value );

};

#endif /* NAVGLWIDGET_H_ */
