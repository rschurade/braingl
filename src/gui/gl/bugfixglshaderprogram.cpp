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

#if defined(Q_OS_MAC) && QT_VERSION <= 0x040805 && QT_VERSION >= 0x040800    // if less or equal to 4.8.5

#include <QtCore>

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
    : m_context(QGLContext::currentContext()),
      m_parent(parent)
{
    //qDebug() << "BugfixGLShaderProgram::BugfixGLShaderProgram";
    Q_ASSERT(m_context);
    m_programId = glCreateProgram();
    Q_ASSERT( m_programId != 0 );
}

BugfixGLShaderProgram::BugfixGLShaderProgram(const QGLContext *context, QObject *parent)
    : m_context(context ? context : QGLContext::currentContext()),
      m_parent(parent)
{
    //qDebug() << "BugfixGLShaderProgram::BugfixGLShaderProgram";
    Q_ASSERT(m_context);
    if (!QGLContext::areSharing(m_context, QGLContext::currentContext()))
    {
        qWarning("context must be the current context or sharing with it.");
        ((QGLContext *)m_context)->makeCurrent();
    }

    m_programId = glCreateProgram();
    Q_ASSERT( m_programId != 0 );
}

BugfixGLShaderProgram::~BugfixGLShaderProgram()
{
    //qDebug() << "BugfixGLShaderProgram::~BugfixGLShaderProgram";
    glDeleteProgram( m_programId );
}

bool BugfixGLShaderProgram::addShaderFromSourceCode(QFlags<QGLShader::ShaderTypeBit> type, QString const& source)
{
    //qDebug() << "BugfixGLShaderProgram::addShaderFromSourceCode";
    _glCheckError();
    if (!QGLContext::areSharing(m_context, QGLContext::currentContext()))
    {
        qWarning("context must be the current context or sharing with it.");
        return false;
    }
    // map type from QFlags<QGLShader::ShaderTypeBit> to GL_VERTEX_SHADER/GL_FRAGMENT_SHADER
    GLint shtype;
    switch (type)
    {
        case QGLShader::Vertex:
                shtype = GL_VERTEX_SHADER; break;
        case QGLShader::Fragment:
                shtype = GL_FRAGMENT_SHADER; break;
        case QGLShader::Geometry:
                shtype = GL_EXT_geometry_shader4; break;
        default:
        m_log.append( QString( "invalid shader type %1" ).arg( type ) );
                qCritical() << "addShaderFromSourceCode: invalid shader type" << type;
                return false; break;
    }
    GLuint shader = glCreateShader( shtype );
    _glCheckError();
    QStringList codeLines = source.split( QRegExp( "\\r\\n|\\r|\\n" ) );
    int versPos = codeLines.indexOf( QRegExp( "^\\s{0,}#\\s{0,}version.*" ) );
    if ( versPos == -1 )
    {
        // no version specification found?
        //qDebug()  << "#version not found in shader source, prepending #defines";
        codeLines.prepend( QString( "#version 330\n#define lowp\n#define mediump\n#define highp" ) );
    }
    else
    {
        //qDebug()  << "#version found in shader source, inserting #defines at line" << versPos+1;
        codeLines.insert( versPos+1, QString( "#define lowp\n#define mediump\n#define highp" ) );
    }
    // otherwise #version is not found by compiler?
    codeLines.prepend( QString( "" ) );
    codeLines.append( QString( "" ) );
    QString fixedSource = codeLines.join( QString( "\n" ) );
    //qDebug() << "shader source after fixing:" << fixedSource.toUtf8().constData();
    const GLchar *strarr[] = { (const char *)fixedSource.toUtf8().constData() };

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

    glAttachShader( m_programId, shader );
    _glCheckError();
    return true;
}

int BugfixGLShaderProgram::attributeLocation(char const* name) const
{
    //qDebug() << "BugfixGLShaderProgram::attributeLocation";
    if (isLinked()) {
        return glGetAttribLocation(m_programId, name);
    } else {
        qWarning() << "QGLShaderProgram::attributeLocation(" << name
                   << "): shader program is not linked";
        return -1;
    }
}

bool BugfixGLShaderProgram::bind()
{
    //qDebug() << "BugfixGLShaderProgram::bind";
    /* use program object */
    if (m_context && !QGLContext::areSharing(m_context, QGLContext::currentContext()))
    {
        qWarning("bind: context must be the current context or sharing with it.");
        return false;
    }

    if (m_programId == 0)
    {
        qWarning() << "binding invalid shader program, results are undefined.";
    }

    if (!isLinked())
    {
        link();
        if (!isLinked())
            return false;
    }
    glUseProgram(m_programId);
    _glCheckError();
    return true;
}

void BugfixGLShaderProgram::enableAttributeArray(int location)
{
    //qDebug() << "BugfixGLShaderProgram::enableAttributeArray";
    if (location != -1)
         glEnableVertexAttribArray(location);
}

bool BugfixGLShaderProgram::isLinked() const
{
    //qDebug() << "BugfixGLShaderProgram::isLinked";
    return m_isLinked;
}

bool BugfixGLShaderProgram::link()
{
    //qDebug() << "BugfixGLShaderProgram::link";
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
    //qDebug() << "BugfixGLShaderProgram::log";
    return m_log;
}

GLuint BugfixGLShaderProgram::programId() const
{
    return m_programId;
}

void BugfixGLShaderProgram::setUniformValue(int location, QMatrix4x4 const& value)
{
    if (location == -1)
        return;
    if (sizeof(qreal) == sizeof(GLfloat)) {
        glUniformMatrix4fv(location, 1, GL_FALSE,
             reinterpret_cast<const GLfloat *>(value.constData()));
    } else {
        GLfloat mat[4 * 4];
        const qreal *data = value.constData();
        for (int i = 0; i < 4 * 4; ++i)
            mat[i] = data[i];
        glUniformMatrix4fv(location, 1, GL_FALSE, mat);
    }
}

void BugfixGLShaderProgram::setUniformValue(char const* name, QMatrix4x4 const& value)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
    setUniformValue(uniformLocation(name), value);
}

void BugfixGLShaderProgram::setUniformValue(int location, float value)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
    if (location != -1)
        glUniform1fv(location, 1, &value);
}

void BugfixGLShaderProgram::setUniformValue(char const* name, float value)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
    setUniformValue(uniformLocation(name), value);
}

void BugfixGLShaderProgram::setUniformValue(int location, float v1, float v2)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
    if (location != -1) {
        GLfloat values[2] = {v1, v2};
        glUniform2fv(location, 1, values);
    }
}

void BugfixGLShaderProgram::setUniformValue(char const* name, float v1, float v2)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
    setUniformValue(uniformLocation(name), v1, v2);
}

void BugfixGLShaderProgram::setUniformValue(int location, float v1, float v2, float v3)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
    if (location != -1) {
        GLfloat values[3] = {v1, v2, v3};
        glUniform3fv(location, 1, values);
    }
}

void BugfixGLShaderProgram::setUniformValue(char const* name, float v1, float v2, float v3)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
    setUniformValue(uniformLocation(name), v1, v2, v3);
}

void BugfixGLShaderProgram::setUniformValue(int location, float v1, float v2, float v3, float v4)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
    if (location != -1) {
        GLfloat values[4] = {v1, v2, v3, v4};
        glUniform4fv(location, 1, values);
    }
}

void BugfixGLShaderProgram::setUniformValue(char const* name, float v1, float v2, float v3, float v4)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
    setUniformValue(uniformLocation(name), v1, v2, v3, v4);
}

void BugfixGLShaderProgram::setUniformValue(int location, int value)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
    if (location != -1)
       glUniform1i(location, value);
}

void BugfixGLShaderProgram::setUniformValue(char const* name, int value)
{
    //qDebug() << "BugfixGLShaderProgram::setUniformValue";
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
#endif
