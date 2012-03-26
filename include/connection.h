#ifndef TECGRAF_CONNECTION_H_
#define TECGRAF_CONNECTION_H_

#include <memory>
#include <stdexcept>
#include <string>
#include <CORBA.h>
#include <openssl/evp.h>

#include "openbus.h"
#include "interceptors/orbInitializer_impl.h"
#include "stubs/scs.h"
#include "stubs/core.h"
#include "stubs/credential.h"
#include "stubs/access_control.h"
#include "stubs/offer_registry.h"

namespace openbus {
  class ORB;
  class LoginCache;
  class RenewLogin;
  struct CallerChain;  
  namespace interceptors {
    class ClientInterceptor;
    class ServerInterceptor;
    class ORBInitializer;
  }
}

namespace openbus {  
  namespace idl = tecgraf::openbus::core::v2_00;
  namespace idl_ac = tecgraf::openbus::core::v2_00::services::access_control;
  namespace idl_accesscontrol = idl_ac;
  namespace idl_or = tecgraf::openbus::core::v2_00::services::offer_registry;
  namespace idl_offerregistry = idl_or;
  namespace idl_cr = tecgraf::openbus::core::v2_00::credential;
  namespace idl_credential = idl_cr;
  
  class Connection {
  public:
    typedef bool (*InvalidLoginCallback_ptr) (const Connection*, const char* login);
    
    /** Exceptions */
    //[todo] revisar exceptions
    struct Exception {
      virtual const char* name() const { return "Exception"; }
    };
      
    struct AlreadyLogged : public Exception {
      const char* name() const { return "AlreadyLogged"; }
    };

    struct CorruptedPrivateKey : public Exception {
      const char* name() const { return "CorruptedPrivateKey"; }
    };

    struct CorruptedBusCertificate : public Exception {
      const char* name() const { return "CorruptedBusCertificate"; }
    };

    struct WrongPrivateKey : public Exception {
      const char* name() const { return "WrongPrivateKey"; }
    };
    
    Connection(
      const std::string host,
      const unsigned int port,
      ORB* orb,
      const interceptors::ORBInitializer* orbInitializer) throw(CORBA::Exception);
    ~Connection();
    
    void loginByPassword(const char* entity, const char* password)
      throw (
        AlreadyLogged, 
        idl_ac::AccessDenied, 
        idl_ac::WrongEncoding,
        idl::services::ServiceFailure,
        CORBA::Exception);

    void loginByCertificate(const char* entity, EVP_PKEY* privateKey)
      throw (
        CorruptedPrivateKey, 
        CorruptedBusCertificate,
        WrongPrivateKey,
        AlreadyLogged, 
        idl_ac::MissingCertificate, 
        idl_ac::AccessDenied, 
        idl_ac::WrongEncoding,
        idl::services::ServiceFailure,
        CORBA::Exception);

    void loginByCertificate(const char* entity, const char* privateKeyFilename)
      throw (
        CorruptedPrivateKey, 
        CorruptedBusCertificate,
        WrongPrivateKey,
        AlreadyLogged, 
        idl_ac::MissingCertificate, 
        idl_ac::AccessDenied, 
        idl_ac::WrongEncoding,
        idl::services::ServiceFailure,
        CORBA::Exception);
    
    std::pair <idl_ac::LoginProcess*, unsigned char*> startSingleSignOn() 
      throw (idl::services::ServiceFailure);
      
    void loginBySingleSignOn(idl_ac::LoginProcess* loginProcess, unsigned char* secret)
      throw (idl::services::ServiceFailure);
      
    void onInvalidLoginCallback(InvalidLoginCallback_ptr p) { _onInvalidLogin = p; }
    InvalidLoginCallback_ptr onInvalidLoginCallback() { return _onInvalidLogin; }      
    bool logout();
    CallerChain* getCallerChain();
    void joinChain(CallerChain* chain);
    void exitChain();
    CallerChain* getJoineChain();
    void close();

    CORBA::ORB* orb() const;
    const idl_or::OfferRegistry_var offers() const { return _offer_registry; }
    const char* busid() const { return _busid; }
    const idl_ac::LoginInfo* login() const { return _loginInfo.get(); }

    //[todo] esconder (tests/01.cpp)
    const idl_ac::LoginRegistry_var login_registry() const { return _login_registry; }
  private:
    const idl_ac::AccessControl_var access_control() const { return _access_control; }
    EVP_PKEY* prvKey() const { return _prvKey; }
    EVP_PKEY* busKey() const { return _busKey; }

    std::string _host;
    unsigned int _port;
    ORB* _orb;
    const interceptors::ORBInitializer* _orbInitializer;
    interceptors::ClientInterceptor* _clientInterceptor;
    interceptors::ServerInterceptor* _serverInterceptor;
    std::auto_ptr<RenewLogin> _renewLogin;
    std::auto_ptr<idl_ac::LoginInfo> _loginInfo;
    scs::core::IComponent_var _iComponent;
    idl_ac::AccessControl_var _access_control;
    idl_ac::LoginRegistry_var _login_registry;
    idl_or::OfferRegistry_var _offer_registry;
    idl::Identifier_var _busid;
    EVP_PKEY* _busKey;
    idl::OctetSeq_var buskeyOctetSeq;
    EVP_PKEY* _prvKey;
    InvalidLoginCallback_ptr _onInvalidLogin;
    std::auto_ptr<LoginCache> _loginCache;
    friend class openbus::interceptors::ServerInterceptor;
    friend class openbus::interceptors::ClientInterceptor;
  };
  
  struct CallerChain {
    char* busid;
    idl_ac::LoginInfoSeq callers;
  private:
    idl_ac::SignedCallChain _signedCallChain;
    const idl_ac::SignedCallChain* signedCallChain() const { return &_signedCallChain; }
    void signedCallChain(idl_ac::SignedCallChain p) { _signedCallChain = p; }
    friend class Connection;
  };
}

#endif
