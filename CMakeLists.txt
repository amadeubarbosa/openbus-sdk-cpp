
cmake_minimum_required (VERSION 2.6)
project (openbus_sdk_cpp)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

enable_testing()

find_package (Mico)
find_package (OpenSSL)
find_package (Boost 1.44.0)
find_package (SCS)
find_package (core_idl)

if (NOT MICO_FOUND)
   message(FATAL_ERROR "Mico is required to build the Openbus SDK C++, please define MICO_ROOT_DIR")
endif (NOT MICO_FOUND)

if (NOT OPENSSL_FOUND)
   message(FATAL_ERROR "OpenSSL is required to build the Openbus SDK C++, please define OPENSSL_ROOT_DIR")
endif (NOT OPENSSL_FOUND)

if (NOT Boost_FOUND)
   message(FATAL_ERROR "Boost is required to build the Openbus SDK C++, please define BOOST_ROOT")
endif (NOT Boost_FOUND)

if (NOT SCS_FOUND)
   message(FATAL_ERROR "SCS is required to build the Openbus SDK C++, please define SCS_ROOT_DIR")
endif (NOT SCS_FOUND)

if (NOT CORE_IDL_FOUND)
   message(FATAL_ERROR "CORE IDL is required to build the Openbus SDK C++, please define CORE_IDL_ROOT_DIR")
endif (NOT CORE_IDL_FOUND)

include_directories (include)
include_directories (${MICO_INCLUDE_DIR})
include_directories (${CMAKE_CURRENT_BINARY_DIR})
include_directories (${Boost_INCLUDE_DIR})
include_directories (${SCS_INCLUDE_DIR})
include_directories (${INCLUDE_DIRECTORIES})

add_definitions(-DOPENBUS_IDL_VERSION=${OPENBUS_IDL_VERSION})

mico_idl(${SCS_IDL_DIR}/scs.idl)
mico_idl(${CORE_IDL_IDL_DIR}/core.idl)
mico_idl(${CORE_IDL_IDL_DIR}/registry_service.idl
         DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/scs.h)
mico_idl(${CORE_IDL_IDL_DIR}/access_control_service.idl
        ARGS --any --typecode DEPENDS
         core.h)
mico_idl(${CORE_IDL_IDL_DIR}/fault_tolerance.idl)

set(SOURCES src/ClientInterceptor.cpp
            src/ServerInterceptor.cpp
            src/ORBInitializerImpl.cpp
            src/openbus.cpp
            src/Helper.cpp
            src/FaultToleranceManager.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.h
            ${CMAKE_CURRENT_BINARY_DIR}/registry_service.h
            ${CMAKE_CURRENT_BINARY_DIR}/fault_tolerance.h
            ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.cc
            ${CMAKE_CURRENT_BINARY_DIR}/registry_service.cc
            ${CMAKE_CURRENT_BINARY_DIR}/fault_tolerance.cc)

add_library(openbus_sdk_static STATIC ${SOURCES})
add_library(openbus_sdk_shared SHARED ${SOURCES})

target_link_libraries(openbus_sdk_static ${MICO_LIBRARIES})
target_link_libraries(openbus_sdk_shared ${MICO_LIBRARIES})
set_target_properties(openbus_sdk_static PROPERTIES OUTPUT_NAME "openbusmico" PREFIX "lib")
set_target_properties(openbus_sdk_shared PROPERTIES OUTPUT_NAME "openbusmico" PREFIX "lib")

add_subdirectory(test/mico/acs)
