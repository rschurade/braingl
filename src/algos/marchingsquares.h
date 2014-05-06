/*
 * marchingsquares.h
 *
 *  Created on: May 6, 2014
 *      Author: schurade
 */

#ifndef MARCHINGSQUARES_H_
#define MARCHINGSQUARES_H_

#include <vector>

class MarchingSquares
{
public:
    MarchingSquares( std::vector<float>* data, float isoValue, int nx, int ny, int mult = 4 );
    virtual ~MarchingSquares();

    std::vector<float> run();


private:
    std::vector<float>* m_data;
    std::vector<bool> m_mask;
    std::vector<int>m_states;
    float m_isoValue;
    int m_nx;
    int m_ny;
    int m_yMult;
    int m_mult;
    int m_mult4;

    std::vector<float>m_verts;

    int step( int x, int y );
    int id( int x, int y );

    void paintSW( int x, int y );
    void paintSE( int x, int y );
    void paintNE( int x, int y );
    void paintNW( int x, int y );
    void paintSN( int x, int y );
    void paintEW( int x, int y );
};

#endif /* MARCHINGSQUARES_H_ */
