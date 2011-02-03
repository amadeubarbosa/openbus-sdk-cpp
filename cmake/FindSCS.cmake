
FIND_PATH(SCS_ROOT_DIR
  NAMES include/IComponent.h
)
MARK_AS_ADVANCED(SCS_ROOT_DIR)

FIND_PATH(SCS_INCLUDE_DIR
  NAMES IComponent.h
  PATHS ${SCS_ROOT_DIR}/include
)

FIND_PATH(SCS_INCLUDE_DIR
  NAMES scs.idl
  PATHS ${SCS_ROOT_DIR}/idl
)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SCS DEFAULT_MSG
  SCS_LIBRARIES 
  SCS_INCLUDE_DIR
  SCS_IDL_DIR
)

mark_as_advanced(SCS_LIBRARIES SCS_IDL_DIR)
