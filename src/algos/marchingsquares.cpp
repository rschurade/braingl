/*
 * marchingsquares.cpp
 *
 *  Created on: May 6, 2014
 *      Author: schurade
 */

#include "marchingsquares.h"

MarchingSquares::MarchingSquares( std::vector<float>* data, float isoValue, int nx, int ny, float dx, float dy, bool voxelize ) :
    m_data( data ),
    m_isoValue( isoValue ),
    m_nx( nx ),
    m_ny( ny ),
    m_dx( dx ),
    m_dy( dy ),
    m_dx2( dx / 2 ),
    m_dy2( dy / 2 ),
    m_interpolation( voxelize ),
    m_stripeType( 0 ),
    m_stripeDistance( 2 )
{
}

MarchingSquares::~MarchingSquares()
{
}

int MarchingSquares::id( int x, int y )
{
    return x + m_nx * y;
}

int MarchingSquares::step( int x, int y )
{
    // Scan our 4 pixel area
    bool upLeft = m_mask[id( x, y + 1 )] > 0;
    bool upRight = m_mask[id( x + 1, y + 1 )] > 0;
    bool downLeft = m_mask[id( x, y )] > 0;
    bool downRight = m_mask[id( x + 1, y )] > 0;

    // Determine which state we are in
    int state = 0;

    if ( downLeft )
    {
        state |= 1;
    }
    if ( downRight )
    {
        state |= 2;
    }
    if ( upRight )
    {
        state |= 4;
    }
    if ( upLeft )
    {
        state |= 8;
    }

    return state;
}

std::vector<float> MarchingSquares::run()
{
    m_verts.clear();
    m_mask.resize( m_data->size(), false );
    for ( unsigned int i = 0; i < m_data->size(); ++i )
    {
        m_mask[i] = m_data->at( i ) - m_isoValue;
    }

    m_states.resize( m_nx * m_ny, 0 );

    for ( int y = 0; y < m_ny - 1; ++y )
    {
        for ( int x = 0; x < m_nx - 1; ++x )
        {
            m_states[id( x, y )] = step( x, y );
        }
    }

    for ( int y = 0; y < m_ny - 1; ++y )
    {
        for ( int x = 0; x < m_nx - 1; ++x )
        {
            int state = m_states[id( x, y )];

            switch ( state )
            {
                case 0:
                case 15:
                    break;
                case 1:
                    paintSW( x, y );
                    break;
                case 2:
                    paintSE( x, y );
                    break;
                case 3:
                    paintEW( x, y );
                    break;
                case 4:
                    paintNE( x, y );
                    break;
                case 5:
                    paintSE( x, y );
                    paintNW( x, y );
                    break;
                case 6:
                    paintSN( x, y );
                    break;
                case 7:
                    paintNW( x, y );
                    break;
                case 8:
                    paintNW( x, y );
                    break;
                case 9:
                    paintSN( x, y );
                    break;
                case 10:
                    paintSW( x, y );
                    paintNE( x, y );
                    break;
                case 11:
                    paintNE( x, y );
                    break;
                case 12:
                    paintEW( x, y );
                    break;
                case 13:
                    paintSE( x, y );
                    break;
                case 14:
                    paintSW( x, y );
                    break;
            }
        }
    }
    return m_verts;
}

std::vector<float> MarchingSquares::runStripes( int stripeType, int distance )
{
    m_stripeType = stripeType;
    m_stripeDistance = distance;

    m_stripeVerts.clear();

    for ( int y = 0; y < m_ny - 1; ++y )
    {
        for ( int x = 0; x < m_nx - 1; ++x )
        {
            if ( ( ( x + y ) % m_stripeDistance ) == 0 )
            {
                if ( m_mask[id( x, y )] > 0 )
                {
                    m_stripeVerts.push_back( x * m_dx - m_dx2 );
                    m_stripeVerts.push_back( y * m_dy - m_dx2 );
                    m_stripeVerts.push_back( x * m_dx + m_dx2 );
                    m_stripeVerts.push_back( y * m_dy + m_dy2 );
                }
            }
        }
    }

    return m_stripeVerts;
}

void MarchingSquares::paintNW( int x, int y )
{
    if ( m_interpolation )
    {
        float xAlpha = ( m_isoValue - m_data->at( id( x, y + 1 ) ) ) / ( m_data->at( id( x + 1, y + 1 ) ) - m_data->at( id( x, y + 1 ) ) );
        float yAlpha = ( m_isoValue - m_data->at( id( x, y ) ) ) / ( m_data->at( id( x, y + 1 ) ) - m_data->at( id( x, y ) ) );

        m_verts.push_back( x * m_dx );
        m_verts.push_back( ( 1 - yAlpha ) * ( y * m_dy ) + yAlpha * ( ( y + 1 ) * m_dy ) );
        m_verts.push_back( ( 1 - xAlpha ) * ( x * m_dx ) + xAlpha * ( ( x + 1 ) * m_dx ) );
        m_verts.push_back( y * m_dy + m_dy );
    }
    else
    {
        m_verts.push_back( x * m_dx );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy );
    }
}

void MarchingSquares::paintNE( int x, int y )
{
    if ( m_interpolation )
    {
        float xAlpha = ( m_isoValue - m_data->at( id( x, y + 1 ) ) ) / ( m_data->at( id( x + 1, y + 1 ) ) - m_data->at( id( x, y + 1 ) ) );
        float yAlpha = ( m_isoValue - m_data->at( id( x + 1, y ) ) ) / ( m_data->at( id( x + 1, y + 1 ) ) - m_data->at( id( x + 1, y ) ) );

        m_verts.push_back( ( 1 - xAlpha ) * ( x * m_dx ) + xAlpha * ( ( x + 1 ) * m_dx ) );
        m_verts.push_back( y * m_dy + m_dy );
        m_verts.push_back( x * m_dx + m_dx );
        m_verts.push_back( ( 1 - yAlpha ) * ( y * m_dy ) + yAlpha * ( ( y + 1 ) * m_dy ) );
    }
    else
    {
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx );
        m_verts.push_back( y * m_dy + m_dy2 );
    }
}

void MarchingSquares::paintSE( int x, int y )
{
    if ( m_interpolation )
    {
        float xAlpha = ( m_isoValue - m_data->at( id( x, y ) ) ) / ( m_data->at( id( x + 1, y ) ) - m_data->at( id( x, y ) ) );
        float yAlpha = ( m_isoValue - m_data->at( id( x + 1, y ) ) ) / ( m_data->at( id( x + 1, y + 1 ) ) - m_data->at( id( x + 1, y ) ) );

        m_verts.push_back( ( 1 - xAlpha ) * ( x * m_dx ) + xAlpha * ( ( x + 1 ) * m_dx ) );
        m_verts.push_back( y * m_dy );
        m_verts.push_back( x * m_dx + m_dx );
        m_verts.push_back( ( 1 - yAlpha ) * ( y * m_dy ) + yAlpha * ( ( y + 1 ) * m_dy ) );
    }
    else
    {
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx );
        m_verts.push_back( y * m_dy + m_dy2 );
    }
}

void MarchingSquares::paintSW( int x, int y )
{
    if ( m_interpolation )
    {
        float xAlpha = ( m_isoValue - m_data->at( id( x, y ) ) ) / ( m_data->at( id( x + 1, y ) ) - m_data->at( id( x, y ) ) );
        float yAlpha = ( m_isoValue - m_data->at( id( x, y ) ) ) / ( m_data->at( id( x, y + 1 ) ) - m_data->at( id( x, y ) ) );
        m_verts.push_back( x * m_dx );
        m_verts.push_back( ( 1 - yAlpha ) * ( y * m_dy ) + yAlpha * ( ( y + 1 ) * m_dy ) );
        m_verts.push_back( ( 1 - xAlpha ) * ( x * m_dx ) + xAlpha * ( ( x + 1 ) * m_dx ) );
        m_verts.push_back( y * m_dy );
    }
    else
    {
        m_verts.push_back( x * m_dx );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy );
    }
}

void MarchingSquares::paintSN( int x, int y )
{
    if ( m_interpolation )
    {
        float xAlpha1 = ( m_isoValue - m_data->at( id( x, y ) ) ) / ( m_data->at( id( x + 1, y ) ) - m_data->at( id( x, y ) ) );
        float xAlpha2 = ( m_isoValue - m_data->at( id( x, y + 1 ) ) ) / ( m_data->at( id( x + 1, y + 1 ) ) - m_data->at( id( x, y + 1 ) ) );

        m_verts.push_back( ( 1 - xAlpha1 ) * ( x * m_dx ) + xAlpha1 * ( ( x + 1 ) * m_dx ) );
        m_verts.push_back( y * m_dy );
        m_verts.push_back( ( 1 - xAlpha2 ) * ( x * m_dx ) + xAlpha2 * ( ( x + 1 ) * m_dx ) );
        m_verts.push_back( y * m_dy + m_dy );
    }
    else
    {
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy );
        m_verts.push_back( x * m_dx + m_dx2 );
        m_verts.push_back( y * m_dy + m_dy );
    }
}

void MarchingSquares::paintEW( int x, int y )
{
    if ( m_interpolation )
    {
        float yAlpha1 = ( m_isoValue - m_data->at( id( x, y ) ) ) / ( m_data->at( id( x, y + 1 ) ) - m_data->at( id( x, y ) ) );
        float yAlpha2 = ( m_isoValue - m_data->at( id( x + 1, y ) ) ) / ( m_data->at( id( x + 1, y + 1 ) ) - m_data->at( id( x + 1, y ) ) );
        m_verts.push_back( x * m_dx );
        m_verts.push_back( ( 1 - yAlpha1 ) * ( y * m_dy ) + yAlpha1 * ( ( y + 1 ) * m_dy ) );
        m_verts.push_back( x * m_dx + m_dx );
        m_verts.push_back( ( 1 - yAlpha2 ) * ( y * m_dy ) + yAlpha2 * ( ( y + 1 ) * m_dy ) );
    }
    else
    {
        m_verts.push_back( x * m_dx );
        m_verts.push_back( y * m_dy + m_dy2 );
        m_verts.push_back( x * m_dx + m_dx );
        m_verts.push_back( y * m_dy + m_dy2 );
    }
}
