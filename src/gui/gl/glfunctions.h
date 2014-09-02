/*
 * glfunctions.h
 *
 * Created on: 15.05.2012
 * @author Ralph Schurade
 */

#ifndef GLFUNCTIONS_H_
#define GLFUNCTIONS_H_

#include <QPoint>
#include <QList>
#include <QVector3D>
#include <QImage>
#include <QColor>

#include <QOpenGLFunctions_3_3_Core>

class ColormapRenderer;
class OrientationHelperRenderer;
class PropertyGroup;
class QGLShaderProgram;
class ROI;
class ShapeRenderer;
class SliceRenderer;
class TextRenderer;

struct VertexData
{
    QVector3D position;
    QVector3D texCoord;
};


class GLFunctions
{
public:
    static bool setupTextures( QString target = "maingl" );
    static void setTexInterpolation( int row );

    static QGLShaderProgram* initShader( QString name );
    static void loadShaders();
    static void reloadShaders();

    static QList<int> getTextureIndexes( QString target = "maingl" );

    static QGLShaderProgram* getShader( QString name );
    static bool validateShader( QString name );

    static void setShaderVarsSlice( QGLShaderProgram* program, QString target );
    static void setTextureUniforms( QGLShaderProgram* program, QString target );

    static std::vector<QString> getShaderNames();
    static QString getShaderCode( QString name );
    static void setShaderCode( QString name,  QString source );

    static unsigned int getPickIndex();

    static void updateColormapShader();

    static void initRenderers();
    static void renderText( QString text, int x, int y, int size, int width, int height, QColor color, int renderMode );
    static void renderLabel( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, QString text, int size, float x, float y, float z, QColor color, float alpha, int width, int height, int renderMode );

    static void renderBox( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix,
                            float x, float y, float z, float dx, float dy, float dz,
                            QColor color, int pickID, int width, int height, int renderMode );
    static void renderSphere( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix,
                               float x, float y, float z, float dx, float dy, float dz,
                               QColor color, int pickID, int width, int height, int renderMode );
    static void renderSlices( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, QString target );

    static void renderOrientHelper( QMatrix4x4 p_matrix, QMatrix4x4 mv_matrix, int width, int height, int renderMode, QString target );


    static void createColormapBarProps( PropertyGroup& props );
    static void drawColormapBar( PropertyGroup& props, int width, int height, int renderMode );

    static bool getAndPrintGLError( QString prefix = "" );

    static void deleteTexture( GLuint tex );

    static int idealThreadCount;
    static int maxDim;
    static QHash<QString, float> sliceAlpha;

    static ROI* roi;
    static QOpenGLFunctions_3_3_Core* f;

    static bool m_debug;

private:
    GLFunctions() {};
    virtual ~GLFunctions() {};

    static QString copyShaderToString( QString name, QString ext );

    static SliceRenderer* m_sliceRenderer;
    static TextRenderer* m_textRenderer;
    static ShapeRenderer* m_shapeRenderer;
    static ColormapRenderer* m_colormapRenderer;
    static OrientationHelperRenderer* m_orientationRenderer;

    static QHash< QString, QString >m_shaderIncludes;
    static QHash< QString, QString >m_shaderSources;
    static QHash< QString, QGLShaderProgram* >m_shaders;
    static std::vector<QString>m_shaderNames;

    static bool shadersLoaded;
    static unsigned int pickIndex;

    static std::vector< GLuint >m_texturesToDelete;
};

#endif /* GLFUNCTIONS_H_ */
