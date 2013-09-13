# This function converts a given filename to a proper target name. This is very useful if you want to define custom targets for
# files and need a unique name.
# _filename the filename to convert
# _target returns the proper target string
FUNCTION( FILE_TO_TARGETSTRING _filename _target )
    # strip the whole path up to src
    STRING( REGEX REPLACE "^.*/src" "src" fileExcaped "${_filename}" ) 

    # remove all those ugly chars
    STRING( REGEX REPLACE "[^A-Za-z0-9]" "X" fileExcaped "${fileExcaped}" )

    # done. Return value
    SET( ${_target} "${fileExcaped}" PARENT_SCOPE )
ENDFUNCTION( FILE_TO_TARGETSTRING )

# Gets the major, minor and patch number from a version string.
# _VersionString the string to split
# _Major the major version number - result
# _Minor the minor version number - result
# _Patch the patch number - result
FUNCTION( SPLIT_VERSION_STRING _VersionString _Major _Minor _Patch )
  STRING( STRIP _VersionString ${_VersionString} )
  STRING( REGEX MATCH "^[0-9]+" _MajorProxy "${_VersionString}" )
  STRING( REGEX MATCH "[^0-9][0-9]+[^0-9]" _MinorProxy "${_VersionString}" )
  STRING( REGEX MATCH "[0-9]+" _MinorProxy "${_MinorProxy}" )
  STRING( REGEX MATCH "[0-9]+$" _PatchProxy "${_VersionString}" )
  SET( ${_Major} "${_MajorProxy}" PARENT_SCOPE )
  SET( ${_Minor} "${_MinorProxy}" PARENT_SCOPE )
  SET( ${_Patch} "${_PatchProxy}" PARENT_SCOPE )
ENDFUNCTION( SPLIT_VERSION_STRING )

# Recursively searches compile files (headers, sources).
# _DirString: where to search
# _CPPFiles contains the cpp files afterwards
# _HFiles contains the h files afterwards, without tests
# _TestFiles contains only the test headers
FUNCTION( COLLECT_COMPILE_FILES _DirString _CPPFiles _HFiles _TestFiles )
    # recursively get all files
    FILE( GLOB_RECURSE CPP_FILES ${_DirString}/*.cpp ${_DirString}/*.cxx )
    FILE( GLOB_RECURSE H_FILES   ${_DirString}/*.h )
    FILE( GLOB_RECURSE TEST_FILES   ${_DirString}/*_test.h )

    # exclude some special dirs
    FOREACH( file ${H_FILES} )
        # the test directories should be excluded from normal compilation completely
        STRING( REGEX MATCH "^.*\\/test\\/.*" IsTest "${file}" )
        # ext sources should be build seperatly 
        STRING( REGEX MATCH "^.*\\/ext\\/.*" IsExternal "${file}" )
        IF( IsTest )
            LIST( REMOVE_ITEM H_FILES ${file} )
        ENDIF()
        IF( IsExternal )
            LIST( REMOVE_ITEM H_FILES ${file} )
        ENDIF()
    ENDFOREACH( file )
    FOREACH( file ${CPP_FILES} )
        # the test directories should be excluded from normal compilation completely
        STRING( REGEX MATCH "^.*\\/test\\/.*" IsTest "${file}" )
        # ext sources should be build seperatly 
        STRING( REGEX MATCH "^.*\\/ext\\/.*" IsExternal "${file}" )
        IF( IsTest )
            LIST( REMOVE_ITEM CPP_FILES ${file} )
        ENDIF()
        IF( IsExternal )
            LIST( REMOVE_ITEM CPP_FILES ${file} )
        ENDIF()
    ENDFOREACH( file )

    SET( ${_CPPFiles} "${CPP_FILES}" PARENT_SCOPE )
    SET( ${_HFiles} "${H_FILES}" PARENT_SCOPE )
    SET( ${_TestFiles} "${TEST_FILES}" PARENT_SCOPE )
ENDFUNCTION( COLLECT_COMPILE_FILES )
