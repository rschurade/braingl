/*
 * datasetselectionwidget.h
 *
 *  Created on: Jan 17, 2013
 *      Author: Ralph Schurade
 */

#ifndef DATASETSELECTIONWIDGET_H_
#define DATASETSELECTIONWIDGET_H_

#include "../../../data/enums.h"

#include <QtGui/QtGui>

class Dataset;

class DatasetSelectionWidget : public QWidget
{
public:
    DatasetSelectionWidget( QVector< QPair<QString, Fn::DatasetType> >&filter, QList<Dataset*> &dsl, QWidget* parent = 0 );
    virtual ~DatasetSelectionWidget();

private:
    QVBoxLayout* m_layout;
};

#endif /* DATASETSELECTIONWIDGET_H_ */
