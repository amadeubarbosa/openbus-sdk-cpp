// -*- coding: iso-8859-1-unix -*-

#include "openbus/shared_auth_secret.hpp"
#include "openbus/detail/interceptors/orb_initializer.hpp"

namespace openbus
{

SharedAuthSecret::SharedAuthSecret()
  : login_process_(idl::access::LoginProcess::_nil())
  , legacy_login_process_(idl::legacy::access::LoginProcess::_nil())
{
}

SharedAuthSecret::SharedAuthSecret(
  const std::string &busid,
  idl::access::LoginProcess_var login_process,
  idl::legacy::access::LoginProcess_var legacy_login_process,
  const idl::core::OctetSeq &secret,
  interceptors::ORBInitializer *init)
  : busid_(busid)
  , login_process_(login_process)
  , legacy_login_process_(legacy_login_process)
  , secret_(secret)
  , orb_initializer_(init)
{
}

void SharedAuthSecret::cancel()
{
  interceptors::ignore_interceptor i(orb_initializer_);
  login_process_->cancel();
}

}
