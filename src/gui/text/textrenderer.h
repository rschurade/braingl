/*
 * textrenderer.h
 *
 *  Created on: Feb 20, 2013
 *      Author: schurade
 */

#ifndef TEXTRENDERER_H_
#define TEXTRENDERER_H_

#include "../../thirdparty/glew/include/glew.h"

#include <QtCore/QChar>
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtGui/QColor>

class TextRenderer
{
public:
    TextRenderer();
    virtual ~TextRenderer();

    void init();

    void renderText( QString text, int x = 0, int y = 0 );
    void setSize( int size );
    void setColor( QColor color );

private:
    GLuint *vboIds;
    GLuint m_fontTextureGLuint;
    QHash<QChar, int >m_characterPositions;

    float m_textSize;
    QColor m_textColor;

    void createFontTexture();
    void initGeometry();
};

#endif /* TEXTRENDERER_H_ */
