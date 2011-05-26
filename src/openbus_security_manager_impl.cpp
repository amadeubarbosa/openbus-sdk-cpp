
#include <tec/openbus/openbus_security_manager_impl.hpp>
#include <tec/openbus/security.hpp>

#include <boost/tuple/tuple.hpp>

namespace tec { namespace openbus {

void openbus_security_manager_impl::loginByPasswordBinary
  (tecgraf::openbus::core::OctetSeq const& username
   , tecgraf::openbus::core::OctetSeq const& password
   , tecgraf::openbus::core::v1_06::access_control_service::IAccessControlService_ptr acs)
{
  
}

void openbus_security_manager_impl::loginByPassword
  (const char* username
   , const char* password
   , tecgraf::openbus::core::v1_06::access_control_service::IAccessControlService_ptr acs)
{
  std::cout << "logging in" << std::endl;
  std::vector<unsigned char> signature;
  boost::tie(key, signature) = tec::openbus::login_by_password(acs, "Tester", "Tester");
}

} }
