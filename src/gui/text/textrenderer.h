/*
 * textrenderer.h
 *
 * Created on: Feb 20, 2013
 * @author schurade
 */

#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

//#include "GL/glew.h"
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

#include <QChar>
#include <QHash>
#include <QString>
#include <QColor>

class TextRenderer
{
public:
    TextRenderer();
    virtual ~TextRenderer();

    void init();

    void renderText( QString text, int x, int y, int width, int height, int renderMode );
    void setSize( float size );
    void setColor( QColor color );

private:
    GLuint *vboIds;
    GLuint *m_fontTextureGLuint;
    QHash<QChar, int >m_characterPositions;

    float m_textSizeX;
    float m_textSizeY;
    QColor m_textColor;

    void createFontTexture();
    void initGeometry();
};

#endif /* TEXTRENDERER_H_ */
