#ifndef CORE_3_2_CONTEXT_H
#define CORE_3_2_CONTEXT_H

#include <QGLContext>

#if defined(Q_OS_MAC)
#if QT_VERSION <= 0x040805 && QT_VERSION >= 0x040800    // if less or equal to 4.8.5
void* select_3_2_mac_visual(GDHandle handle, int depthBufferSize);
#else
#error __FILE__ __LINE__ This Qt version is not supported.
#endif
#endif

struct core_3_2_context : public QGLContext {
    core_3_2_context(const QGLFormat& format, QPaintDevice* device) : QGLContext(format,device) {}
    core_3_2_context(const QGLFormat& format) : QGLContext(format) {}

#if defined(Q_OS_MAC)
#if QT_VERSION <= 0x040805 && QT_VERSION >= 0x040800    // if less or equal to 4.8.5
    virtual void* chooseMacVisual(GDHandle handle) {
        return select_3_2_mac_visual(handle, this->format().depthBufferSize());
    }
#endif
#endif
};

#endif // CORE_3_2_CONTEXT_H
