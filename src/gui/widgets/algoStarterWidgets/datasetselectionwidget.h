/*
 * datasetselectionwidget.h
 *
 * Created on: Jan 17, 2013
 * @author Ralph Schurade
 */

#ifndef DATASETSELECTIONWIDGET_H_
#define DATASETSELECTIONWIDGET_H_

#include "../../../data/enums.h"

#include <QtGui>

class Dataset;
class SelectWithLabel;

class DatasetSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    DatasetSelectionWidget( std::vector< QPair<QString, QList<Fn::DatasetType > > >&filter, QList<QVariant> &dsl, QWidget* parent = 0 );
    virtual ~DatasetSelectionWidget();

private:
    QVBoxLayout* m_layout;
    QList<SelectWithLabel*>m_selectList;
    QList<QVariant>m_selections;
signals:
    void signalSelected( QList<QVariant> );

public slots:
    void selectButtonPressed();
};

#endif /* DATASETSELECTIONWIDGET_H_ */
