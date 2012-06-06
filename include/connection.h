#ifndef TECGRAF_CONNECTION_H_
#define TECGRAF_CONNECTION_H_

#include <CORBA.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <openssl/evp.h>

#include "stubs/scs.h"
#include "stubs/core.h"
#include "stubs/credential.h"
#include "stubs/access_control.h"
#include "stubs/offer_registry.h"

namespace openbus {  
  namespace idl = tecgraf::openbus::core::v2_00;
  namespace idl_ac = tecgraf::openbus::core::v2_00::services::access_control;
  namespace idl_accesscontrol = idl_ac;
  namespace idl_or = tecgraf::openbus::core::v2_00::services::offer_registry;
  namespace idl_offerregistry = idl_or;
  namespace idl_cr = tecgraf::openbus::core::v2_00::credential;
  namespace idl_credential = idl_cr;
}

#include "interceptors/orbInitializer_impl.h"
#ifdef OPENBUS_SDK_MULTITHREAD
#include "util/mutex.h"
#endif

/* forward declarations */
namespace openbus {
  class LoginCache;
  class RenewLogin;
  struct CallerChain;  
  namespace interceptors {
    class ClientInterceptor;
    class ServerInterceptor;
    class ORBInitializer;
  }
  class ConnectionManager;
}

namespace openbus {  
  class Connection {
  public:
    typedef bool (*InvalidLoginCallback_ptr) (const Connection*, const idl_ac::LoginInfo*);
    
    struct Exception {
      virtual const char* name() const { return "openbus::Connection::Exception"; }
    };
      
    struct AccessDenied : public Exception {
      const char* name() const { return "openbus::Connection::AccessDenied"; }
    };

    struct AlreadyLoggedIn : public Exception {
      const char* name() const { return "openbus::Connection::AlreadyLoggedIn"; }
    };

    struct CorruptedPrivateKey : public Exception {
      const char* name() const { return "openbus::Connection::CorruptedPrivateKey"; }
    };

    struct WrongPrivateKey : public Exception {
      const char* name() const { return "openbus::Connection::WrongPrivateKey"; }
    };

    struct WrongSecret : public Exception {
      const char* name() const { return "openbus::Connection::WrongSecret"; }
    };
    
    struct InvalidLoginProcess : public Exception {
      const char* name() const { return "openbus::Connection::InvalidLoginProcess"; }
    };

    void loginByPassword(const char* entity, const char* password)
      throw (AlreadyLoggedIn, AccessDenied, idl::services::ServiceFailure, CORBA::Exception);

    void loginByCertificate(const char* entity, EVP_PKEY* privateKey)
      throw (CorruptedPrivateKey, WrongPrivateKey, AlreadyLoggedIn, idl_ac::MissingCertificate, 
      idl::services::ServiceFailure, CORBA::Exception);

    void loginByCertificate(const char* entity, const char* privateKeyFilename)
      throw (CorruptedPrivateKey, WrongPrivateKey, AlreadyLoggedIn, idl_ac::MissingCertificate, 
      idl::services::ServiceFailure, CORBA::Exception);
    
    std::pair <idl_ac::LoginProcess*, unsigned char*> startSingleSignOn() 
      throw (idl::services::ServiceFailure, CORBA::Exception);
      
    void loginBySingleSignOn(idl_ac::LoginProcess* loginProcess, unsigned char* secret)
  		throw(WrongSecret, InvalidLoginProcess, AlreadyLoggedIn, idl::services::ServiceFailure, 
  		CORBA::Exception);
          
    bool logout() throw (CORBA::Exception);
    CallerChain* getCallerChain() throw (CORBA::Exception);
    void joinChain(CallerChain* chain) throw (CORBA::Exception);
    void exitChain() throw (CORBA::Exception);
    CallerChain* getJoinedChain() throw (CORBA::Exception);
    
    void onInvalidLogin(InvalidLoginCallback_ptr p) { _onInvalidLogin = p; }
    InvalidLoginCallback_ptr onInvalidLogin() const { return _onInvalidLogin; }      
    const idl_ac::LoginInfo* login() const { return _loginInfo.get(); }
    const char* busid() const { return _busid; }
    const idl_or::OfferRegistry_var offers() const { return _offer_registry; }
    ~Connection();
  private:
    Connection(const std::string host, const unsigned int port, CORBA::ORB*, 
      const interceptors::ORBInitializer*, ConnectionManager*) throw(CORBA::Exception);
      
    const idl_ac::LoginRegistry_var login_registry() const { return _login_registry; }
    const idl_ac::AccessControl_var access_control() const { return _access_control; }
    EVP_PKEY* key() const { return _key; }
    EVP_PKEY* busKey() const { return _busKey; }
    bool _logout(bool local);
    std::string _host;
    unsigned int _port;
    CORBA::ORB* orb() const { return _orb; }
    CORBA::ORB* _orb;
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
    idl::OctetSeq_var _buskeyOctetSeq;
    EVP_PKEY* _key;
    InvalidLoginCallback_ptr _onInvalidLogin;
    std::auto_ptr<LoginCache> _loginCache;
    ConnectionManager* _manager;
    #ifdef OPENBUS_SDK_MULTITHREAD
    MICOMT::Mutex _mutex;
    #endif
    friend class openbus::interceptors::ServerInterceptor;
    friend class openbus::interceptors::ClientInterceptor;
    friend class RenewLogin;
    friend class LoginCache;
    friend class ConnectionManager;
  };
  
  struct CallerChain {
    const char* busid() const { return _busid; }
    const idl_ac::LoginInfoSeq& callers() const { return _callers; }
    CallerChain() : _busid(0) { _callers.length(0); }
    const idl_cr::SignedCallChain* signedCallChain() const { return &_signedCallChain; }
  private:
    char* _busid;
    idl_ac::LoginInfoSeq _callers;
    idl_cr::SignedCallChain _signedCallChain;
    void signedCallChain(idl_cr::SignedCallChain p) { _signedCallChain = p; }
    friend class Connection;
    friend class openbus::interceptors::ClientInterceptor;
  };
}

#endif
