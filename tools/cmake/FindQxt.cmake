# Try to find GLEW library and include pathi. Once done this will define:
# QXT_FOUND
# QXT_INCLUDE_PATH
# QXT_CORE_LIB
# QXT_GUI_LIB

IF (WIN32)
    FIND_PATH( QXT_INCLUDE_PATH QxtCore
        $ENV{PROGRAMFILES}/QxtCore
        PATHS/include/
        DOC "The directory where QxtCore resides")
    FIND_LIBRARY( QXT_CORE_LIB
        NAMES QxtCore
        PATHS/lib
        $ENV{PROGRAMFILES}/lib
        DOC "The Qxt core library")
    FIND_LIBRARY( QXT_GUI_LIB
        NAMES QxtGui
        PATHS/lib
        $ENV{PROGRAMFILES}/lib
        DOC "The Qxt gui library")
ELSE (WIN32)
    FIND_PATH( GLEW_INCLUDE_PATH GL/glew.h
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        DOC "The directory where GL/glew.h resides")
    FIND_LIBRARY( GLEW_LIBRARY
        NAMES GLEW glew
        PATHS
        /usr/lib64
        /usr/lib
        /usr/local/lib64
        /usr/local/lib
        /sw/lib
        /opt/local/lib
        DOC "The GLEW library")
ENDIF (WIN32)

IF (QXT_INCLUDE_PATH)
    SET( QXT_FOUND 1 CACHE STRING "Set to 1 if QXT is found, 0 otherwise")
ELSE (QXT_INCLUDE_PATH)
    SET( QXT_FOUND 0 CACHE STRING "Set to 1 if QXT is found, 0 otherwise")
ENDIF (QXT_INCLUDE_PATH)

SET( QXT_LIBS ${QXT_CORE_LIB} ${QXT_GUI_LIB} )

MARK_AS_ADVANCED( QXT_FOUND )