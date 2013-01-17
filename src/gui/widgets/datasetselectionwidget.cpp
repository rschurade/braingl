/*
 * datasetselectionwidget.cpp
 *
 *  Created on: Jan 17, 2013
 *      Author: schurade
 */
#include "../../data/datasets/dataset.h"

#include "selectwithlabel.h"

#include "datasetselectionwidget.h"

DatasetSelectionWidget::DatasetSelectionWidget( QVector< QPair<QString, FN_DATASET_TYPE> >&filter, QList<Dataset*> &dsl, QWidget* parent ) :
    QWidget( parent )
{
    m_layout = new QVBoxLayout();

    for ( int i = 0; i < filter.size(); ++i )
    {
        SelectWithLabel* sel = new SelectWithLabel( filter[i].first, i );
        for ( int k = 0; k < dsl.size(); ++k )
        {
            if ( dsl[k]->getProperty( "type" ).toInt() == filter[i].second || filter[i].second == FNDT_NIFTI_ANY )
            {
                sel->insertItem( k, dsl[k]->getProperty( "name" ).toString() );
            }
        }
        m_layout->addWidget( sel );
    }
    setLayout( m_layout );
}

DatasetSelectionWidget::~DatasetSelectionWidget()
{
}

