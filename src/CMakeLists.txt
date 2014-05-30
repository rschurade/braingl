# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# General CMake Setup
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

SET( CMAKE_DISABLE_SOURCE_CHANGES ON )
SET( CMAKE_DISABLE_IN_SOURCE_BUILD ON )

PROJECT( brainGL )

# where to find our utils and cmake modules?
SET( FN_TOOLS_DIR ${PROJECT_SOURCE_DIR}/../tools )

# append search path for FindModules:
LIST( APPEND CMAKE_MODULE_PATH ${FN_TOOLS_DIR}/cmake )

OPTION( FN_VERBOSE_MAKEFILE "More output on compilation" OFF )
IF ( FN_VERBOSE_MAKEFILE MATCHES ON )
    set ( CMAKE_VERBOSE_MAKEFILE ON )
ENDIF()

IF (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Some common setup
# ---------------------------------------------------------------------------------------------------------------------------------------------------
IF( CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin" )
    SET( BinName "${PROJECT_NAME}" )
ELSE()
    SET( BinName "braingl" )
ENDIF()

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
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR}/QtOpenGL )
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR}/QtWebKit )
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR}/QtNetwork )
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR}/QtXml )

# Libraries for linking:
SET( QT_LIBS ${QT_QTCORE_LIBRARY}
                 ${QT_QTGUI_LIBRARY}
                 ${QT_QTOPENGL_LIBRARY} 
                 ${QT_QTWEBKIT_LIBRARY}
                 ${QT_QTNETWORK_LIBRARY}
                 ${QT_QTXML_LIBRARY} )

# Resources
SET(QtIcon_RCCS resources.qrc)

IF( ${CMAKE_BUILD_TYPE} MATCHES "Debug" )
    ADD_DEFINITIONS( -DQT_DEBUG_OUTPUT )
    ADD_DEFINITIONS( -D__DEBUG__ )
ENDIF()

IF( ${CMAKE_BUILD_TYPE} MATCHES "Release" )
    ADD_DEFINITIONS( -D__RELEASE__ )
ENDIF()


# ---------------------------------------------------------------------------------------------------------------------------------------------------
#
# REQUIRED third party libs
#
# ---------------------------------------------------------------------------------------------------------------------------------------------------

FIND_PACKAGE( GLEW REQUIRED )
INCLUDE_DIRECTORIES( ${GLEW_INCLUDE_PATH} )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# OpenGL, at least 1.2
# See http://www.opengl.org
#
FIND_PACKAGE( OpenGL REQUIRED )

# include the OpenGL header paths
INCLUDE_DIRECTORIES( ${OPENGL_INCLUDE_DIR} )

ADD_DEFINITIONS( -DHAVE_ZLIB )

INCLUDE( Utils )
INCLUDE( HGVersion )

# find the boost packages
FIND_PACKAGE( Boost 1.46.0 )

# include the boost headers
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

# -----------------------------------------------------------------------------------------------------------------------------------------------
# VTK
# 
FIND_PACKAGE( VTK REQUIRED )
include(${VTK_USE_FILE})

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# CXX flags
# ---------------------------------------------------------------------------------------------------------------------------------------------------
IF( CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin" )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++11 -stdlib=libc++" )
ELSE()
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++0x" )
ENDIF()

# ---------------------------------------------------------------------------------------------------------------------------------------------------
# Add sources as target
# ---------------------------------------------------------------------------------------------------------------------------------------------------
SET_PROPERTY( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/nifti/nifti1_io.cpp PROPERTY COMPILE_FLAGS "-Wno-write-strings -Wno-sign-compare -Wno-unused-but-set-variable")
SET_PROPERTY( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cnifti/CiftiXMLReader.cxx PROPERTY COMPILE_FLAGS "-Wstrict-overflow")
SET_PROPERTY( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/data/writervtk.cpp PROPERTY COMPILE_FLAGS "-Wno-deprecated" )
SET_PROPERTY( SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/data/loadervtk.cpp PROPERTY COMPILE_FLAGS "-Wno-deprecated" )
SET ( CMAKE_AUTOMOC TRUE )

# Collect the compile-files for this target
COLLECT_COMPILE_FILES( "${CMAKE_CURRENT_SOURCE_DIR}" TARGET_CPP_FILES TARGET_H_FILES TARGET_TEST_FILES )

QT4_AUTOMOC( ${TARGET_CPP_FILES} )    
# This is needed since the mocs will be generated there
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cifti )
QT4_ADD_RESOURCES( QtIcon_RCC_SRCS ${QtIcon_RCCS} )

IF( NOT CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin" )
    # XXX just add the Mac-specific dependencies later?
    ADD_EXECUTABLE( ${BinName} MACOSX_BUNDLE ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${QtIcon_RCC_SRCS})

    ReadProjectRevisionStatus()
ENDIF()

IF (CMAKE_VERBOSE_MAKEFILE)
      MESSAGE(STATUS)
      MESSAGE(STATUS "*******************************************************************************")
      MESSAGE(STATUS "*        SUMMARY OF USED VARIABLES -> Details in AllVariables.txt             *")
      MESSAGE(STATUS "*******************************************************************************")
IF(CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin" )
          MESSAGE(STATUS "COMPILING FOR OSX, platforms: ${CMAKE_OSX_ARCHITECTURES}")
ENDIF()
      MESSAGE(STATUS "CMAKE_ROOT ........................... = ${CMAKE_ROOT}")
      MESSAGE(STATUS "CMAKE_HOME_DIRECTORY ................. = ${CMAKE_HOME_DIRECTORY}")
      MESSAGE(STATUS "CMAKE_VERSION ........................ = ${CMAKE_VERSION}")
      MESSAGE(STATUS "CMAKE_GENERATOR ...................... = ${CMAKE_GENERATOR}")
      MESSAGE(STATUS "CMAKE_MODULE_PATH .................... = ${CMAKE_MODULE_PATH}")
      MESSAGE(STATUS "CMAKE_HOST_SYSTEM .................... = ${CMAKE_HOST_SYSTEM}")
      MESSAGE(STATUS "CMAKE_HOST_SYSTEM_VERSION ............ = ${CMAKE_HOST_SYSTEM_VERSION}")
      MESSAGE(STATUS "CMAKE_HOST_SYSTEM_NAME ............... = ${CMAKE_HOST_SYSTEM_NAME}")
      MESSAGE(STATUS "CMAKE_HOST_SYSTEM_PROCESSOR .......... = ${CMAKE_HOST_SYSTEM_PROCESSOR}")
      MESSAGE(STATUS "CMAKE_HOST_UNIX ...................... = ${CMAKE_HOST_UNIX}")
      MESSAGE(STATUS "CMAKE_BUILD_TYPE ..................... = ${CMAKE_BUILD_TYPE}")
      MESSAGE(STATUS "CMAKE_C_COMPILER ..................... = ${CMAKE_C_COMPILER}")
      MESSAGE(STATUS "CMAKE_CXX_COMPILER ................... = ${CMAKE_CXX_COMPILER}")
      MESSAGE(STATUS "CMAKE_CXX_FLAGS ...................... = ${CMAKE_CXX_FLAGS}")
      MESSAGE(STATUS "CMAKE_CURRENT_SOURCE_DIR ............. = ${CMAKE_CURRENT_SOURCE_DIR}")
      MESSAGE(STATUS "CMAKE_CURRENT_BINARY_DIR ............. = ${CMAKE_CURRENT_BINARY_DIR}")
      MESSAGE(STATUS "CMAKE_LIBRARY_PATH ................... = ${CMAKE_LIBRARY_PATH}")          
ENDIF()


IF( CMAKE_HOST_SYSTEM MATCHES "Linux" )
    ADD_DEFINITIONS( -D__LINUX__ )
    
	# Some linux distributions need to explicitily link against X11. We add this lib here.
	SET( ADDITIONAL_TARGET_LINK_LIBRARIES "X11" )
	TARGET_LINK_LIBRARIES( ${BinName} ${QT_LIBS}  ${GLEW_LIBRARY} ${VTK_LIBRARIES} GLU )
ENDIF()

IF( CMAKE_HOST_SYSTEM MATCHES "Windows" )
    ADD_DEFINITIONS( -D__WINDOWS__ )
    
    OPTION( SURPRESS_CMD_WINDOW "No cmd window" OFF )
    IF ( SURPRESS_CMD_WINDOW MATCHES ON )
        SET(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -mwindows")
    ENDIF()
    
	TARGET_LINK_LIBRARIES( ${BinName} ${QT_LIBS} ${GLEW_LIBRARY} ${VTK_LIBRARIES} z glu32 opengl32 )
ENDIF()

IF( CMAKE_HOST_SYSTEM_NAME MATCHES "Darwin" )

    # XXX move to platform-independent settings?
    SET( TARGET_CPP_FILES ${TARGET_CPP_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/gui/core_profile_3_2_attributes.mm" )

    SET( MACOSX_RESOURCE_FILES ${CMAKE_SOURCE_DIR}/icons/${BinName}.icns )
    SET_SOURCE_FILES_PROPERTIES( ${CMAKE_SOURCE_DIR}/icons/${BinName}.icns
                               PROPERTIES MACOSX_PACKAGE_LOCATION Resources )

    # XXX merge with definition above
    ADD_EXECUTABLE( ${BinName} MACOSX_BUNDLE ${TARGET_CPP_FILES} ${TARGET_H_FILES} ${FNAV_HEADERS_MOC} ${QtIcon_RCC_SRCS} ${MACOSX_RESOURCE_FILES} )

    # XXX ditto
    ReadProjectRevisionStatus()

    SET( MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME} - Version ${__HGTIP__}" )
    # XXX value should be formed like 1.2.3
    SET( MACOSX_BUNDLE_BUNDLE_VERSION ${__HGTIP__} )
    SET( MACOSX_BUNDLE_ICON_FILE "${BinName}.icns" )
    SET( MACOSX_BUNDLE_GUI_IDENTIFIER "de.braingl" )
    SET( MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT_NAME}" )

    TARGET_LINK_LIBRARIES( ${BinName} ${QT_LIBS} ${GLEW_LIBRARY} ${OPENGL_LIBRARY} ${VTK_LIBRARIES} "-framework Foundation" "-framework Cocoa" z)

    SET( BUNDLED_DIR ${CMAKE_CURRENT_BINARY_DIR}/selfcontained )

    # create self-contained application bundle with embedded Frameworks
    # and dylibs
    INSTALL( CODE "
      INCLUDE( BundleUtilities )
      FILE( REMOVE_RECURSE ${BUNDLED_DIR} )
      COPY_AND_FIXUP_BUNDLE( ${BinName}.app ${BUNDLED_DIR}/${BinName}.app \"\" \"\" ) 
    ")

    # create distribution disk image (`package´ build target)
    SET( CPACK_GENERATOR "DragNDrop" )
    SET( CPACK_INSTALLED_DIRECTORIES ${BUNDLED_DIR} . )
    INCLUDE( CPack )

ENDIF()

