/*
 * correlationmatrix.h
 *
 *  Created on: Dec 18, 2013
 *      Author: boettgerj
 */

#ifndef CORRELATIONMATRIX_H_
#define CORRELATIONMATRIX_H_

#include <QString>

#include "qfile.h"
#include "qdatastream.h"

class CorrelationMatrix
{
public:
    CorrelationMatrix( int i );
    CorrelationMatrix( QString filename );
    virtual ~CorrelationMatrix();

    void init( int n );
    int getN();
    void setValue( int i, int j, float v );
    float getValue( int i, int j );

    void makeHistogram(bool* roi);

    float percFromThresh(float t);
    float threshFromPerc(float p);
    void setInitialized(bool b);

private:
    void loadEverything();
    bool* m_loaded;
    int m_n;
    float** m_values;
    QFile* m_file;
    QDataStream* m_instream;
    void load(int i);
    QString m_filename;

    int* m_histogram;
    float* m_perc_histogram;
    int m_nbins;
};

#endif /* CORRELATIONMATRIX_H_ */
