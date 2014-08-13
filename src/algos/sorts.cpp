/*
 * sorts.cpp
 *
 * Created on: Nov 26, 2012
 * @author Ralph Schurade
 */
#include "sorts.h"

#include <QDebug>

Sorts::Sorts()
{
}

Sorts::~Sorts()
{
}

void Sorts::quickFindFirstK( std::vector<int>& ids, std::vector<float>& vals, int left, int right, int k )
{
    if ( right > left )
    {
        int pivotIndex = left + ( right - left ) / 2;
        int pivotNewIndex = Sorts::partition(ids, vals, left, right, pivotIndex);

        quickFindFirstK( ids, vals, left, pivotNewIndex-1, k );
        if ( pivotNewIndex < k )
        {
            quickFindFirstK(ids, vals, pivotNewIndex+1, right, k);
        }
    }
}

int Sorts::partition( std::vector<int>& ids, std::vector<float>& vals, int left, int right, int pivotIndex )
{
    int pivotValue = vals.at( ids.at( pivotIndex ) );

    Sorts::swap ( ids, pivotIndex, right );  // Move pivot to end
    int storeIndex = left;

    for ( int i = left; i <= right; ++i )  //      for i from left to right
    {
        if ( vals.at( ids.at (i ) ) > pivotValue )
        {
            Sorts::swap ( ids, storeIndex, i );
             ++storeIndex;
        }
    }
    Sorts::swap ( ids, right, storeIndex );  // Move pivot to its final place
    return storeIndex;
}

void Sorts::swap( std::vector<int>& ids, int id1, int id2 )
{
    int tmp = ids.at( id1 );
    ids[id1] = ids.at( id2 );
    ids[id2] = tmp;
}

void Sorts::quickSort( std::vector<int>& ids, std::vector<float>& vals, int left, int right )
{
    int i = left, j = right;
    int tmp;
    float pivot = vals.at( ids.at( left + ( right - left ) / 2 ) );

    /* partition */
    while ( i <= j )
    {
        while ( vals.at( ids.at( i ) ) > pivot )
            ++i;
        while ( vals.at( ids.at( j ) ) < pivot )
            --j;
        if ( i <= j )
        {
            tmp = ids.at( i );
            ids[i] = ids.at( j );
            ids[j] = tmp;
            ++i;
            --j;
        }
    };

    /* recursion */
    if ( left < j )
        quickSort( ids, vals, left, j );
    if ( i < right )
        quickSort( ids, vals, i, right );
}
