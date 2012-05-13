/*
 * datasetpropertywidget.cpp
 *
 *  Created on: 13.05.2012
 *      Author: Ralph
 */

#include "datasetpropertywidget.h"

DatasetPropertyWidget::DatasetPropertyWidget( QString name, QWidget* parent ) :
    QDockWidget( name, parent )
{
    setObjectName( name );

}

DatasetPropertyWidget::~DatasetPropertyWidget()
{
    // TODO Auto-generated destructor stub
}

