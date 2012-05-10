/*
 * scenerenderer.h
 *
 *  Created on: 09.05.2012
 *      Author: Ralph
 */

#ifndef SCENERENDERER_H_
#define SCENERENDERER_H_

#include "../glew/include/glew.h"

class DataStore;
class SliceRenderer;

class SceneRenderer
{
public:
	SceneRenderer( DataStore* dataStore );
	virtual ~SceneRenderer();

	void resizeGL( int width, int height );

	void initGL();
	void draw();

private:
	DataStore* m_dataStore;
	SliceRenderer* m_sliceRenderer;
};

#endif /* SCENERENDERER_H_ */
