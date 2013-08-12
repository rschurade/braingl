/*
 * camerabase.cpp
 *
 *  Created on: Aug 7, 2013
 *      Author: schurade
 */

#include "camerabase.h"

CameraBase::CameraBase( int width, int height ) :
    m_width( width ),
    m_height( height )
{
    m_adjust_width = 1.0 / ( ( width - 1.0 ) * 0.5 );
    m_adjust_height = 1.0 / ( ( height - 1.0 ) * 0.5 );

}

CameraBase::~CameraBase()
{
}

/// sets the window size.
void CameraBase::set_win_size( int width,  int height )
{
    m_width = width;
    m_height = height;
    m_adjust_width = 1.0 / ( ( width - 1.0 ) * 0.5 );
    m_adjust_height = 1.0 / ( ( height - 1.0 ) * 0.5 );
}
