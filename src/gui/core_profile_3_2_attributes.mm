#include <QGLContext>

#if defined(Q_OS_MAC)
#if QT_VERSION <= 0x040805 && QT_VERSION >= 0x040800    // if less or equal to 4.8.5
void* select_3_2_mac_visual(GDHandle handle, int depthBufferSize)
{
    static const int Max = 40;
    NSOpenGLPixelFormatAttribute attribs[Max];
    int cnt = 0;

    attribs[cnt++] = NSOpenGLPFAOpenGLProfile;
    attribs[cnt++] = NSOpenGLProfileVersion3_2Core;

    attribs[cnt++] = NSOpenGLPFADoubleBuffer;

    if (depthBufferSize != -1)
    {
        attribs[cnt++] = NSOpenGLPFADepthSize;
        attribs[cnt++] = (NSOpenGLPixelFormatAttribute)(depthBufferSize==-1)?32:depthBufferSize;
    }

    attribs[cnt] = 0;
    Q_ASSERT(cnt < Max);

    return [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
}
#else
#error __FILE__ __LINE__ This Qt version is not supported.
#endif
#endif
