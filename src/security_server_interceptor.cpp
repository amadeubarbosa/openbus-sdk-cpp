
#include <tec/openbus/detail/security_server_interceptor.hpp>
#include <tec/openbus/service_context_constants.hpp>
#include <tec/openbus/util/sequence_iterator.hpp>

#include <openssl/rand.h>

namespace tec { namespace openbus { namespace detail {

security_server_interceptor::security_server_interceptor(openbus_security_manager_impl* security_manager)
  : security_manager(security_manager)
{
}

char* security_server_interceptor::name()
{
  return CORBA::string_dup("tec::sdk::detail::security_server_interceptor::openbus");
}

void security_server_interceptor::destroy()
{
}

void security_server_interceptor::receive_request_service_contexts(PortableInterceptor::ServerRequestInfo_ptr info)
{
  std::cout << "receive_request_service_contexts" << std::endl;
}

void security_server_interceptor::receive_request(PortableInterceptor::ServerRequestInfo_ptr info)
{
  std::cout << "receive_request" << std::endl;

  std::cout << "Operation: " << info->operation() << std::endl;
  if(!security_manager->key.empty())
  {
    std::cout << "Is logged in, let's see if the request passes security" << std::endl;
    ::CORBA::OctetSeq_var object_id = info->object_id();
    ::CORBA::OctetSeq_var adapter_id = info->adapter_id();
    std::cout << "object_id: ";
    std::cout << std::hex;
    for(int i = 0; i < object_id->length(); ++i)
    {
      unsigned char v = (*object_id)[i];
      unsigned int i = v;
      std::cout << i;
    }
    std::cout << std::dec << std::endl;
    std::cout << "adapter_id: ";
    std::cout << std::hex;
    for(int i = 0; i < adapter_id->length(); ++i)
    {
      unsigned char v = (*adapter_id)[i];
      unsigned int i = v;
      std::cout << i;
    }
    std::cout << std::dec << std::endl;

    try
    {
      ::IOP::ServiceContext_var security_session_id_context
        = info->get_request_service_context(openbus::security_session_id);
    
    // std::cout << "request_id " << info->request_id() << std::endl;

    // CORBA::Object_var current_obj = security_manager->orb->resolve_initial_references("POACurrent");
    // std::cout << __FILE__ ":" << __LINE__ << std::endl;
    // assert(!CORBA::is_nil(current_obj));
    // std::cout << __FILE__ ":" << __LINE__ << std::endl;
    // PortableServer::Current_var current = PortableServer::Current::_narrow(current_obj);
    // std::cout << __FILE__ ":" << __LINE__ << std::endl;
    // assert(!CORBA::is_nil(current));
    // std::cout << __FILE__ ":" << __LINE__ << std::endl;
    // PortableServer::ObjectId_var new_object_id = current->get_object_id();
    // std::cout << __FILE__ ":" << __LINE__ << std::endl;
    // assert(!!new_object_id);
    // std::cout << __FILE__ ":" << __LINE__ << std::endl;
    // std::cout << "object_id: " << PortableServer::ObjectId_to_string(*new_object_id) << std::endl;
    // CORBA::Object_var object = current->get_reference();
    // CORBA::String_var ior = security_manager->orb->object_to_string(object.in());
    // std::cout << "ior: " << ior.in() << std::endl;
    }
    catch(CORBA::BAD_PARAM const&)
    {
      tecgraf::openbus::core::no_permission no_permission;

      std::vector<unsigned char> id(16);
      // Must create random id
      RAND_bytes(&id[0], id.size());

      no_permission.security_session_id.length(16);
      util::sequence_iterator<tecgraf::openbus::core::OctetSeq>
        first(no_permission.security_session_id);
      std::copy(id.begin(), id.end(), first);

      throw no_permission;
    }
  }
  else
  {
    std::cout << "Not logged in, so not intercepting" << std::endl;
  }
}

void security_server_interceptor::send_reply(PortableInterceptor::ServerRequestInfo_ptr info)
{
  std::cout << "send_reply" << std::endl;
}

void security_server_interceptor::send_exception(PortableInterceptor::ServerRequestInfo_ptr info)
{
  std::cout << "send_exception" << std::endl;
}

void security_server_interceptor::send_other(PortableInterceptor::ServerRequestInfo_ptr)
{
  std::cout << "send_other" << std::endl;
}

} } }
