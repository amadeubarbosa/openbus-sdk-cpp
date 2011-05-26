
cmake_minimum_required (VERSION 2.6)
project (openbus_sdk)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package (Mico)
if (NOT DEFINED OPENSSL_FOUND)
find_package (OpenSSL)
endif (NOT DEFINED OPENSSL_FOUND)
if (NOT DEFINED CORE_IDL_FOUND)
find_package(Core_Idl)
endif (NOT DEFINED CORE_IDL_FOUND)
find_package (Threads)

#if (NOT DEFINED Mico_FOUND)
#message (FATAL_ERROR "Must define MICO_ROOT_DIR")
#endif (NOT DEFINED Mico_FOUND)
#if (NOT DEFINED CORE_IDL_FOUND)
#message (FATAL_ERROR "Must defined CORE_IDL_ROOT_DIR or CORE_IDL_IDL_DIR")
#endif (NOT DEFINED CORE_IDL_FOUND)
find_package (Boost)

include_directories (include)
include_directories (${MICO_INCLUDE_DIR})
include_directories (${CMAKE_CURRENT_BINARY_DIR})
include_directories (${Boost_INCLUDE_DIRS})

mico_idl(${CORE_IDL_IDL_DIR}/core.idl)
mico_idl(${CORE_IDL_IDL_DIR}/access_control_service.idl)
mico_idl(${CORE_IDL_IDL_DIR}/openbus_security_manager.idl ARGS --any)

set(SOURCES src/security.cpp
            src/security_interceptors.cpp
            src/orb_initializer.cpp
            src/security_client_interceptor.cpp
            src/security_server_interceptor.cpp
            src/openbus_security_manager_impl.cpp
            ${CMAKE_CURRENT_BINARY_DIR}/openbus_security_manager.h
            ${CMAKE_CURRENT_BINARY_DIR}/openbus_security_manager.cc
            ${CMAKE_CURRENT_BINARY_DIR}/core.h
            ${CMAKE_CURRENT_BINARY_DIR}/core.cc
            ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.h
            ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.cc)

add_library(openbus_sdk_static STATIC ${SOURCES})
add_library(openbus_sdk_shared SHARED ${SOURCES})

target_link_libraries(openbus_sdk_static ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(openbus_sdk_shared ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(openbus_sdk_static PROPERTIES OUTPUT_NAME "sdkmico" PREFIX "lib")
set_target_properties(openbus_sdk_shared PROPERTIES OUTPUT_NAME "sdkmico" PREFIX "lib")
