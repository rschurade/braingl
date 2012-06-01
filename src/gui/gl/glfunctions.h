/*
 * glfunctions.h
 *
 *  Created on: 15.05.2012
 *      Author: Ralph
 */

#ifndef GLFUNCTIONS_H_
#define GLFUNCTIONS_H_

#include <QtCore/QList>

class QAbstractItemModel;
class QGLShaderProgram;

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

    static void setSliceShaderVars( QGLShaderProgram* program, QAbstractItemModel* model );
    static void setShaderVars( QString name, QAbstractItemModel* model );

    static QList<int> getTextureIndexes( QAbstractItemModel* model );

    static QGLShaderProgram* getShader( QString name );

private:
    GLFunctions();
    virtual ~GLFunctions();

    static QHash< QString, QGLShaderProgram* >m_shaders;

    static bool shadersLoaded;
};

#endif /* GLFUNCTIONS_H_ */
