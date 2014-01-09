cmake_minimum_required (VERSION 2.8)

macro ( ReadProjectRevisionStatus )
message(STATUS "GETTING HG VERSION")
#message( STATUS PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR} )

execute_process(
    COMMAND hg tip --template "{rev}:{node|short}"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_VARIABLE __HGTIP__
    OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "__HGTIP__=${__HGTIP__}")

SET( CHANGESET  "\"" ${__HGTIP__} "\"" )

add_custom_target(buildheader ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/buildtime.h)
file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/buildtime.h )
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/buildtime.h "#define BUILDTIME __TIME__\n#define BUILDDATE __DATE__\n#define HGTIP ${CHANGESET}\n")
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/buildtime.h PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
add_dependencies(${BinName} buildheader)

endmacro()
