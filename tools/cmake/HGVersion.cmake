cmake_minimum_required (VERSION 2.8)

macro ( ReadProjectRevisionStatus )
message(STATUS "GETTING HG VERSION")
#message( STATUS PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR} )

#exec_program( hg ${PROJECT_SOURCE_DIR} ARGS paths OUTPUT_VARIABLE ${PROJECT_NAME}_HGPATHS )
#message( STATUS "${PROJECT_NAME}_HGPATHS=${${PROJECT_NAME}_HGPATHS}}" )
exec_program( hg ${PROJECT_SOURCE_DIR} ARGS tip --template='{rev}:{node|short}\n' OUTPUT_VARIABLE __HGTIP__ )
message(STATUS "__HGTIP__=${__HGTIP__}")

SET( CHANGESET  "\"" ${__HGTIP__} "\"" )

#string( REPLACE "\n" ";" __HGTIP__ ${__HGTIP__} )
#foreach(HGTIP ${__HGTIP__})
#    string(SUBSTRING "${HGTIP}" 0 11 HGTIPSTART)
#    if (HGTIPSTART MATCHES "changeset: ")
#        SET( CHANGESET  "\"" ${HGTIP} "\"" )
#        message(STATUS "${HGTIP}")
#    endif()
#endforeach()

add_custom_target(buildheader ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/buildtime.h)
file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/buildtime.h )
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/buildtime.h "#define BUILDTIME __TIME__\n#define BUILDDATE __DATE__\n#define HGTIP ${CHANGESET}\n")
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/buildtime.h PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
add_dependencies(${BinName} buildheader)

endmacro()
