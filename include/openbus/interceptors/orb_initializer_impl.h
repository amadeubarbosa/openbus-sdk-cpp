/*
** interceptors/ORBInitializerImpl.h
*/

#ifndef OPENBUS_SDK_ORB_INTERCEPTORS_INITIALIZER_IMPL_H
#define OPENBUS_SDK_ORB_INTERCEPTORS_INITIALIZER_IMPL_H

#ifdef OPENBUS_ORBIX
  #include <omg/PortableInterceptor.hh>
#endif

#include <openbus/interceptors/client_interceptor.h>
#include <openbus/interceptors/server_interceptor.h>

namespace openbus {

struct orb_state;

namespace interceptors {

/**
 * Define as políticas para a validação de credenciais interceptadas.
 */ 
enum CredentialValidationPolicy
{
/**
 * Indica que as credenciais interceptadas serão sempre validadas.
 */
ALWAYS
/**
 * Indica que as credenciais interceptadas serão validadas e 
 * armazenadas em um cache.
 */
, CACHED
/**
 * Indica que as credenciais interceptadas não serão validadas.
 */
,  NONE
};

class orb_initializer_impl : public PortableInterceptor::ORBInitializer
#ifdef OPENBUS_ORBIX
                           , public IT_CORBA::RefCountedLocalObject 
#endif
{
  static bool singleInstance;
  IOP::Codec_var codec;
  openbus::orb_state* orb_state;
  server_interceptor* serverInterceptor;
  client_interceptor* clientInterceptor;
  PortableInterceptor::SlotId slotid;
public:
  PortableInterceptor::ORBInitInfo* _info;
  orb_initializer_impl(openbus::orb_state& orb_state);
  ~orb_initializer_impl();

  void pre_init(PortableInterceptor::ORBInitInfo_ptr info);
  void post_init(PortableInterceptor::ORBInitInfo_ptr info);

  server_interceptor* getServerInterceptor();
};

} }

#endif
