// -*- coding: iso-8859-1-unix -*-

#define OPENBUS_ASSISTANT_ALL_EXCEPTIONS()                              \
  (CORBA::NO_PERMISSION)(CORBA::TRANSIENT)(CORBA::COMM_FAILURE)(CORBA::OBJECT_NOT_EXIST) \
  (tecgraf::openbus::core::v2_0::services::ServiceFailure)(tecgraf::openbus::core::v2_0::services::UnauthorizedOperation) \
  (openbus::InvalidLoginProcess)                                        \
  (openbus::InvalidPrivateKey)                                          \
  (openbus::BusChanged)                                                 \
  (tecgraf::openbus::core::v2_0::services::access_control::InvalidCertificate) \
  (tecgraf::openbus::core::v2_0::services::access_control::MissingCertificate) \
  (tecgraf::openbus::core::v2_0::services::access_control::InvalidPublicKey) \
  (tecgraf::openbus::core::v2_0::services::access_control::AccessDenied) \
  (tecgraf::openbus::core::v2_0::services::offer_registry::UnauthorizedFacets) \
  (tecgraf::openbus::core::v2_0::services::offer_registry::InvalidService) \
  (tecgraf::openbus::core::v2_0::services::offer_registry::InvalidProperties) \
  (CORBA::SystemException)(CORBA::UserException)(std::runtime_error)(std::bad_alloc)(std::exception)

#define OPENBUS_ASSISTANT_LOGIN_EXCEPTIONS()                            \
  (CORBA::NO_PERMISSION)(CORBA::TRANSIENT)(CORBA::COMM_FAILURE)(CORBA::OBJECT_NOT_EXIST) \
  (tecgraf::openbus::core::v2_0::services::ServiceFailure)              \
  (openbus::InvalidLoginProcess)                                        \
  (openbus::InvalidPrivateKey)                                          \
  (openbus::BusChanged)                                                 \
  (tecgraf::openbus::core::v2_0::services::access_control::InvalidCertificate) \
  (tecgraf::openbus::core::v2_0::services::access_control::MissingCertificate) \
  (tecgraf::openbus::core::v2_0::services::access_control::InvalidPublicKey) \
  (tecgraf::openbus::core::v2_0::services::access_control::AccessDenied) \
  (CORBA::SystemException)(CORBA::UserException)(std::runtime_error)(std::bad_alloc)(std::exception)

#define OPENBUS_ASSISTANT_REGISTER_EXCEPTIONS()                         \
  (CORBA::NO_PERMISSION)(CORBA::TRANSIENT)(CORBA::COMM_FAILURE)(CORBA::OBJECT_NOT_EXIST) \
  (tecgraf::openbus::core::v2_0::services::ServiceFailure)(tecgraf::openbus::core::v2_0::services::UnauthorizedOperation) \
  (tecgraf::openbus::core::v2_0::services::access_control::AccessDenied) \
  (tecgraf::openbus::core::v2_0::services::offer_registry::UnauthorizedFacets) \
  (tecgraf::openbus::core::v2_0::services::offer_registry::InvalidService) \
  (tecgraf::openbus::core::v2_0::services::offer_registry::InvalidProperties) \
  (CORBA::SystemException)(CORBA::UserException)(std::runtime_error)(std::bad_alloc)(std::exception)

#define OPENBUS_ASSISTANT_FATAL_EXCEPTIONS()                              \
  (std::bad_alloc)(std::exception)

#define OPENBUS_ASSISTANT_FIND_EXCEPTIONS()                              \
  (CORBA::NO_PERMISSION)(CORBA::TRANSIENT)(CORBA::COMM_FAILURE)(CORBA::OBJECT_NOT_EXIST) \
  (tecgraf::openbus::core::v2_0::services::ServiceFailure)(tecgraf::openbus::core::v2_0::services::UnauthorizedOperation) \
  (tecgraf::openbus::core::v2_0::services::access_control::AccessDenied) \
  (CORBA::SystemException)(CORBA::UserException)(std::runtime_error)(std::bad_alloc)(std::exception)
