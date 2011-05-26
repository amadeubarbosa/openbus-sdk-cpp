
#include <tec/openbus/security.hpp>
#include <tec/openbus/detail/orb_initializer.hpp>

#include <CORBA.h>

namespace tec { namespace openbus {

void initialize()
{
  detail::orb_initializer* initializer = new detail::orb_initializer;
  PortableInterceptor::register_orb_initializer(initializer);
}

void initialize_security_by_login(CORBA::ORB_ptr orb, const char* username, const char* password
                                  , tecgraf::openbus::core::v1_06::access_control_service::IAccessControlService_ptr acs)
{
  CORBA::Object_var security_manager_obj = orb->resolve_initial_references("OpenbusSecurityManager");
  assert(!CORBA::is_nil(security_manager_obj));
  tecgraf::openbus::core::openbus_security_manager_var security_manager
    = tecgraf::openbus::core::openbus_security_manager::_narrow(security_manager_obj);
  assert(!CORBA::is_nil(security_manager));

  tec::openbus::openbus_security_manager_impl& manager
    = dynamic_cast<tec::openbus::openbus_security_manager_impl&>(*security_manager.operator->());

  security_manager->loginByPassword(username, password, acs);
  
  manager.orb = orb;
}

} }
