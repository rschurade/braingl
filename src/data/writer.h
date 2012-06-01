/*
 * writer.h
 *
 *  Created on: Jun 1, 2012
 *      Author: schurade
 */

#ifndef WRITER_H_
#define WRITER_H_

#include <QtCore/QString>

#include "nifti/nifti1_io.h"

#include "dataset.h"

class Writer
{
public:
    Writer( Dataset* dataset, QString fileName );
    virtual ~Writer();

    bool save();

private:
    Dataset* m_dataset;
    QString m_fileName;
};

#endif /* WRITER_H_ */
