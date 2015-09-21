// -*- coding: iso-8859-1-unix -*-
/**
* API do OpenBus SDK C++
* \file openbus/caller_chain.hpp
*/

#ifndef TECGRAF_SDK_OPENBUS_CALLER_CHAIN_HPP
#define TECGRAF_SDK_OPENBUS_CALLER_CHAIN_HPP

#include "openbus/idl.hpp"

#include <string>
#include <cstring>

namespace tecgraf
{ 
namespace openbus
{ 
namespace core 
{ 
namespace v2_1 
{ 
namespace services 
{ 
namespace access_control 
{

inline bool operator==(const LoginInfo &lhs, const LoginInfo &rhs)
{
  return lhs.id.in() == rhs.id.in() 
    || (lhs.id.in() && rhs.id.in() && !std::strcmp(lhs.id.in(), rhs.id.in()));
}

inline bool operator!=(const LoginInfo &lhs, const LoginInfo &rhs)
{
  return !(lhs == rhs);
}

inline bool operator==(const LoginInfoSeq &lhs, const LoginInfoSeq &rhs)
{
  if (lhs.length() != rhs.length())
  {
    return false;
  }
  for (CORBA::ULong i(0); i < rhs.length(); ++i)
  {
    if (lhs[i] != rhs[i])
    {
      return false;
    }
  }
  return true;
}

inline bool operator!=(const LoginInfoSeq &lhs, const LoginInfoSeq &rhs)
{
  return !(lhs == rhs);
}

}}}}}}

namespace openbus 
{
class OpenBusContext;

namespace interceptors
{
  struct ClientInterceptor;
}
  
/**
 * \brief Cadeia de chamadas oriundas de um barramento.
 * 
 * Coleção de informações dos logins que originaram chamadas em cadeia
 * através de um barramento. Cadeias de chamadas representam chamadas
 * aninhadas dentro do barramento e são úteis para que os sistemas que
 * recebam essas chamadas possam identificar se a chamada foi
 * originada por entidades autorizadas ou não.
 */
struct OPENBUS_SDK_DECL CallerChain 
{
  /**
  * \brief Barramento através do qual as chamadas foram originadas.
  */
  const std::string busid() const 
  {
    return _busid;
  }

	/**
   * \brief Entidade para a qual a chamada estava destinada. 
   *
   * Só é possível fazer 
   * chamadas dentro dessa cadeia através do método 
   * \ref OpenBusContext::joinChain se a entidade da conexão 
   * corrente for a mesmo do target.
   *
   */
  const std::string target() const
  {
    return _target;
  }
  
	/**
	 * \brief Lista de informações de login de todas as entidades que originaram as
	 * chamadas nessa cadeia. 
   *
   * Quando essa lista é vazia isso indica que a  chamada não está inclusa em 
   * outra cadeia de chamadas.
	 */
  const idl::access::LoginInfoSeq &originators() const 
  {
    return _originators;
  }
  
  /**
   * \brief Informação de login da entidade que realizou a última chamada da 
   * cadeia.
   */
  const idl::access::LoginInfo &caller() const 
  {
    return _caller;
  }

  /**
   * \brief Construtor default que indica há ausência de uma cadeia.
   *
   * O valor de um CallerChain default-constructed pode ser usado para
   * verificar a ausência de uma cadeia da seguinte forma:
   * \code
   * CallerChain chain(openbusContext.getCallerChain());
   * if(chain != CallerChain())
   *   // Possui CallerChain
   * else
   *   // Nao possui CallerChain
   * \endcode
   */
  CallerChain() 
  {
  }
  
//private:
  
#ifndef OPENBUS_SDK_TEST
private:
#else
public:
#endif
  CallerChain(
    const idl::access::CallChain &chain,
    const std::string &busid,
    const std::string &target,
    const idl::creden::SignedData &signed_chain)
    : _busid(chain.bus.in())
    , _target(target)
    , _originators(chain.originators)
    , _caller(chain.caller)
    , _signed_chain(signed_chain)
  {
  }

  CallerChain(
    const idl::legacy::access::CallChain &chain,
    const std::string &busid,
    const std::string &target,
    const idl::legacy::creden::SignedCallChain &signed_chain)
    : _busid(busid)
    , _target(target)
    , _legacy_signed_chain(signed_chain)
  {
    _originators = idl::access::LoginInfoSeq(
      chain.originators.length(),
      chain.originators.length(),
      (idl::access::LoginInfo*)chain.originators.get_buffer());
    _caller.id = chain.caller.id;
    _caller.entity = chain.caller.entity;
  }

  CallerChain(const std::string &busid, 
              const std::string &target,
              const idl::access::LoginInfoSeq &originators, 
              const idl::access::LoginInfo &caller) 
    : _busid(busid)
    , _target(target)
    , _originators(originators)
    , _caller(caller) 
  {
  }

  idl::creden::SignedData
    signed_chain(idl::creden::CredentialData) const
  {
    return _signed_chain;
  }

  idl::legacy::creden::SignedCallChain
    signed_chain(idl::legacy::creden::CredentialData) const
  {
    return _legacy_signed_chain;
  }

  bool is_legacy() const
  {
    return _legacy_signed_chain.encoded.length() > 0 ? true : false;
  }

  std::string _busid;
  std::string _target;
  idl::access::LoginInfoSeq _originators;
  idl::access::LoginInfo _caller;
  idl::creden::SignedData _signed_chain;
  idl::legacy::creden::SignedCallChain _legacy_signed_chain;
  
  friend class OpenBusContext;
  friend struct interceptors::ClientInterceptor;
  friend inline bool operator==(CallerChain const &lhs, 
                                CallerChain const &rhs) 
  {
    return lhs._busid == rhs._busid
      && lhs._originators == rhs._originators
      && lhs._caller == rhs._caller;
  }
};

inline bool operator!=(CallerChain const &lhs,
                       CallerChain const &rhs) 
{
  return !(lhs == rhs);
}

}

#endif
