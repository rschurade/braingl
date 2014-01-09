/*
 * datasetselectionwidget.cpp
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */
#include "datasetselectionwidget.h"

#include "../controls/selectwithlabel.h"

#include "../../../data/datasets/dataset.h"
#include "../../../data/vptr.h"

DatasetSelectionWidget::DatasetSelectionWidget( QVector< QPair<QString, QList<Fn::DatasetType> > >&filter, QList<QVariant> &dsl, QWidget* parent ) :
    QWidget( parent )
{
    m_layout = new QVBoxLayout();

    for ( int i = 0; i < filter.size(); ++i )
    {
        SelectWithLabel* sel = new SelectWithLabel( filter[i].first, i );
        m_selectList.push_back( sel );

        QList<Fn::DatasetType> types = filter[i].second;

        for ( int k = 0; k < dsl.size(); ++k )
        {
            for( int l = 0; l < types.size(); ++l )
            {
                if ( VPtr<Dataset>::asPtr( dsl[k] )->properties()->get( Fn::Property::D_TYPE ).toInt() == (int)types[l] )
                {
                    sel->insertItem( k, VPtr<Dataset>::asPtr( dsl[k] )->properties()->get( Fn::Property::D_NAME ).toString() );
                    m_selections.push_back( dsl[k] );
                }
            }
        }
        m_layout->addWidget( sel );
    }
    m_layout->addStretch();

    QPushButton* button = new QPushButton( tr("Select") );
    connect( button, SIGNAL( clicked() ), this, SLOT( selectButtonPressed() ) );
    m_layout->addWidget( button );
    setLayout( m_layout );
}

DatasetSelectionWidget::~DatasetSelectionWidget()
{
}

void DatasetSelectionWidget::selectButtonPressed()
{
    QList<QVariant>out;
    for( int i = 0; i < m_selectList.size(); ++i )
    {
        out.push_back( m_selections[m_selectList[i]->getCurrentIndex()] );
    }
    emit( signalSelected( out ) );
    this->hide();
}
