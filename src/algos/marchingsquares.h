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
    MarchingSquares( std::vector<float>* data, float isoValue, int nx, int ny, float dx, float dy, bool interpolation = true );
    virtual ~MarchingSquares();

    std::vector<float> run();

private:
    std::vector<float>* m_data;
    std::vector<float> m_mask;
    std::vector<int>m_states;
    float m_isoValue;
    int m_nx;
    int m_ny;
    float m_dx;
    float m_dy;
    float m_dx2;
    float m_dy2;

    bool m_interpolation;

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
