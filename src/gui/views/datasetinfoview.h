/*
 * datasetinfoview.h
 *
 *  Created on: 12.05.2012
 *      Author: Ralph Schurade
 */

#ifndef DATASETINFOVIEW_H_
#define DATASETINFOVIEW_H_

#include <QTableView>

class DatasetInfoView : public QTableView
{
public:
    DatasetInfoView( QWidget * parent = 0 );
    virtual ~DatasetInfoView();
};

#endif /* DATASETINFOVIEW_H_ */
