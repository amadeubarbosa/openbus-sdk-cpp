
FIND_PATH(CORE_IDL_ROOT_DIR
  NAMES src/access_control_service.idl
)
MARK_AS_ADVANCED(CORE_IDL_ROOT_DIR)

FIND_PATH(CORE_IDL_IDL_DIR
  NAMES access_control_service.idl
  PATHS ${CORE_IDL_ROOT_DIR}/src
)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CORE_IDL DEFAULT_MSG
  CORE_IDL_IDL_DIR
)

mark_as_advanced(CORE_IDL_IDL_DIR)
