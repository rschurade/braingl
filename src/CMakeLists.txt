# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# General CMake Setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT( Fibernavigator2 )

# where to find our utils and cmake modules?
SET( FN_TOOLS_DIR ${PROJECT_SOURCE_DIR}/../tools )

# append search path for FindModules:
LIST( APPEND CMAKE_MODULE_PATH ${FN_TOOLS_DIR}/cmake )

OPTION( FN_VERBOSE_MAKEFILE "More output on compilation" OFF )
IF ( FN_VERBOSE_MAKEFILE MATCHES ON )
    set ( CMAKE_VERBOSE_MAKEFILE ON )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Some common setup
# ---------------------------------------------------------------------------------------------------------------------------------------------------
SET( BinName "fnav" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Setup QT4
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Package dependencies:
FIND_PACKAGE( Qt4 REQUIRED )
MARK_AS_ADVANCED( FORCE QT_QMAKE_EXECUTABLE )

# Includes:
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR}/QtGui )
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR}/QtCore )

# Libraries for linking:
SET( QT_LIBS ${QT_QTCORE_LIBRARY}
             ${QT_QTGUI_LIBRARY}
             ${QT_QTOPENGL_LIBRARY} )

# Resources
SET(QtIcon_RCCS resources.qrc)

IF( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
    ADD_DEFINITIONS( -DQT_DEBUG_OUTPUT )
    ADD_DEFINITIONS( -D__DEBUG__ )
ENDIF()

IF( ${CMAKE_BUILD_TYPE} STREQUAL "Release" )
    ADD_DEFINITIONS( -D__RELEASE__ )
ENDIF()


# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# REQUIRED third party libs
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------
FIND_PACKAGE( GLEW REQUIRED )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# OpenGL, at least 1.2
# See http://www.opengl.org
#
FIND_PACKAGE( OpenGL REQUIRED )

# include the OpenGL header paths
INCLUDE_DIRECTORIES( ${OPENGL_INCLUDE_DIR} )

ADD_DEFINITIONS( -DHAVE_ZLIB )

INCLUDE( Utils )

# find the boost packages
FIND_PACKAGE( Boost 1.46.0 )

FIND_PACKAGE( OpenMP )

# include the boost headers
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# CXX flags
# ---------------------------------------------------------------------------------------------------------------------------------------------------
set( CMAKE_CXX_FLAGS "-Wall -pedantic -ansi -std=c++0x -fopenmp" )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Add sources as target
# ---------------------------------------------------------------------------------------------------------------------------------------------------
SET ( CMAKE_AUTOMOC TRUE )
SET_PROPERTY( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/nifti/nifti1_io.cpp PROPERTY COMPILE_FLAGS "-Wno-write-strings -Wno-sign-compare -Wno-unused-but-set-variable")

IF( CMAKE_HOST_SYSTEM MATCHES "Linux" )
    ADD_DEFINITIONS( -D__LINUX__ )
	# Collect the compile-files for this target
	COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

    SET( GUI_QT4_MOC_HDRS ${TARGET_H_FILES} )

    # This is needed since the mocs will be generated there
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

    QT4_ADD_RESOURCES( QtIcon_RCC_SRCS ${QtIcon_RCCS} )
    
    #QT4_WRAP_CPP(FNAV_HEADERS_MOC ${GUI_QT4_MOC_HDRS} )

    ADD_EXECUTABLE( ${BinName} ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${FNAV_HEADERS_MOC} ${QtIcon_RCC_SRCS})

	# Some linux distributions need to explicitily link against X11. We add this lib here.
	SET( ADDITIONAL_TARGET_LINK_LIBRARIES "X11" )
	TARGET_LINK_LIBRARIES( ${BinName} ${GLEW_LIBRARY} ${QT_LIBS} )
ENDIF()

IF( CMAKE_HOST_SYSTEM MATCHES "Windows" )
    ADD_DEFINITIONS( -D__WINDOWS__ )
    # Collect the compile-files for this target
    COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

    SET( GUI_QT4_MOC_HDRS ${TARGET_H_FILES} )

	QT4_AUTOMOC( ${TARGET_CPP_FILES} )

    # This is needed since the mocs will be generated there
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

    QT4_ADD_RESOURCES( QtIcon_RCC_SRCS ${QtIcon_RCCS} )

    ADD_EXECUTABLE( ${BinName} ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${FNAV_HEADERS_MOC} ${QtIcon_RCC_SRCS})

	TARGET_LINK_LIBRARIES( ${BinName} ${QT_LIBS} ${GLEW_LIBRARY} z glu32 opengl32 )
ENDIF()

