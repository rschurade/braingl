/*
 * glfunctions.h
 *
 * Created on: 15.05.2012
 * @author Ralph Schurade
 */

#ifndef GLFUNCTIONS_H_
#define GLFUNCTIONS_H_

#include "GL/glew.h"

#include <QPoint>
#include <QList>
#include <QVector3D>
#include <QImage>
#include <QColor>

class QGLShaderProgram;
class ShapeRenderer;
class TextRenderer;

struct VertexData
{
    QVector3D position;
    QVector3D texCoord;
};


class GLFunctions
{
public:
    static void setupTextures( QString target = "maingl" );
    static void setTexInterpolation( int row );

    static QGLShaderProgram* initShader( QString name );
    static void loadShaders();
    static void reloadShaders();

    static QList<int> getTextureIndexes( QString target = "maingl" );

    static QGLShaderProgram* getShader( QString name );

    static void setShaderVarsSlice( QGLShaderProgram* program, QString target );
    static void setTextureUniforms( QGLShaderProgram* program, QString target );

    static QVector<QString> getShaderNames();
    static QString getShaderCode( QString name );
    static void setShaderCode( QString name,  QString source );

    static int getPickIndex();

    static void updateColormapShader();

    static void initTextRenderer();
    static void renderText( QString text, int x, int y, int size, int width, int height, QColor color, int renderMode );

    static void initShapeRenderer();
    static void drawBox( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix,
                            float x, float y, float z, float dx, float dy, float dz,
                            QColor color, int pickID, int width, int height, int renderMode );
    static void drawSphere( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix,
                               float x, float y, float z, float dx, float dy, float dz,
                               QColor color, int pickID, int width, int height, int renderMode );

    static void getAndPrintGLError( QString prefix = "" );

    static int idealThreadCount;
    static QHash<QString, float> sliceAlpha;

private:
    GLFunctions() {};
    virtual ~GLFunctions() {};

    static QString copyShaderToString( QString name, QString ext );

    static TextRenderer* m_textRenderer;
    static ShapeRenderer* m_shapeRenderer;

    static QHash< QString, QString >m_shaderIncludes;
    static QHash< QString, QString >m_shaderSources;
    static QHash< QString, QGLShaderProgram* >m_shaders;
    static QVector<QString>m_shaderNames;

    static bool shadersLoaded;
    static unsigned int pickIndex;
};

#endif /* GLFUNCTIONS_H_ */
