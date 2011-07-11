
find_package (CxxTest)

include_directories(${SCS_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})

if (MICO_FOUND)
  mico_idl (${CMAKE_CURRENT_SOURCE_DIR}/idl/RGSTest.idl)

  mico_idl(${SCS_IDL_IDL_DIR}/scs.idl)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/core.idl)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/access_control_service.idl ARGS --any --typecode)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/registry_service.idl)
  mico_idl(${OPENBUS_CORE_IDL_IDL_DIR}/fault_tolerance.idl)

  include_directories(mico ${CMAKE_CURRENT_BINARY_DIR} ${OPENBUS_INCLUDE_DIRECTORIES} ${CXXTEST_INCLUDE_DIRS})

  cxxtest_add_test (acs_test ACSTestSuite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mico/acs/ACSTestSuite.h
    ${CMAKE_CURRENT_BINARY_DIR}/core.h
    ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/scs.h
    ${CMAKE_CURRENT_BINARY_DIR}/registry_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/fault_tolerance.h)

  cxxtest_add_test (rgs_test RGSTestSuite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mico/rgs/RGSTestSuite.h
    ${CMAKE_CURRENT_BINARY_DIR}/RGSTest.h
    ${CMAKE_CURRENT_BINARY_DIR}/RGSTest.cc
    ${CMAKE_CURRENT_BINARY_DIR}/core.h
    ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/scs.h
    ${CMAKE_CURRENT_BINARY_DIR}/registry_service.h
    ${CMAKE_CURRENT_BINARY_DIR}/fault_tolerance.h)

  target_link_libraries(acs_test ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_sdk_static openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})
  target_link_libraries(rgs_test ${MICO_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} openbus_sdk_static openbus_logger_static ${OPENSSL_LIBRARIES} ${LUA_LIBRARIES} ${SCS_LIBRARIES})

else (MICO_FOUND)

  orbix_idl (${CMAKE_CURRENT_SOURCE_DIR}/idl/RGSTest.idl ARGS -base -poa)

   orbix_idl(${SCS_IDL_IDL_DIR}/scs.idl ARGS -base -poa)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/core.idl ARGS -base -poa)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/access_control_service.idl ARGS -base -poa)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/registry_service.idl ARGS -base -poa)
   orbix_idl(${OPENBUS_CORE_IDL_IDL_DIR}/fault_tolerance.idl ARGS -base -poa)

  include_directories(orbix ${CMAKE_CURRENT_BINARY_DIR} ${OPENBUS_INCLUDE_DIRECTORIES})

  cxxtest_add_test (acs_test ACSTestSuite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/orbix/acs/ACSTestSuite.h
    ${CMAKE_CURRENT_BINARY_DIR}/core.hh
    ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.hh
    ${CMAKE_CURRENT_BINARY_DIR}/scs.hh
    ${CMAKE_CURRENT_BINARY_DIR}/registry_service.hh
    ${CMAKE_CURRENT_BINARY_DIR}/fault_tolerance.hh)
  cxxtest_add_test (rgs_test RGSTestSuite.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/orbix/rgs/RGSTestSuite.h
    ${CMAKE_CURRENT_BINARY_DIR}/RGSTest.hh
    ${CMAKE_CURRENT_BINARY_DIR}/RGSTestC.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/RGSTestS.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/core.hh
    ${CMAKE_CURRENT_BINARY_DIR}/access_control_service.hh
    ${CMAKE_CURRENT_BINARY_DIR}/scs.hh
    ${CMAKE_CURRENT_BINARY_DIR}/registry_service.hh
    ${CMAKE_CURRENT_BINARY_DIR}/fault_tolerance.hh)

endif (MICO_FOUND)
