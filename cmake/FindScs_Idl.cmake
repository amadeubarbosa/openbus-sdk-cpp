
FIND_PATH(SCS_IDL_ROOT_DIR
  NAMES src/scs.idl
)
MARK_AS_ADVANCED(SCS_IDL_ROOT_DIR)

FIND_PATH(SCS_IDL_IDL_DIR
  NAMES scs.idl
  PATHS ${SCS_IDL_ROOT_DIR}/src
)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SCS_IDL DEFAULT_MSG
  SCS_IDL_IDL_DIR
)

mark_as_advanced(SCS_IDL_IDL_DIR)
