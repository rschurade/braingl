/*
 * dataset.cpp
 *
 *  Created on: May 3, 2012
 *      Author: schurade
 */

#include "dataset.h"

Dataset::Dataset( std::string filename ) :
	m_filename( filename )
{
    // TODO Auto-generated constructor stub

}

Dataset::~Dataset()
{
    // TODO Auto-generated destructor stub
}

std::string Dataset::getFilename()
{
	return m_filename;
}
