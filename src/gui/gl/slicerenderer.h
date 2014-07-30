/*
 * slicerenderer.h
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */

#ifndef SLICERENDERER_H_
#define SLICERENDERER_H_

#include "objectrenderer.h"

#include <QString>

class SliceRenderer : public ObjectRenderer
{
public:
	SliceRenderer();
	virtual ~SliceRenderer();

	void init();

	void draw( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, QString target );

protected:
    void setupTextures();
    void setShaderVars( QString target );

    void initGeometry();

private:
	void drawAxial( QString target );
	void drawCoronal( QString target );
	void drawSagittal( QString target );

	GLuint vbo0;
	GLuint vbo1;
	GLuint vbo2;
};

#endif /* SLICERENDERER_H_ */
