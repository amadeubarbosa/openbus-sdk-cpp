
FIND_PATH(MICO_ROOT_DIR
  NAMES include/CORBA.h
)
MARK_AS_ADVANCED(MICO_ROOT_DIR)

FIND_PATH(MICO_INCLUDE_DIR
  NAMES CORBA.h
  PATHS ${MICO_ROOT_DIR}/include
)

FIND_PROGRAM(MICO_COMPILER_IDL idl
  PATHS ${MICO_ROOT_DIR}/bin
)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MICO DEFAULT_MSG
  MICO_LIBRARIES 
  MICO_INCLUDE_DIR
  MICO_COMPILER_IDL
)

FUNCTION (mico_idl idlfile)
  GET_FILENAME_COMPONENT(MICO_SOURCE_FILENAME ${idlfile} NAME)
  GET_FILENAME_COMPONENT(MICO_SOURCE_FILENAME_WE ${idlfile} NAME_WE)
  SET(MICO_IDL_PARAMS)
  MATH(EXPR CURRENT_ARG 1)
  # ARGS
  IF(${ARGC} GREATER "${CURRENT_ARG}" AND "${ARGV${CURRENT_ARG}}" STREQUAL "ARGS")
    MATH(EXPR CURRENT_ARG ${CURRENT_ARG}+1)
    WHILE((NOT "${CURRENT_ARG}" EQUAL "{ARGC}") AND (NOT ${ARGV${CURRENT_ARG}} STREQUAL "DEPENDS") AND (NOT ${ARGV${CURRENT_ARG}} STREQUAL "SUBDIR"))
      SET(MICO_IDL_ARGS ${MICO_IDL_ARGS} ${ARGV${CURRENT_ARG}})
      MATH(EXPR CURRENT_ARG ${CURRENT_ARG}+1)
    ENDWHILE((NOT "${CURRENT_ARG}" EQUAL "{ARGC}") AND (NOT ${ARGV${CURRENT_ARG}} STREQUAL "DEPENDS") AND (NOT ${ARGV${CURRENT_ARG}} STREQUAL "SUBDIR"))
  ENDIF(${ARGC} GREATER "${CURRENT_ARG}" AND "${ARGV${CURRENT_ARG}}" STREQUAL "ARGS")
  # DEPENDS
  IF("${ARGC}" GREATER "${CURRENT_ARG}" AND "${ARGV${CURRENT_ARG}}" STREQUAL "DEPENDS")
    MATH(EXPR CURRENT_ARG ${CURRENT_ARG}+1)
    WHILE(NOT ("${CURRENT_ARG}" STREQUAL "${ARGC}") AND (NOT ${ARGV{CURRENT_ARG}} STREQUAL "SUBDIR"))
      SET(MICO_IDL_DEPENDS ${MICO_IDL_DEPENDS} ${ARGV${CURRENT_ARG}})
      MATH(EXPR CURRENT_ARG ${CURRENT_ARG}+1)
    ENDWHILE(NOT ("${CURRENT_ARG}" STREQUAL "${ARGC}") AND (NOT ${ARGV{CURRENT_ARG}} STREQUAL "SUBDIR"))
  ENDIF("${ARGC}" GREATER "${CURRENT_ARG}" AND "${ARGV${CURRENT_ARG}}" STREQUAL "DEPENDS")
  # SUBDIR
  IF("${ARGC}" GREATER "${CURRENT_ARG}" AND "${ARGV${CURRENT_ARG}}" STREQUAL "SUBDIR")
    MATH(EXPR CURRENT_ARG ${CURRENT_ARG}+1)
    WHILE(NOT ("${CURRENT_ARG}" STREQUAL "${ARGC}"))
      SET(MICO_IDL_SUBDIR ${MICO_IDL_SUBDIR} ${ARGV${CURRENT_ARG}})
      MATH(EXPR CURRENT_ARG ${CURRENT_ARG}+1)
    ENDWHILE(NOT ("${CURRENT_ARG}" STREQUAL "${ARGC}"))
  ENDIF("${ARGC}" GREATER "${CURRENT_ARG}" AND "${ARGV${CURRENT_ARG}}" STREQUAL "SUBDIR")

  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MICO_IDL_SUBDIR}/${MICO_SOURCE_FILENAME}
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/${MICO_IDL_SUBDIR} && cp ${idlfile} ${CMAKE_CURRENT_BINARY_DIR}/${MICO_IDL_SUBDIR}/${MICO_SOURCE_FILENAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${idlfile}
  )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MICO_IDL_SUBDIR}/${MICO_SOURCE_FILENAME_WE}.h
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${MICO_IDL_SUBDIR}/${MICO_SOURCE_FILENAME_WE}.cc
    COMMAND ${MICO_COMPILER_IDL} ${MICO_IDL_ARGS} ${MICO_SOURCE_FILENAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${MICO_IDL_SUBDIR}
    MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/${MICO_IDL_SUBDIR}/${MICO_SOURCE_FILENAME}
    DEPENDS ${MICO_IDL_DEPENDS}
  )
ENDFUNCTION (mico_idl)

mark_as_advanced(MICO_LIBRARIES MICO_INCLUDE_DIR MICO_COMPILER_IDL)
