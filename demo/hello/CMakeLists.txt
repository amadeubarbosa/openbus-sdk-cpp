
include_directories(${SCS_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

if (DEFINED MICO_FOUND)
  mico_idl (${CMAKE_CURRENT_SOURCE_DIR}/idl/hello.idl)

  mico_idl(${SCS_IDL_IDL_DIR}/scs.idl)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/core.idl)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/access_control_service.idl ARGS --any --typecode)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/registry_service.idl)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/fault_tolerance.idl)

  include_directories(mico ${CMAKE_CURRENT_BINARY_DIR} ${OPENBUS_INCLUDE_DIRECTORIES})

  add_executable (hello_client mico/client.cpp ${CMAKE_CURRENT_BINARY_DIR}/hello.h
    ${CMAKE_CURRENT_BINARY_DIR}/hello.cc
    ${CMAKE_CURRENT_BINARY_DIR}/core.h
    ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/scs.h
    ${CMAKE_CURRENT_BINARY_DIR}/registry_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/fault_tolerance.h)
  add_executable (hello_server mico/server.cpp ${CMAKE_CURRENT_BINARY_DIR}/hello.h
    ${CMAKE_CURRENT_BINARY_DIR}/hello.cc)

  target_link_libraries(hello_client ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_sdk_static openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
  target_link_libraries(hello_server ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_sdk_static openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
  
else (DEFINED MICO_FOUND)

endif (DEFINED MICO_FOUND)

