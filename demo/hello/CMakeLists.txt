
include_directories(${SCS_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

if (MICO_FOUND)
  mico_idl (${CMAKE_CURRENT_SOURCE_DIR}/idl/hello.idl SUBDIR stubs)

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

  include_directories(mico ${CMAKE_CURRENT_BINARY_DIR} ${OPENBUS_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_BINARY_DIR}/stubs ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico)

  add_executable (hello_client mico/client.cpp ${CMAKE_CURRENT_BINARY_DIR}/stubs/hello.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/hello.cc
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/core.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/access_control_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/scs.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/registry_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/mico/fault_tolerance.h)
  add_executable (hello_server mico/server.cpp ${CMAKE_CURRENT_BINARY_DIR}/stubs/hello.h
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/hello.cc)

  target_link_libraries(hello_client ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_sdk_static openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
  target_link_libraries(hello_server ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_sdk_static openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
  
else (MICO_FOUND)
  orbix_idl (${CMAKE_CURRENT_SOURCE_DIR}/idl/hello.idl ARGS -base -poa SUBDIR stubs)

   orbix_idl(${SCS_IDL_IDL_DIR}/scs.idl ARGS -base -poa)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/core.idl ARGS -base -poa SUBDIR stubs/orbix)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/access_control_service.idl ARGS -base -poa SUBDIR stubs/orbix)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/registry_service.idl ARGS -base -poa SUBDIR stubs/orbix)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/fault_tolerance.idl ARGS -base -poa SUBDIR stubs/orbix)

  include_directories(orbix ${CMAKE_CURRENT_BINARY_DIR} ${OPENBUS_INCLUDE_DIRECTORIES})

  add_executable (hello_client orbix/client.cpp ${CMAKE_CURRENT_BINARY_DIR}/stubs/hello.hh
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/helloS.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/helloC.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/core.hh
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/access_control_service.hh
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/scs.hh
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/registry_service.hh
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/orbix/fault_tolerance.hh)
  add_executable (hello_server orbix/server.cpp ${CMAKE_CURRENT_BINARY_DIR}/stubs/hello.hh
    ${CMAKE_CURRENT_BINARY_DIR}/stubs/helloC.cxx ${CMAKE_CURRENT_BINARY_DIR}/stubs/helloS.cxx)

  target_link_libraries(hello_client ${ORBIX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_sdk_static openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
  target_link_libraries(hello_server ${ORBIX_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_sdk_static openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
endif (MICO_FOUND)

