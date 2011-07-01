
FIND_PATH(SCS_ROOT_DIR
  NAMES include/scs/ComponentContext.h
)
MARK_AS_ADVANCED(SCS_ROOT_DIR)

FIND_PATH(SCS_INCLUDE_DIR
  NAMES scs/ComponentContext.h
  PATHS ${SCS_ROOT_DIR}/include
)

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SCS DEFAULT_MSG
  SCS_INCLUDE_DIR
  SCS_LIBRARIES
)

mark_as_advanced(SCS_INCLUDE_DIR)
