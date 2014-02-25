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

#include "qobject.h"
#include "qnetworkaccessmanager.h"
#include "qnetworkreply.h"

class CorrelationMatrix : public QObject
{
    Q_OBJECT

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
    void save(QString filename);

public slots:
    void serviceRequestFinished(QNetworkReply* reply);

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
    bool m_remote;
    QNetworkAccessManager *networkManager;
    void loadMetaData();
    void loadRemote( int i );
    int* m_index;
    QString m_id;
    QString m_passwd;
};

#endif /* CORRELATIONMATRIX_H_ */
