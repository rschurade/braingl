/*
 * datastore.h
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

#ifndef DATASTORE_H_
#define DATASTORE_H_

#include <vector>

class Dataset;

class DataStore
{
public:
    DataStore();
    virtual ~DataStore();

    void addDataset( Dataset* dataset );

private:
    std::vector< Dataset* >m_datasetList;
};

#endif /* DATASTORE_H_ */
