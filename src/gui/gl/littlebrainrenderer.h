/*
 * littlebrainrenderer.h
 *
 *  Created on: Dec 13, 2013
 *      Author: boettgerj
 */

#ifndef LITTLEBRAINRENDERER_H_
#define LITTLEBRAINRENDERER_H_

#include "meshrenderer.h"

class LittleBrainRenderer: public MeshRenderer
{
public:
    LittleBrainRenderer( TriangleMesh2* mesh );
    virtual ~LittleBrainRenderer();
    int m_LB_colormode;

private:
    void setRenderParams( PropertyGroup* props );
};

#endif /* LITTLEBRAINRENDERER_H_ */
