/*
 * glfunctions.h
 *
 *  Created on: 15.05.2012
 *      Author: Ralph Schurade
 */

#ifndef GLFUNCTIONS_H_
#define GLFUNCTIONS_H_

#include "../../thirdparty/glew/include/glew.h"

#include <QtCore/QPoint>
#include <QtCore/QList>
#include <QtGui/QVector3D>
#include <QtGui/QImage>
#include <QtGui/QColor>

class QAbstractItemModel;
class QGLShaderProgram;
class TextRenderer;

struct VertexData
{
    QVector3D position;
    QVector3D texCoord;
};


class GLFunctions
{
public:
    static void setupTextures( QAbstractItemModel* model );
    static void setTexInterpolation( QAbstractItemModel* model, int row );

    static QGLShaderProgram* initShader( QString name );
    static void loadShaders();
    static void reloadShaders();

    static QList<int> getTextureIndexes( QAbstractItemModel* model );

    static QGLShaderProgram* getShader( QString name );

    static void setShaderVarsSlice( QGLShaderProgram* program, QAbstractItemModel* model );

    static QVector<QString> getShaderNames();
    static QString getShaderCode( QString name );
    static void setShaderCode( QString name,  QString source );

    static int getPickIndex();

    static void generate_frame_buffer_texture( const int screen_width, const int screen_height );
    static void generate_pixel_buffer_objects();
    static uint get_object_id( int x, int y );

    static void beginPicking();
    static void endPicking();
    static bool isPicking();

    static void beginOffscreen();
    static void endOffscreen();

    static QImage* getOffscreenTexture();

    static void updateColormapShader();

    static void setScreenSize( int width, int height );
    static QPoint getScreenSize();

    static void initTextRenderer();
    static void renderText( QString text, int x, int y, int size = 10, QColor color = QColor( 0, 0, 0 ) );

private:
    GLFunctions();
    virtual ~GLFunctions();

    static void copyShaderToString( QString name, QString ext );

    static TextRenderer* m_textRenderer;

    static QHash< QString, QString >m_shaderSources;
    static QHash< QString, QGLShaderProgram* >m_shaders;
    static QVector<QString>m_shaderNames;

    static bool shadersLoaded;
    static unsigned int pickIndex;
    static bool picking;

    static GLuint tex;
    static GLuint rbo;
    static GLuint fbo;
    static GLuint pbo_a;
    static GLuint pbo_b;
    static int screenWidth;
    static int screenHeight;
};

#endif /* GLFUNCTIONS_H_ */
