
FIND_PATH(OPENBUS_CORE_IDL_ROOT_DIR
  NAMES src/access_control_service.idl
)
MARK_AS_ADVANCED(OPENBUS_CORE_IDL_ROOT_DIR)

FIND_PATH(OPENBUS_CORE_IDL_IDL_DIR
  NAMES access_control_service.idl
  PATHS ${OPENBUS_CORE_IDL_ROOT_DIR}/src
)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OPENBUS_CORE_IDL DEFAULT_MSG
  OPENBUS_CORE_IDL_IDL_DIR
)

mark_as_advanced(OPENBUS_CORE_IDL_IDL_DIR)
