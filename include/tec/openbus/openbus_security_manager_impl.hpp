#ifndef TEC_OPENBUS_OPENBUS_SECURITY_MANAGER_HPP
#define TEC_OPENBUS_OPENBUS_SECURITY_MANAGER_HPP

#include "openbus_security_manager.h"

#include <tec/ssl/rsa.hpp>

namespace tec { namespace openbus {

struct openbus_security_manager_impl : virtual tecgraf::openbus::core::openbus_security_manager
                                     , virtual CORBA::LocalObject
{
  openbus_security_manager_impl() : orb(0) {}

  void loginByPasswordBinary(tecgraf::openbus::core::OctetSeq const& username
                              , tecgraf::openbus::core::OctetSeq const& password
                              , tecgraf::openbus::core::v1_06::access_control_service::IAccessControlService_ptr acs);
  void loginByPassword(const char* username
                       , const char* password
                       , tecgraf::openbus::core::v1_06::access_control_service::IAccessControlService_ptr acs);

  ssl::rsa key;
  tecgraf::openbus::core::v1_06::OctetSeq_var public_key_signature;
  CORBA::ORB_ptr orb;
};

} }

#endif

