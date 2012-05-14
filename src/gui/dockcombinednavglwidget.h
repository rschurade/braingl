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
    CombinedNavGLWidget* m_glWidget;
    QVBoxLayout* m_layout;
    QSlider* m_slider;
    QLineEdit* m_lineEdit;

    QString m_name;

private slots:
    void update();
    void sliderChanged( int value );
    void settingChanged( QString name, QVariant data );

signals:
    void sliderChange( QString name, QVariant value );

};

#endif /* NAVGLWIDGET_H_ */
