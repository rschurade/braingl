/*
 * sorts.h
 *
 * Created on: Nov 26, 2012
 * @author Ralph Schurade
 */

#ifndef SORTS_H_
#define SORTS_H_

#include <QVector>

class Sorts
{
public:
    static void quickFindFirstK( QVector<int>& ids, std::vector<float>& vals, int left, int right, int k );
    static int partition( QVector<int>& ids, std::vector<float>& vals, int left, int right, int pivotIndex );
    static void swap( QVector<int>& ids, int id1, int id2 );

    static void quickSort( QVector<int>& ids, std::vector<float>& vals, int left, int right );

private:
    Sorts();
    virtual ~Sorts();
};

#endif /* SORTS_H_ */
