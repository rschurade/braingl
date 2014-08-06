/*
 * statewriter.h
 *
 *  Created on: 05.08.2014
 *      Author: Ralph
 */

#ifndef STATEWRITER_H_
#define STATEWRITER_H_

#include <QList>
#include <QString>
#include <QVariant>

class StateWriter
{
public:
    StateWriter();
    virtual ~StateWriter();

    void saveScene( QString fileName, bool packAndGo, QList<QVariant>camera1, QList<QVariant>arcball1, QList<QVariant>camera2, QList<QVariant>arcball2 );



private:

};

#endif /* STATEWRITER_H_ */
