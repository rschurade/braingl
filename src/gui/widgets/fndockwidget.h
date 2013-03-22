/*
 * fndockwidget.h
 *
 * Created on: Jan 23, 2013
 * @author Ralph Schurade
 */

#ifndef FNDOCKWIDGET_H_
#define FNDOCKWIDGET_H_

#include <QDockWidget>

class FNDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    FNDockWidget( QString name, QWidget* widget, QWidget* parent );
    virtual ~FNDockWidget();

public slots:
    void toggleTitleWidget();

protected:
    QWidget* m_titleWidget;
};

#endif /* FNDOCKWIDGET_H_ */
