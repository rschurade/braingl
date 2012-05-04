/*
 * datastore.cpp
 *
 *  Created on: May 4, 2012
 *      Author: schurade
 */

#include "datastore.h"

DataStore::DataStore()
{
    // TODO Auto-generated constructor stub

}

DataStore::~DataStore()
{
    // TODO Auto-generated destructor stub
}

void DataStore::addDataset( Dataset* dataset )
{
    m_datasetList.push_back( dataset );
}
