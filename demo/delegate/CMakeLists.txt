
include_directories(${SCS_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

if (MICO_FOUND)
  mico_idl (${CMAKE_CURRENT_SOURCE_DIR}/idl/delegate.idl)

  mico_idl(${SCS_IDL_IDL_DIR}/scs.idl)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/core.idl)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/access_control_service.idl ARGS --any --typecode)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/registry_service.idl)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/fault_tolerance.idl)

  include_directories(mico ${CMAKE_CURRENT_BINARY_DIR} ${OPENBUS_INCLUDE_DIRECTORIES})

  add_executable (delegate_client multithread/mico/client.cpp ${CMAKE_CURRENT_BINARY_DIR}/delegate.h
    ${CMAKE_CURRENT_BINARY_DIR}/delegate.cc
    ${CMAKE_CURRENT_BINARY_DIR}/core.h
    ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/scs.h
    ${CMAKE_CURRENT_BINARY_DIR}/registry_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/fault_tolerance.h)
  add_executable (delegate_server multithread/mico/server.cpp ${CMAKE_CURRENT_BINARY_DIR}/delegate.h
    ${CMAKE_CURRENT_BINARY_DIR}/delegate.cc)

  target_link_libraries(delegate_client ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_sdk_static openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
  target_link_libraries(delegate_server ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_sdk_static openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
  
else (MICO_FOUND)
  orbix_idl (${CMAKE_CURRENT_SOURCE_DIR}/idl/delegate.idl ARGS -base -poa)

   orbix_idl(${SCS_IDL_IDL_DIR}/scs.idl ARGS -base -poa)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/core.idl ARGS -base -poa)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/access_control_service.idl ARGS -base -poa)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/registry_service.idl ARGS -base -poa)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/fault_tolerance.idl ARGS -base -poa)

  include_directories(orbix ${CMAKE_CURRENT_BINARY_DIR} ${OPENBUS_INCLUDE_DIRECTORIES})

  add_executable (delegate_client singlethread/orbix/client.cpp ${CMAKE_CURRENT_BINARY_DIR}/delegate.hh
    ${CMAKE_CURRENT_BINARY_DIR}/delegateS.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/delegateC.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/core.hh
    ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.hh
    ${CMAKE_CURRENT_BINARY_DIR}/scs.hh
    ${CMAKE_CURRENT_BINARY_DIR}/registry_service.hh
    ${CMAKE_CURRENT_BINARY_DIR}/fault_tolerance.hh)
  add_executable (delegate_server singlethread/orbix/server.cpp ${CMAKE_CURRENT_BINARY_DIR}/delegate.hh
    ${CMAKE_CURRENT_BINARY_DIR}/delegateS.cxx ${CMAKE_CURRENT_BINARY_DIR}/delegateC.cxx)

  target_link_libraries(delegate_client ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
  target_link_libraries(delegate_server ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
endif (MICO_FOUND)

