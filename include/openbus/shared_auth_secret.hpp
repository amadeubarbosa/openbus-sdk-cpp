// -*- coding: iso-8859-1-unix -*-

/**
* API do OpenBus SDK C++
* \file openbus/shared_auth_secret.hpp
* 
*/

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_SHARED_AUTH_SECRET_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_SHARED_AUTH_SECRET_HPP

#include "openbus/idl.hpp"
#include "openbus/detail/decl.hpp"

#include <string>

namespace openbus
{
  class Connection;
  class OpenBusContext;
  namespace interceptors
  {
    struct ORBInitializer;
  }
  
/**
 * \brief Segredo para compartilhamento de autenticação.
 *
 * Objeto que representa uma tentativa de compartilhamento de
 * autenticação através do compartilhamento de um segredo, que pode
 * ser utilizado para realizar uma autenticação junto ao barramento em
 * nome da mesma entidade que gerou e compartilhou o segredo.
 *
 * Cada segredo de autenticação compartilhada pertence a um único barramento e
 * só pode ser utilizado em uma única autenticação.
 *
 */
class OPENBUS_SDK_DECL SharedAuthSecret
{
public:
   /**
    * \brief Cancela o segredo tornando-o inutilizável.
    *
    * Cancela o segredo caso esse ainda esteja ativo, de forma que ele
    * não poderá ser mais utilizado.
    *
    */
   void cancel();
  
  /**
   * \brief Retorna o identificador do barramento em que o segredo
   *        pode ser utilizado.
   */
  std::string busid() const
  {
    return busid_;
  }
private:
  SharedAuthSecret();
  SharedAuthSecret(
    const std::string &bus_id,
    idl::access::LoginProcess_var,
    idl::legacy::access::LoginProcess_var,
    const idl::core::OctetSeq &secret,
    interceptors::ORBInitializer *);  

  std::string busid_;
  idl::access::LoginProcess_var login_process_;
  idl::legacy::access::LoginProcess_var legacy_login_process_;
  idl::core::OctetSeq secret_;
  interceptors::ORBInitializer *orb_initializer_;
  friend class OpenBusContext;
  friend class Connection;
};

}

#endif
