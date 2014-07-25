/*
 * bundlingthread.cpp
 *
 *  Created on: Oct 24, 2013
 *      Author: boettgerj
 */

#include "bundlingthread.h"

BundlingThread::BundlingThread(Connections* cons)
{
    m_cons = cons;
}

BundlingThread::~BundlingThread()
{
    // TODO Auto-generated destructor stub
}

void BundlingThread::run()
{
    m_cons->fullAttract();
}
