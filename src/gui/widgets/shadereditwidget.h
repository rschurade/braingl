/*
 * shadereditwidget.h
 *
 *  Created on: Nov 20, 2012
 *      Author: Ralph Schurade
 */

#ifndef SHADEREDITWIDGET_H_
#define SHADEREDITWIDGET_H_

#include <QtGui>

class ShaderEditWidget : public QWidget
{
    Q_OBJECT

public:
    ShaderEditWidget( QWidget* parent );
    virtual ~ShaderEditWidget();

private:
    QTextEdit* m_edit;
    QString m_currentName;

private slots:
    void slotSelectCode( QAbstractButton  *button );
    void slotSavePressed();
};

#endif /* SHADEREDITWIDGET_H_ */
