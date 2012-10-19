// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_EXCEPTION_MESSAGE_H
#define OPENBUS_ASSISTANT_DETAIL_EXCEPTION_MESSAGE_H

namespace openbus { namespace assistant { namespace assistant_detail {

inline const char* exception_message(CORBA::NO_PERMISSION const&)
{
  return "CORBA::NO_PERMISSION";
}

inline const char* exception_message(CORBA::TRANSIENT const&)
{
  return "CORBA::TRANSIENT";
}

inline const char* exception_message(CORBA::COMM_FAILURE const&)
{
  return "CORBA::COMM_FAILURE";
}

inline const char* exception_message(CORBA::OBJECT_NOT_EXIST const&)
{
  return "CORBA::OBJECT_NOT_EXIST";
}

inline const char* exception_message(idl::services::ServiceFailure const&)
{
  return "tecgraf::openbus::core::v2_0::services::ServiceFailure";
}

inline const char* exception_message(idl::services::UnauthorizedOperation const&)
{
  return "tecgraf::openbus::core::v2_0::services::UnauthorizedOperation";
}

inline const char* exception_message(idl::services::offer_registry::UnauthorizedFacets const&)
{
  return "tecgraf::openbus::core::v2_0::services::offer_registry::UnauthorizedFacets";
}

inline const char* exception_message(CORBA::SystemException const&)
{
  return "Unknown derived exception of CORBA::SystemException";
}

inline const char* exception_message(CORBA::UserException const&)
{
  return "Unknown derived exception of CORBA::UserException";
}

} } }

#endif
