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
    DataStore* m_dataStore;
    QString m_name;

    NavGLWidget* m_glWidget;
    QVBoxLayout* m_layout;
    QSlider* m_slider;
    QLineEdit* m_lineEdit;

private slots:
    void update();
    void sliderChanged( int value );
    void editChanged( QString text );
    void settingChanged();

signals:
    void sliderChange( QString name, QVariant value );

};

#endif /* NAVGLWIDGET_H_ */
