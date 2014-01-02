/*
 * littlebrainrenderer.cpp
 *
 *  Created on: Dec 13, 2013
 *      Author: boettgerj
 */

#include "littlebrainrenderer.h"

LittleBrainRenderer::LittleBrainRenderer( TriangleMesh2* mesh ) :
        MeshRenderer( mesh ),
        m_LB_colormode( 3 )
{

}

LittleBrainRenderer::~LittleBrainRenderer()
{

}

void LittleBrainRenderer::setRenderParams( PropertyGroup* props )
{
    MeshRenderer::setRenderParams( props );
    m_colorMode = m_LB_colormode;
}
