/*
 * scenerenderer.h
 *
 * Created on: 09.05.2012
 * @author Ralph Schurade
 */

#ifndef SCENERENDERER_H_
#define SCENERENDERER_H_

#include "objectrenderer.h"

#include "../../data/enums.h"

#include <QVector2D>
#include <QMatrix4x4>
#include <QModelIndex>

class ArcBall;
class DataStore;
class QItemSelectionModel;

class SceneRenderer : public ObjectRenderer
{
public:
	SceneRenderer( QString name );
	virtual ~SceneRenderer();

	void resizeGL( int width, int height );

	void initGL();
	void draw( QMatrix4x4 mvMatrix, QMatrix4x4 pMatrix );
	void renderMerge();
	QImage* screenshot( QMatrix4x4 mvMatrix, QMatrix4x4 pMatrix );

    QVector3D mapMouse2World( int x, int y, int z );
    QVector2D mapWorld2Mouse( float x, float y, float z );
    QVector3D mapMouse2World( float x, float y );
    uint get_object_id( int x, int y, int width, int height );
    QImage* getOffscreenTexture( int width, int height );

    void renderPick();

    QString getRenderTarget();

private:
	void renderScene();
	void renderScenePart( int renderMode, QString target0, QString target1 );

	void renderDatasets();
	void renderRois();

    void generate_pixel_buffer_objects( int width, int height );
	void initFBO( int width, int height );
    GLuint createTexture( int width, int height );
    void setRenderTarget( QString target );
    void setRenderTargets( QString target0, QString target1 );
    GLuint getTexture( QString name );
    void clearTexture( QString name, float r, float g, float b, float a );

	QString m_renderTarget;

	GLuint vbo;

	int m_width;
	int m_height;
	int m_renderMode;

	QMatrix4x4 m_mvMatrix;
	QMatrix4x4 m_pMatrix;

    GLuint pbo_a;
    GLuint pbo_b;

    QHash<QString, GLuint>textures;
    GLuint RBO;
    GLuint FBO;
};

#endif /* SCENERENDERER_H_ */
