/*
 * mathwidget.h
 *
 *  Created on: Mar 24, 2014
 *      Author: schurade
 */

#ifndef MATHWIDGET_H_
#define MATHWIDGET_H_

#include "../../../data/enums.h"

#include <QtGui>

class DatasetScalar;
class SliderWithEdit;
class SelectWithLabel;

class MathWidget : public QWidget
{
    Q_OBJECT

public:
    MathWidget( DatasetScalar* ds, QWidget* parent = 0 );
    virtual ~MathWidget();

private:
    DatasetScalar* m_dataset;

    QVBoxLayout* m_layout;
    SelectWithLabel* m_modeSelect;
    SelectWithLabel* m_sourceSelect;
    QPushButton* m_executeButton;

    SliderWithEdit* m_arg;

private slots:
    void modeChanged( int mode );
    void execute();

};

#endif /* MATHWIDGET_H_ */
