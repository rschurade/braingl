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

set(CMAKE_AUTOMOC TRUE)
#set ( CMAKE_VERBOSE_MAKEFILE ON )

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

# Libraries for linking:
SET( QT_LIBS ${QT_QTCORE_LIBRARY}
             ${QT_QTGUI_LIBRARY}
             ${QT_QTOPENGL_LIBRARY} )

# Resources
SET(QtIcon_RCCS icons.qrc)

# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# REQUIRED third party libs
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# -----------------------------------------------------------------------------------------------------------------------------------------------
# Boost, see http://www.boost.org/
#
# To see, which boost libs we currently use, you may run the following command # in the src directory on a linux box to make some investigations:
# grep -i include `find . -type f` | grep boost | awk '{print $2}' | sort | uniq

# Setup boost options
SET( Boost_USE_MULTITHREAD ON )

# find the boost packages
FIND_PACKAGE( Boost 1.46.0 REQUIRED program_options thread filesystem date_time system signals regex )

# include the boost headers
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# avoid filesystem 2 stuff
ADD_DEFINITIONS( "-DBOOST_FILESYSTEM_VERSION=3" )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# OpenGL, at least 1.2
# See http://www.opengl.org
#
FIND_PACKAGE( OpenGL REQUIRED )

# include the OpenGL header paths
INCLUDE_DIRECTORIES( ${OPENGL_INCLUDE_DIR} )


# -----------------------------------------------------------------------------------------------------------------------------------------------
# NiftiLib, at least 2.0.0
# See  http://niftilib.sourceforge.net
# FIND_PACKAGE( niftilib REQUIRED QUIET )
# IF( NIFTILIB_FOUND )
#   #MESSAGE( STATUS "Using system installed nifitlib." )
#   INCLUDE_DIRECTORIES( ${NIFTILIB_INCLUDE_DIR} )
# ENDIF()

# FIND_PACKAGE( ZLIB )
# this now works on win if we specify the zlib directory
# IF( ZLIB_FOUND )
#    ADD_DEFINITIONS( -DHAVE_ZLIB )
#    INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
# ENDIF( ZLIB_FOUND )

ADD_DEFINITIONS( -DHAVE_ZLIB )

INCLUDE( Utils )

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# GUI compilation options
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Add sources as target
# ---------------------------------------------------------------------------------------------------------------------------------------------------

# Collect the compile-files for this target
COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

# Qt4 Requires all classes with a QWidget stuff inside to be put into the MOC mechanism. We utilize the automoc mechanism here.
SET( GUI_QT4_MOC_HDRS ${TARGET_H_FILES} )
QT4_AUTOMOC( ${TARGET_CPP_FILES} )
# This is needed since the mocs will be generated there
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

QT4_ADD_RESOURCES(QtIcon_RCC_SRCS ${QtIcon_RCCS})

#QT4_WRAP_CPP(fnav_HEADERS_MOC ${GUI_QT4_MOC_HDRS} )

set_source_files_properties( ${CMAKE_CURRENT_SOURCE_DIR}/data/nifti/nifti1_io.cpp PROPERTIES COMPILE_FLAGS -Wno-write-strings)

ADD_EXECUTABLE( ${BinName} ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${fnav_HEADERS_MOC} ${QtIcon_RCC_SRCS})

IF( CMAKE_HOST_SYSTEM MATCHES "Linux" )
	# Some linux distributions need to explicitily link against X11. We add this lib here.
	SET( ADDITIONAL_TARGET_LINK_LIBRARIES "X11" )

	TARGET_LINK_LIBRARIES( ${BinName} ${QT_LIBS} ${Boost_LIBRARIES} )
ENDIF()

IF( CMAKE_HOST_SYSTEM MATCHES "Windows" )
	TARGET_LINK_LIBRARIES( ${BinName} ${QT_LIBS} glu32 opengl32 )
ENDIF()

