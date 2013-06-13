/*
 * datasetselectionwidget.cpp
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#include "datasetselectionwidget.h"

#include "../controls/selectwithlabel.h"

#include "../../../data/datasets/dataset.h"

DatasetSelectionWidget::DatasetSelectionWidget( QVector< QPair<QString, Fn::DatasetType> >&filter, QList<Dataset*> &dsl, QWidget* parent ) :
    QWidget( parent )
{
    m_layout = new QVBoxLayout();

    for ( int i = 0; i < filter.size(); ++i )
    {
        SelectWithLabel* sel = new SelectWithLabel( filter[i].first, i );
        for ( int k = 0; k < dsl.size(); ++k )
        {
            if ( dsl[k]->properties()->get( Fn::Property::TYPE ).toInt() == (int)filter[i].second || (int)filter[i].second == (int)Fn::DatasetType::NIFTI_ANY )
            {
                sel->insertItem( k, dsl[k]->properties()->get( Fn::Property::NAME ).toString() );
            }
        }
        m_layout->addWidget( sel );
    }
    m_layout->addStretch();
    setLayout( m_layout );
}

DatasetSelectionWidget::~DatasetSelectionWidget()
{
}

