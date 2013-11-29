/*
 * bugfixglshaderprogram.cpp
 *
 * @author Timm Wetzel
 * 2013-11-29
 *
 * Minimal reimplementation of QGLShaderProgram because the original has a bug causing
 * it to find no shader capabilities with a CoreProfile GL context (Qt4.8).
 * The interface is complete as of Qt4.8.5, but only the bare minimum is implemented.
 * Using additional methods will cause linker errors.
 *
 * Inspired by https://github.com/mortennobel/QtOpenGL3.2Core .
 */

#include "bugfixglshaderprogram.h"

// debugging
#include <iostream>
#include <string>
bool _glError(const char *file, int line) {
        bool ret = true;
        GLenum err (glGetError());

        while(err!=GL_NO_ERROR) {
                ret = false;
                std::string error;

                switch(err) {
                        case GL_INVALID_OPERATION:      error="INVALID_OPERATION";      break;
                        case GL_INVALID_ENUM:           error="INVALID_ENUM";           break;
                        case GL_INVALID_VALUE:          error="INVALID_VALUE";          break;
                        case GL_OUT_OF_MEMORY:          error="OUT_OF_MEMORY";          break;
                        case GL_INVALID_FRAMEBUFFER_OPERATION:  error="INVALID_FRAMEBUFFER_OPERATION";  break;
                }

                std::cerr<<"GL_"<<error<<" - "<<file<<':'<<line<<std::endl;
                err=glGetError();
        }
        return ret;
}
#define _glCheckError() _glError(__FILE__,__LINE__)


BugfixGLShaderProgram::BugfixGLShaderProgram(QObject *parent)
    : m_parent(parent)
{
    //qDebug() << "BugfixGLShaderProgram::BugfixGLShaderProgram";
}

BugfixGLShaderProgram::BugfixGLShaderProgram(QGLContext *context, QObject *parent)
    : m_context(context),
      m_parent(parent)
{
    qDebug() << "BugfixGLShaderProgram::BugfixGLShaderProgram TBD";
}

BugfixGLShaderProgram::~BugfixGLShaderProgram()
{
    //qDebug() << "BugfixGLShaderProgram::~BugfixGLShaderProgram";
}

bool BugfixGLShaderProgram::addShaderFromSourceCode(QFlags<QGLShader::ShaderTypeBit> type, QString const& source)
{
    qDebug() << "BugfixGLShaderProgram::addShaderFromSourceCode";
    GLuint program = glCreateProgram();
    _glCheckError();
    GLuint shader = glCreateShader( type );
    _glCheckError();
    const GLchar *strarr[] = { (const char *)source.constData() };
    glShaderSource( shader, 1, strarr, NULL );
    _glCheckError();
    glCompileShader( shader );
    _glCheckError();

    GLint compiled;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
    if ( !compiled ) {
        qWarning() << "shader failed to compile: ";
        GLint  logSize;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
        QByteArray logMsg( logSize, 0 );
        glGetShaderInfoLog( shader, logSize, NULL, logMsg.data() );
        _glCheckError();
        m_log.append(logMsg);
        qWarning() << logMsg;
        m_programId = -1;
        return false;
    }

    glAttachShader( program, shader );
    _glCheckError();
    m_programId = program;
    return true;
}

int BugfixGLShaderProgram::attributeLocation(char const*) const
{
    qDebug() << "BugfixGLShaderProgram::attributeLocation TBD";
    return 0;
}

bool BugfixGLShaderProgram::bind()
{
    qDebug() << "BugfixGLShaderProgram::bind";
    /* use program object */
    if (!isLinked())
    {
        link();
    }
    glUseProgram(m_programId);
    _glCheckError();
    // XXX error checking
    return true;
}

void BugfixGLShaderProgram::enableAttributeArray(int location)
{
    qDebug() << "BugfixGLShaderProgram::enableAttributeArray";
    if (location != -1)
         glEnableVertexAttribArray(location);
}

bool BugfixGLShaderProgram::isLinked() const
{
    qDebug() << "BugfixGLShaderProgram::isLinked";
    return m_isLinked;
}

bool BugfixGLShaderProgram::link()
{
    qDebug() << "BugfixGLShaderProgram::link";
    //glBindFragDataLocation(program, 0, "fragColor");

    /* link  and error check */
    glLinkProgram(m_programId);
    _glCheckError();

    GLint linked;
    glGetProgramiv( m_programId, GL_LINK_STATUS, &linked );
    if ( !linked ) {
        qWarning() << "Shader program failed to link";
        GLint  logSize;
        glGetProgramiv( m_programId, GL_INFO_LOG_LENGTH, &logSize);
        QByteArray logMsg( logSize, 0 );
        glGetProgramInfoLog( m_programId, logSize, NULL, logMsg.data() );
        _glCheckError();
        m_log.append(logMsg);
        qWarning() << logMsg;
        m_isLinked = false;
        return false;
    }
    m_isLinked = true;
    return true;
}

QString BugfixGLShaderProgram::log() const
{
    qDebug() << "BugfixGLShaderProgram::log";
    return m_log;
}

GLuint BugfixGLShaderProgram::programId() const
{
    return m_programId;
}

void BugfixGLShaderProgram::setUniformValue(char const*, QMatrix4x4 const&)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue TBD";
}

void BugfixGLShaderProgram::setUniformValue(int location, float value)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue";
    if (location != -1)
        glUniform1fv(location, 1, &value);
}

void BugfixGLShaderProgram::setUniformValue(char const* name, float value)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue";
    setUniformValue(uniformLocation(name), value);
}

void BugfixGLShaderProgram::setUniformValue(int location, float v1, float v2)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue TBD";
    if (location != -1) {
        GLfloat values[2] = {v1, v2};
        glUniform2fv(location, 1, values);
    }
}

void BugfixGLShaderProgram::setUniformValue(char const* name, float v1, float v2)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue";
    setUniformValue(uniformLocation(name), v1, v2);
}

void BugfixGLShaderProgram::setUniformValue(int location, float v1, float v2, float v3)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue";
    if (location != -1) {
        GLfloat values[3] = {v1, v2, v3};
        glUniform3fv(location, 1, values);
    }
}

void BugfixGLShaderProgram::setUniformValue(char const* name, float v1, float v2, float v3)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue TBD";
    setUniformValue(uniformLocation(name), v1, v2, v3);
}

void BugfixGLShaderProgram::setUniformValue(int location, float v1, float v2, float v3, float v4)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue TBD";
    if (location != -1) {
        GLfloat values[4] = {v1, v2, v3, v4};
        glUniform4fv(location, 1, values);
    }
}

void BugfixGLShaderProgram::setUniformValue(char const* name, float v1, float v2, float v3, float v4)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue";
    setUniformValue(uniformLocation(name), v1, v2, v3, v4);
}

void BugfixGLShaderProgram::setUniformValue(int location, int value)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue";
    if (location != -1)
       glUniform1i(location, value);
}

void BugfixGLShaderProgram::setUniformValue(char const* name, int value)
{
    qDebug() << "BugfixGLShaderProgram::setUniformValue";
    setUniformValue(uniformLocation(name), value);
}

int QGLShaderProgram::uniformLocation(const char *name) const
{
    if (m_isLinked) {
        return glGetUniformLocation(m_programId, name);
    } else {
        qWarning() << "QGLShaderProgram::uniformLocation(" << name
                   << "): shader program is not linked";
        return -1;
    }
}

#if 0
GLuint GLWidget::prepareShaderProgram( const QString& vertexShaderPath,
                                     const QString& fragmentShaderPath )
{
    struct Shader {
        const QString&  filename;
        GLenum       type;
        GLchar*      source;
    }  shaders[2] = {
        { vertexShaderPath, GL_VERTEX_SHADER, NULL },
        { fragmentShaderPath, GL_FRAGMENT_SHADER, NULL }
    };

    GLuint program = glCreateProgram();

    for ( int i = 0; i < 2; ++i ) {
        Shader& s = shaders[i];
        QFile file( s.filename );
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            qWarning() << "Cannot open file " << s.filename;
            exit( EXIT_FAILURE );
        }
        QByteArray data = file.readAll();
        file.close();
        s.source = data.data();

        if ( shaders[i].source == NULL ) {
            qWarning() << "Failed to read " << s.filename;
            exit( EXIT_FAILURE );
        }
        GLuint shader = glCreateShader( s.type );
        glShaderSource( shader, 1, (const GLchar**) &s.source, NULL );
        glCompileShader( shader );

        GLint  compiled;
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            qWarning() << s.filename << " failed to compile:" ;
            GLint  logSize;
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &logSize );
            char* logMsg = new char[logSize];
            glGetShaderInfoLog( shader, logSize, NULL, logMsg );
            qWarning() << logMsg;
            delete [] logMsg;

            exit( EXIT_FAILURE );
        }

        glAttachShader( program, shader );
    }

    /* Link output */
    glBindFragDataLocation(program, 0, "fragColor");

    /* link  and error check */
    glLinkProgram(program);

    GLint  linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    if ( !linked ) {
        qWarning() << "Shader program failed to link";
        GLint  logSize;
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &logSize);
        char* logMsg = new char[logSize];
        glGetProgramInfoLog( program, logSize, NULL, logMsg );
        qWarning() << logMsg ;
        delete [] logMsg;

        exit( EXIT_FAILURE );
    }

    /* use program object */
    glUseProgram(program);

    return program;
}

void GLWidget::initializeGL()
{
    glSetup();

    // Prepare a complete shader program...
    m_shader = prepareShaderProgram( ":/simple.vert", ":/simple.frag" );

    // Create a interleaved triangle (vec3 position, vec3 color)
    float points[] = { -0.5f, -0.5f, 0.0f, 1.0f, 0.0, 0.0,
                        0.5f, -0.5f, 0.0f, 0.0f, 1.0, 0.0,
                        0.0f,  0.5f, 0.0f, 0.0f, 0.0, 1.0,  };
    glGenVertexArrays(1, &m_vertexBuffer);
    glBindVertexArray(m_vertexBuffer);
    GLuint  vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * 6 * sizeof(float), points, GL_STATIC_DRAW);
    GLuint positionAttribute = glGetAttribLocation(m_shader, "vertex");
    GLuint colorAttribute = glGetAttribLocation(m_shader, "color");
    glEnableVertexAttribArray(positionAttribute);
    glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (const GLvoid *)0);
    glEnableVertexAttribArray(colorAttribute);
    glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (const GLvoid *)(sizeof(float)*3));
    glCheckError();
}
#endif
