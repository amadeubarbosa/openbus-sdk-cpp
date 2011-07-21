
cmake_minimum_required (VERSION 2.6)
project (openbus_sdk)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package (Mico)
if (NOT MICO_FOUND OR DEFINED OPENBUS_SDK_USE_ORBIX)
find_package (Orbix)
if (NOT ORBIX_FOUND)
message(FATAL "Mico nor Orbix was found")
endif (NOT ORBIX_FOUND)
endif (NOT MICO_FOUND OR DEFINED OPENBUS_SDK_USE_ORBIX)
if (NOT OPENSSL_FOUND)
find_package (OpenSSL)
endif (NOT OPENSSL_FOUND)
if (NOT OPENBUS_CORE_IDL_FOUND)
find_package (Core_Idl)
endif (NOT OPENBUS_CORE_IDL_FOUND)
if (NOT SCS_IDL_FOUND)
find_package (Scs_Idl)
endif (NOT SCS_IDL_FOUND)
find_package (Threads)

find_package (Lua51)
find_package (Scs)

include_directories (.)
include_directories (${LOGGER_ROOT})
include_directories (${CMAKE_CURRENT_BINARY_DIR})

if (MICO_FOUND)
  include_directories (${MICO_INCLUDE_DIR})
  include_directories (${CMAKE_CURRENT_BINARY_DIR}/stubs/mico)
  add_definitions(-DOPENBUS_MICO)
  add_definitions(-DSCS_MICO)

  mico_idl(${SCS_IDL_IDL_DIR}/scs.idl SUBDIR stubs/mico)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/core.idl SUBDIR stubs/mico)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/access_control_service.idl ARGS --any --typecode
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/core.idl
            ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/scs.idl
    SUBDIR stubs/mico)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/registry_service.idl 
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/core.idl
            ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/access_control_service.idl
    SUBDIR stubs/mico)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/fault_tolerance.idl
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/core.idl
            ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/access_control_service.idl
    SUBDIR stubs/mico)
  set(SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/scs.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/registry_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/core.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/access_control_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/fault_tolerance.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/scs.cc
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/core.cc
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/access_control_service.cc
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/fault_tolerance.cc
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/registry_service.cc)

else (MICO_FOUND)
  add_definitions(-DOPENBUS_ORBIX)
  add_definitions(-DSCS_ORBIX)
  include_directories (${ORBIX_INCLUDE_DIR})

   orbix_idl(${SCS_IDL_IDL_DIR}/scs.idl ARGS -base -poa
     SUBDIR stubs/orbix)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/core.idl ARGS -base -poa
     SUBDIR stubs/orbix)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/access_control_service.idl ARGS -base -poa
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/core.idl
            ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/scs.idl
     SUBDIR stubs/orbix)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/registry_service.idl ARGS -base -poa
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/core.idl
            ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/access_control_service.idl
     SUBDIR stubs/orbix)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/fault_tolerance.idl ARGS -base -poa
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/core.idl
            ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/access_control_service.idl
     SUBDIR stubs/orbix)

   set(SOURCES
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/scsS.hh
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/scs.hh
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/scsC.cxx
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/scsS.cxx
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/coreS.hh
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/core.hh
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/coreC.cxx
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/coreS.cxx
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/access_control_serviceS.hh
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/access_control_service.hh
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/access_control_serviceC.cxx
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/access_control_serviceS.cxx
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/registry_serviceS.hh
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/registry_service.hh
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/registry_serviceC.cxx
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/registry_serviceS.cxx
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/fault_toleranceS.hh
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/fault_tolerance.hh
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/fault_toleranceC.cxx
     ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/fault_toleranceS.cxx)

endif (MICO_FOUND)

set(SOURCES ${SOURCES}
  openbus.cpp
  FaultToleranceManager.cpp
  openbus/interceptors/ServerInterceptor.cpp
  openbus/interceptors/ClientInterceptor.cpp
  openbus/interceptors/ORBInitializerImpl.cpp
  openbus/util/Helper.cpp)

add_library(openbus_logger_static STATIC ${LOGGER_ROOT}/logger.cpp)
add_library(openbus_logger_shared STATIC ${LOGGER_ROOT}/logger.cpp)

add_library(openbus_sdk_static STATIC ${SOURCES})
add_library(openbus_sdk_shared SHARED ${SOURCES})


if (MICO_FOUND)
  target_link_libraries(openbus_sdk_static ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_logger_static)
  target_link_libraries(openbus_sdk_shared ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_logger_shared)

  set_target_properties(openbus_sdk_static PROPERTIES OUTPUT_NAME "sdkmico" PREFIX "lib")
  set_target_properties(openbus_sdk_shared PROPERTIES OUTPUT_NAME "sdkmico" PREFIX "lib")
else (MICO_FOUND)
  target_link_libraries(openbus_sdk_static ${ORBIX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_logger_static)
  target_link_libraries(openbus_sdk_shared ${ORBIX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_logger_shared)

  set_target_properties(openbus_sdk_static PROPERTIES OUTPUT_NAME "sdkorbix" PREFIX "lib")
  set_target_properties(openbus_sdk_shared PROPERTIES OUTPUT_NAME "sdkorbix" PREFIX "lib")
endif (MICO_FOUND)

enable_testing()

#add_subdirectory(demo)
#add_subdirectory(test)
