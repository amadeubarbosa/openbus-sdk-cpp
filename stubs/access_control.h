/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2006 by The Mico Team
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <CORBA.h>
#include <mico/throw.h>

#ifndef __ACCESS_CONTROL_H__
#define __ACCESS_CONTROL_H__






namespace tecgraf
{
namespace openbus
{
namespace core
{
namespace v2_00
{
namespace services
{
namespace access_control
{

class CertificateRegistry;
typedef CertificateRegistry *CertificateRegistry_ptr;
typedef CertificateRegistry_ptr CertificateRegistryRef;
typedef ObjVar< CertificateRegistry > CertificateRegistry_var;
typedef ObjOut< CertificateRegistry > CertificateRegistry_out;

class LoginByCertificate;
typedef LoginByCertificate *LoginByCertificate_ptr;
typedef LoginByCertificate_ptr LoginByCertificateRef;
typedef ObjVar< LoginByCertificate > LoginByCertificate_var;
typedef ObjOut< LoginByCertificate > LoginByCertificate_out;

class AccessControl;
typedef AccessControl *AccessControl_ptr;
typedef AccessControl_ptr AccessControlRef;
typedef ObjVar< AccessControl > AccessControl_var;
typedef ObjOut< AccessControl > AccessControl_out;

class LoginObserver;
typedef LoginObserver *LoginObserver_ptr;
typedef LoginObserver_ptr LoginObserverRef;
typedef ObjVar< LoginObserver > LoginObserver_var;
typedef ObjOut< LoginObserver > LoginObserver_out;

class LoginObserverSubscription;
typedef LoginObserverSubscription *LoginObserverSubscription_ptr;
typedef LoginObserverSubscription_ptr LoginObserverSubscriptionRef;
typedef ObjVar< LoginObserverSubscription > LoginObserverSubscription_var;
typedef ObjOut< LoginObserverSubscription > LoginObserverSubscription_out;

class LoginRegistry;
typedef LoginRegistry *LoginRegistry_ptr;
typedef LoginRegistry_ptr LoginRegistryRef;
typedef ObjVar< LoginRegistry > LoginRegistry_var;
typedef ObjOut< LoginRegistry > LoginRegistry_out;

}
}
}
}
}
}



#include <core.h>



namespace tecgraf
{
namespace openbus
{
namespace core
{
namespace v2_00
{
namespace services
{
namespace access_control
{

const char* const AccessControlFacet = "AccessControl_2_00";
const char* const LoginRegistryFacet = "LoginRegistry_2_00";
const char* const CertificateRegistryFacet = "CertificateRegistry_2_00";
const CORBA::ULong NoLoginCode = 1112888064UL;
const CORBA::ULong InvalidLoginCode = 1112888065UL;
const CORBA::ULong UnverifiedLoginCode = 1112888066UL;
const CORBA::ULong InvalidRemoteCode = 1112888067UL;
const CORBA::ULong InvalidResetCode = 1112888068UL;
const CORBA::ULong DeniedLoginCode = 1112888069UL;
const CORBA::ULong InvalidCredentialCode = 1112888070UL;
const CORBA::ULong InvalidChainCode = 1112888071UL;
const CORBA::ULong UnknownBusCode = 1112888072UL;
typedef SequenceTmpl< CORBA::Octet,MICO_TID_OCTET> EntityCertificate;
typedef TSeqVar< SequenceTmpl< CORBA::Octet,MICO_TID_OCTET> > EntityCertificate_var;
typedef TSeqOut< SequenceTmpl< CORBA::Octet,MICO_TID_OCTET> > EntityCertificate_out;

extern CORBA::TypeCodeConst _tc_EntityCertificate;

struct MissingCertificate : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  MissingCertificate();
  ~MissingCertificate();
  MissingCertificate( const MissingCertificate& s );
  MissingCertificate& operator=( const MissingCertificate& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  MissingCertificate();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  MissingCertificate( const char* _m0 );

  #ifdef HAVE_STD_EH
  MissingCertificate *operator->() { return this; }
  MissingCertificate& operator*() { return *this; }
  operator MissingCertificate*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static MissingCertificate *_downcast( CORBA::Exception *ex );
  static const MissingCertificate *_downcast( const CORBA::Exception *ex );
  Identifier_var entity;
};

#ifdef HAVE_STD_EH
typedef MissingCertificate MissingCertificate_catch;
#else
typedef ExceptVar< MissingCertificate > MissingCertificate_var;
typedef TVarOut< MissingCertificate > MissingCertificate_out;
typedef MissingCertificate_var MissingCertificate_catch;
#endif // HAVE_STD_EH

extern CORBA::TypeCodeConst _tc_MissingCertificate;

struct InvalidCertificate : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  InvalidCertificate();
  ~InvalidCertificate();
  InvalidCertificate( const InvalidCertificate& s );
  InvalidCertificate& operator=( const InvalidCertificate& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  InvalidCertificate();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  InvalidCertificate( const char* _m0 );

  #ifdef HAVE_STD_EH
  InvalidCertificate *operator->() { return this; }
  InvalidCertificate& operator*() { return *this; }
  operator InvalidCertificate*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static InvalidCertificate *_downcast( CORBA::Exception *ex );
  static const InvalidCertificate *_downcast( const CORBA::Exception *ex );
  CORBA::String_var message;
};

#ifdef HAVE_STD_EH
typedef InvalidCertificate InvalidCertificate_catch;
#else
typedef ExceptVar< InvalidCertificate > InvalidCertificate_var;
typedef TVarOut< InvalidCertificate > InvalidCertificate_out;
typedef InvalidCertificate_var InvalidCertificate_catch;
#endif // HAVE_STD_EH

extern CORBA::TypeCodeConst _tc_InvalidCertificate;


/*
 * Base class and common definitions for interface CertificateRegistry
 */

class CertificateRegistry : 
  virtual public CORBA::Object
{
  public:
    virtual ~CertificateRegistry();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef CertificateRegistry_ptr _ptr_type;
    typedef CertificateRegistry_var _var_type;
    #endif

    static CertificateRegistry_ptr _narrow( CORBA::Object_ptr obj );
    static CertificateRegistry_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static CertificateRegistry_ptr _duplicate( CertificateRegistry_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static CertificateRegistry_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual void registerCertificate( const char* entity, const ::tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate& cert ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate* getCertificate( const char* entity ) = 0;
    virtual CORBA::Boolean removeCertificate( const char* entity ) = 0;

  protected:
    CertificateRegistry() {};
  private:
    CertificateRegistry( const CertificateRegistry& );
    void operator=( const CertificateRegistry& );
};

extern CORBA::TypeCodeConst _tc_CertificateRegistry;

// Stub for interface CertificateRegistry
class CertificateRegistry_stub:
  virtual public CertificateRegistry
{
  public:
    virtual ~CertificateRegistry_stub();
    void registerCertificate( const char* entity, const ::tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate& cert );
    ::tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate* getCertificate( const char* entity );
    CORBA::Boolean removeCertificate( const char* entity );

  private:
    void operator=( const CertificateRegistry_stub& );
};

#ifndef MICO_CONF_NO_POA

class CertificateRegistry_stub_clp :
  virtual public CertificateRegistry_stub,
  virtual public PortableServer::StubBase
{
  public:
    CertificateRegistry_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~CertificateRegistry_stub_clp ();
    void registerCertificate( const char* entity, const ::tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate& cert );
    ::tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate* getCertificate( const char* entity );
    CORBA::Boolean removeCertificate( const char* entity );

  protected:
    CertificateRegistry_stub_clp ();
  private:
    void operator=( const CertificateRegistry_stub_clp & );
};

#endif // MICO_CONF_NO_POA

struct LoginAuthenticationInfo;
typedef TVarVar< LoginAuthenticationInfo > LoginAuthenticationInfo_var;
typedef TVarOut< LoginAuthenticationInfo > LoginAuthenticationInfo_out;


struct LoginAuthenticationInfo {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef LoginAuthenticationInfo_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  LoginAuthenticationInfo();
  ~LoginAuthenticationInfo();
  LoginAuthenticationInfo( const LoginAuthenticationInfo& s );
  LoginAuthenticationInfo& operator=( const LoginAuthenticationInfo& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  HashValue hash;
  OctetSeq data;
};

extern CORBA::TypeCodeConst _tc_LoginAuthenticationInfo;

struct LoginInfo;
typedef TVarVar< LoginInfo > LoginInfo_var;
typedef TVarOut< LoginInfo > LoginInfo_out;


struct LoginInfo {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef LoginInfo_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  LoginInfo();
  ~LoginInfo();
  LoginInfo( const LoginInfo& s );
  LoginInfo& operator=( const LoginInfo& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  Identifier_var id;
  Identifier_var entity;
};

extern CORBA::TypeCodeConst _tc_LoginInfo;

typedef SequenceTmpl< LoginInfo,MICO_TID_DEF> LoginInfoSeq;
typedef TSeqVar< SequenceTmpl< LoginInfo,MICO_TID_DEF> > LoginInfoSeq_var;
typedef TSeqOut< SequenceTmpl< LoginInfo,MICO_TID_DEF> > LoginInfoSeq_out;

extern CORBA::TypeCodeConst _tc_LoginInfoSeq;

struct CallChain;
typedef TVarVar< CallChain > CallChain_var;
typedef TVarOut< CallChain > CallChain_out;


struct CallChain {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef CallChain_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  CallChain();
  ~CallChain();
  CallChain( const CallChain& s );
  CallChain& operator=( const CallChain& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  Identifier_var target;
  LoginInfoSeq callers;
};

extern CORBA::TypeCodeConst _tc_CallChain;

struct SignedCallChain;
typedef TVarVar< SignedCallChain > SignedCallChain_var;
typedef TVarOut< SignedCallChain > SignedCallChain_out;


struct SignedCallChain {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef SignedCallChain_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  SignedCallChain();
  ~SignedCallChain();
  SignedCallChain( const SignedCallChain& s );
  SignedCallChain& operator=( const SignedCallChain& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  EncryptedBlock signature;
  OctetSeq encoded;
};

extern CORBA::TypeCodeConst _tc_SignedCallChain;

typedef CORBA::ULong ValidityTime;
typedef ValidityTime& ValidityTime_out;
extern CORBA::TypeCodeConst _tc_ValidityTime;

typedef SequenceTmpl< ValidityTime,MICO_TID_DEF> ValidityTimeSeq;
typedef TSeqVar< SequenceTmpl< ValidityTime,MICO_TID_DEF> > ValidityTimeSeq_var;
typedef TSeqOut< SequenceTmpl< ValidityTime,MICO_TID_DEF> > ValidityTimeSeq_out;

extern CORBA::TypeCodeConst _tc_ValidityTimeSeq;

struct WrongEncoding : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  WrongEncoding();
  ~WrongEncoding();
  WrongEncoding( const WrongEncoding& s );
  WrongEncoding& operator=( const WrongEncoding& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS


  #ifdef HAVE_STD_EH
  WrongEncoding *operator->() { return this; }
  WrongEncoding& operator*() { return *this; }
  operator WrongEncoding*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static WrongEncoding *_downcast( CORBA::Exception *ex );
  static const WrongEncoding *_downcast( const CORBA::Exception *ex );
};

#ifdef HAVE_STD_EH
typedef WrongEncoding WrongEncoding_catch;
#else
typedef ExceptVar< WrongEncoding > WrongEncoding_var;
typedef TVarOut< WrongEncoding > WrongEncoding_out;
typedef WrongEncoding_var WrongEncoding_catch;
#endif // HAVE_STD_EH

extern CORBA::TypeCodeConst _tc_WrongEncoding;

struct AccessDenied : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  AccessDenied();
  ~AccessDenied();
  AccessDenied( const AccessDenied& s );
  AccessDenied& operator=( const AccessDenied& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS


  #ifdef HAVE_STD_EH
  AccessDenied *operator->() { return this; }
  AccessDenied& operator*() { return *this; }
  operator AccessDenied*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static AccessDenied *_downcast( CORBA::Exception *ex );
  static const AccessDenied *_downcast( const CORBA::Exception *ex );
};

#ifdef HAVE_STD_EH
typedef AccessDenied AccessDenied_catch;
#else
typedef ExceptVar< AccessDenied > AccessDenied_var;
typedef TVarOut< AccessDenied > AccessDenied_out;
typedef AccessDenied_var AccessDenied_catch;
#endif // HAVE_STD_EH

extern CORBA::TypeCodeConst _tc_AccessDenied;


/*
 * Base class and common definitions for interface LoginByCertificate
 */

class LoginByCertificate : 
  virtual public CORBA::Object
{
  public:
    virtual ~LoginByCertificate();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef LoginByCertificate_ptr _ptr_type;
    typedef LoginByCertificate_var _var_type;
    #endif

    static LoginByCertificate_ptr _narrow( CORBA::Object_ptr obj );
    static LoginByCertificate_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static LoginByCertificate_ptr _duplicate( LoginByCertificate_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static LoginByCertificate_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual char* login( const ::tecgraf::openbus::core::v2_00::OctetSeq& pubkey, const ::tecgraf::openbus::core::v2_00::EncryptedBlock encrypted, ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out validity ) = 0;
    virtual void cancel() = 0;

  protected:
    LoginByCertificate() {};
  private:
    LoginByCertificate( const LoginByCertificate& );
    void operator=( const LoginByCertificate& );
};

extern CORBA::TypeCodeConst _tc_LoginByCertificate;

// Stub for interface LoginByCertificate
class LoginByCertificate_stub:
  virtual public LoginByCertificate
{
  public:
    virtual ~LoginByCertificate_stub();
    char* login( const ::tecgraf::openbus::core::v2_00::OctetSeq& pubkey, const ::tecgraf::openbus::core::v2_00::EncryptedBlock encrypted, ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out validity );
    void cancel();

  private:
    void operator=( const LoginByCertificate_stub& );
};

#ifndef MICO_CONF_NO_POA

class LoginByCertificate_stub_clp :
  virtual public LoginByCertificate_stub,
  virtual public PortableServer::StubBase
{
  public:
    LoginByCertificate_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~LoginByCertificate_stub_clp ();
    char* login( const ::tecgraf::openbus::core::v2_00::OctetSeq& pubkey, const ::tecgraf::openbus::core::v2_00::EncryptedBlock encrypted, ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out validity );
    void cancel();

  protected:
    LoginByCertificate_stub_clp ();
  private:
    void operator=( const LoginByCertificate_stub_clp & );
};

#endif // MICO_CONF_NO_POA


/*
 * Base class and common definitions for interface AccessControl
 */

class AccessControl : 
  virtual public CORBA::Object
{
  public:
    virtual ~AccessControl();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef AccessControl_ptr _ptr_type;
    typedef AccessControl_var _var_type;
    #endif

    static AccessControl_ptr _narrow( CORBA::Object_ptr obj );
    static AccessControl_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static AccessControl_ptr _duplicate( AccessControl_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static AccessControl_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual char* busid() = 0;
    virtual ::tecgraf::openbus::core::v2_00::OctetSeq* buskey() = 0;

    virtual char* loginByPassword( const char* entity, const ::tecgraf::openbus::core::v2_00::OctetSeq& pubkey, const ::tecgraf::openbus::core::v2_00::EncryptedBlock encrypted, ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out validity ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr startLoginByCertificate( const char* entity, ::tecgraf::openbus::core::v2_00::EncryptedBlock_out challenge ) = 0;
    virtual void logout() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime renew() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain* signChainFor( const char* target ) = 0;

  protected:
    AccessControl() {};
  private:
    AccessControl( const AccessControl& );
    void operator=( const AccessControl& );
};

extern CORBA::TypeCodeConst _tc_AccessControl;

// Stub for interface AccessControl
class AccessControl_stub:
  virtual public AccessControl
{
  public:
    virtual ~AccessControl_stub();
    char* busid();
    ::tecgraf::openbus::core::v2_00::OctetSeq* buskey();

    char* loginByPassword( const char* entity, const ::tecgraf::openbus::core::v2_00::OctetSeq& pubkey, const ::tecgraf::openbus::core::v2_00::EncryptedBlock encrypted, ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out validity );
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr startLoginByCertificate( const char* entity, ::tecgraf::openbus::core::v2_00::EncryptedBlock_out challenge );
    void logout();
    ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime renew();
    ::tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain* signChainFor( const char* target );

  private:
    void operator=( const AccessControl_stub& );
};

#ifndef MICO_CONF_NO_POA

class AccessControl_stub_clp :
  virtual public AccessControl_stub,
  virtual public PortableServer::StubBase
{
  public:
    AccessControl_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~AccessControl_stub_clp ();
    char* busid();
    ::tecgraf::openbus::core::v2_00::OctetSeq* buskey();

    char* loginByPassword( const char* entity, const ::tecgraf::openbus::core::v2_00::OctetSeq& pubkey, const ::tecgraf::openbus::core::v2_00::EncryptedBlock encrypted, ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out validity );
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr startLoginByCertificate( const char* entity, ::tecgraf::openbus::core::v2_00::EncryptedBlock_out challenge );
    void logout();
    ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime renew();
    ::tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain* signChainFor( const char* target );

  protected:
    AccessControl_stub_clp ();
  private:
    void operator=( const AccessControl_stub_clp & );
};

#endif // MICO_CONF_NO_POA

struct InvalidLogins : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  InvalidLogins();
  ~InvalidLogins();
  InvalidLogins( const InvalidLogins& s );
  InvalidLogins& operator=( const InvalidLogins& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  InvalidLogins();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  InvalidLogins( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& _m0 );

  #ifdef HAVE_STD_EH
  InvalidLogins *operator->() { return this; }
  InvalidLogins& operator*() { return *this; }
  operator InvalidLogins*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static InvalidLogins *_downcast( CORBA::Exception *ex );
  static const InvalidLogins *_downcast( const CORBA::Exception *ex );
  IdentifierSeq loginIds;
};

#ifdef HAVE_STD_EH
typedef InvalidLogins InvalidLogins_catch;
#else
typedef ExceptVar< InvalidLogins > InvalidLogins_var;
typedef TVarOut< InvalidLogins > InvalidLogins_out;
typedef InvalidLogins_var InvalidLogins_catch;
#endif // HAVE_STD_EH

extern CORBA::TypeCodeConst _tc_InvalidLogins;


/*
 * Base class and common definitions for interface LoginObserver
 */

class LoginObserver : 
  virtual public CORBA::Object
{
  public:
    virtual ~LoginObserver();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef LoginObserver_ptr _ptr_type;
    typedef LoginObserver_var _var_type;
    #endif

    static LoginObserver_ptr _narrow( CORBA::Object_ptr obj );
    static LoginObserver_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static LoginObserver_ptr _duplicate( LoginObserver_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static LoginObserver_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual void entityLogout( const ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo& login ) = 0;

  protected:
    LoginObserver() {};
  private:
    LoginObserver( const LoginObserver& );
    void operator=( const LoginObserver& );
};

extern CORBA::TypeCodeConst _tc_LoginObserver;

// Stub for interface LoginObserver
class LoginObserver_stub:
  virtual public LoginObserver
{
  public:
    virtual ~LoginObserver_stub();
    void entityLogout( const ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo& login );

  private:
    void operator=( const LoginObserver_stub& );
};

#ifndef MICO_CONF_NO_POA

class LoginObserver_stub_clp :
  virtual public LoginObserver_stub,
  virtual public PortableServer::StubBase
{
  public:
    LoginObserver_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~LoginObserver_stub_clp ();
    void entityLogout( const ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo& login );

  protected:
    LoginObserver_stub_clp ();
  private:
    void operator=( const LoginObserver_stub_clp & );
};

#endif // MICO_CONF_NO_POA


/*
 * Base class and common definitions for interface LoginObserverSubscription
 */

class LoginObserverSubscription : 
  virtual public CORBA::Object
{
  public:
    virtual ~LoginObserverSubscription();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef LoginObserverSubscription_ptr _ptr_type;
    typedef LoginObserverSubscription_var _var_type;
    #endif

    static LoginObserverSubscription_ptr _narrow( CORBA::Object_ptr obj );
    static LoginObserverSubscription_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static LoginObserverSubscription_ptr _duplicate( LoginObserverSubscription_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static LoginObserverSubscription_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual CORBA::Boolean watchLogin( const char* loginId ) = 0;
    virtual void forgetLogin( const char* loginId ) = 0;
    virtual void watchLogins( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds ) = 0;
    virtual void forgetLogins( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getWatchedLogins() = 0;
    virtual void remove() = 0;

  protected:
    LoginObserverSubscription() {};
  private:
    LoginObserverSubscription( const LoginObserverSubscription& );
    void operator=( const LoginObserverSubscription& );
};

extern CORBA::TypeCodeConst _tc_LoginObserverSubscription;

// Stub for interface LoginObserverSubscription
class LoginObserverSubscription_stub:
  virtual public LoginObserverSubscription
{
  public:
    virtual ~LoginObserverSubscription_stub();
    CORBA::Boolean watchLogin( const char* loginId );
    void forgetLogin( const char* loginId );
    void watchLogins( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds );
    void forgetLogins( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds );
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getWatchedLogins();
    void remove();

  private:
    void operator=( const LoginObserverSubscription_stub& );
};

#ifndef MICO_CONF_NO_POA

class LoginObserverSubscription_stub_clp :
  virtual public LoginObserverSubscription_stub,
  virtual public PortableServer::StubBase
{
  public:
    LoginObserverSubscription_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~LoginObserverSubscription_stub_clp ();
    CORBA::Boolean watchLogin( const char* loginId );
    void forgetLogin( const char* loginId );
    void watchLogins( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds );
    void forgetLogins( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds );
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getWatchedLogins();
    void remove();

  protected:
    LoginObserverSubscription_stub_clp ();
  private:
    void operator=( const LoginObserverSubscription_stub_clp & );
};

#endif // MICO_CONF_NO_POA


/*
 * Base class and common definitions for interface LoginRegistry
 */

class LoginRegistry : 
  virtual public CORBA::Object
{
  public:
    virtual ~LoginRegistry();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef LoginRegistry_ptr _ptr_type;
    typedef LoginRegistry_var _var_type;
    #endif

    static LoginRegistry_ptr _narrow( CORBA::Object_ptr obj );
    static LoginRegistry_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static LoginRegistry_ptr _duplicate( LoginRegistry_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static LoginRegistry_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getAllLogins() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getEntityLogins( const char* entity ) = 0;
    virtual CORBA::Boolean invalidateLogin( const char* loginId ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo* getLoginInfo( const char* loginId, ::tecgraf::openbus::core::v2_00::OctetSeq_out pubkey ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTimeSeq* getValidity( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr subscribeObserver( ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr callback ) = 0;

  protected:
    LoginRegistry() {};
  private:
    LoginRegistry( const LoginRegistry& );
    void operator=( const LoginRegistry& );
};

extern CORBA::TypeCodeConst _tc_LoginRegistry;

// Stub for interface LoginRegistry
class LoginRegistry_stub:
  virtual public LoginRegistry
{
  public:
    virtual ~LoginRegistry_stub();
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getAllLogins();
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getEntityLogins( const char* entity );
    CORBA::Boolean invalidateLogin( const char* loginId );
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo* getLoginInfo( const char* loginId, ::tecgraf::openbus::core::v2_00::OctetSeq_out pubkey );
    ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTimeSeq* getValidity( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds );
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr subscribeObserver( ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr callback );

  private:
    void operator=( const LoginRegistry_stub& );
};

#ifndef MICO_CONF_NO_POA

class LoginRegistry_stub_clp :
  virtual public LoginRegistry_stub,
  virtual public PortableServer::StubBase
{
  public:
    LoginRegistry_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~LoginRegistry_stub_clp ();
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getAllLogins();
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getEntityLogins( const char* entity );
    CORBA::Boolean invalidateLogin( const char* loginId );
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo* getLoginInfo( const char* loginId, ::tecgraf::openbus::core::v2_00::OctetSeq_out pubkey );
    ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTimeSeq* getValidity( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds );
    ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr subscribeObserver( ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr callback );

  protected:
    LoginRegistry_stub_clp ();
  private:
    void operator=( const LoginRegistry_stub_clp & );
};

#endif // MICO_CONF_NO_POA

}
}
}
}
}
}


#ifndef MICO_CONF_NO_POA



namespace POA_tecgraf
{
namespace openbus
{
namespace core
{
namespace v2_00
{
namespace services
{
namespace access_control
{

class CertificateRegistry : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~CertificateRegistry ();
    tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static CertificateRegistry * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual void registerCertificate( const char* entity, const ::tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate& cert ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate* getCertificate( const char* entity ) = 0;
    virtual CORBA::Boolean removeCertificate( const char* entity ) = 0;

  protected:
    CertificateRegistry () {};

  private:
    CertificateRegistry (const CertificateRegistry &);
    void operator= (const CertificateRegistry &);
};

class LoginByCertificate : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~LoginByCertificate ();
    tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static LoginByCertificate * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual char* login( const ::tecgraf::openbus::core::v2_00::OctetSeq& pubkey, const ::tecgraf::openbus::core::v2_00::EncryptedBlock encrypted, ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out validity ) = 0;
    virtual void cancel() = 0;

  protected:
    LoginByCertificate () {};

  private:
    LoginByCertificate (const LoginByCertificate &);
    void operator= (const LoginByCertificate &);
};

class AccessControl : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~AccessControl ();
    tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static AccessControl * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual char* busid() = 0;
    virtual ::tecgraf::openbus::core::v2_00::OctetSeq* buskey() = 0;

    virtual char* loginByPassword( const char* entity, const ::tecgraf::openbus::core::v2_00::OctetSeq& pubkey, const ::tecgraf::openbus::core::v2_00::EncryptedBlock encrypted, ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out validity ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr startLoginByCertificate( const char* entity, ::tecgraf::openbus::core::v2_00::EncryptedBlock_out challenge ) = 0;
    virtual void logout() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime renew() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain* signChainFor( const char* target ) = 0;

  protected:
    AccessControl () {};

  private:
    AccessControl (const AccessControl &);
    void operator= (const AccessControl &);
};

class LoginObserver : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~LoginObserver ();
    tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static LoginObserver * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual void entityLogout( const ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo& login ) = 0;

  protected:
    LoginObserver () {};

  private:
    LoginObserver (const LoginObserver &);
    void operator= (const LoginObserver &);
};

class LoginObserverSubscription : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~LoginObserverSubscription ();
    tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static LoginObserverSubscription * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual CORBA::Boolean watchLogin( const char* loginId ) = 0;
    virtual void forgetLogin( const char* loginId ) = 0;
    virtual void watchLogins( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds ) = 0;
    virtual void forgetLogins( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getWatchedLogins() = 0;
    virtual void remove() = 0;

  protected:
    LoginObserverSubscription () {};

  private:
    LoginObserverSubscription (const LoginObserverSubscription &);
    void operator= (const LoginObserverSubscription &);
};

class LoginRegistry : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~LoginRegistry ();
    tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static LoginRegistry * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getAllLogins() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* getEntityLogins( const char* entity ) = 0;
    virtual CORBA::Boolean invalidateLogin( const char* loginId ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo* getLoginInfo( const char* loginId, ::tecgraf::openbus::core::v2_00::OctetSeq_out pubkey ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTimeSeq* getValidity( const ::tecgraf::openbus::core::v2_00::IdentifierSeq& loginIds ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr subscribeObserver( ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr callback ) = 0;

  protected:
    LoginRegistry () {};

  private:
    LoginRegistry (const LoginRegistry &);
    void operator= (const LoginRegistry &);
};

}
}
}
}
}
}


#endif // MICO_CONF_NO_POA

void operator<<=( CORBA::Any &a, const ::tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate &e );
void operator<<=( CORBA::Any &a, ::tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate *e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate &e );

CORBA::Boolean operator>>=( const CORBA::Any &a, const ::tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate *&e );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate;

void operator<<=( CORBA::Any &a, const ::tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate &e );
void operator<<=( CORBA::Any &a, ::tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate *e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate &e );

CORBA::Boolean operator>>=( const CORBA::Any &a, const ::tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate *&e );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate;

void operator<<=( CORBA::Any &a, const tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr obj );
void operator<<=( CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry;

void operator<<=( CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo &_s );
void operator<<=( CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo *&_s );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo;

void operator<<=( CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo &_s );
void operator<<=( CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo *&_s );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo;

void operator<<=( CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::services::access_control::CallChain &_s );
void operator<<=( CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::services::access_control::CallChain *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::services::access_control::CallChain &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::services::access_control::CallChain *&_s );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain;

void operator<<=( CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain &_s );
void operator<<=( CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain *&_s );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain;

void operator<<=( CORBA::Any &a, const ::tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding &e );
void operator<<=( CORBA::Any &a, ::tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding *e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding &e );

CORBA::Boolean operator>>=( const CORBA::Any &a, const ::tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding *&e );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding;

void operator<<=( CORBA::Any &a, const ::tecgraf::openbus::core::v2_00::services::access_control::AccessDenied &e );
void operator<<=( CORBA::Any &a, ::tecgraf::openbus::core::v2_00::services::access_control::AccessDenied *e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::tecgraf::openbus::core::v2_00::services::access_control::AccessDenied &e );

CORBA::Boolean operator>>=( const CORBA::Any &a, const ::tecgraf::openbus::core::v2_00::services::access_control::AccessDenied *&e );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied;

void operator<<=( CORBA::Any &a, const tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr obj );
void operator<<=( CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate;

void operator<<=( CORBA::Any &a, const tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr obj );
void operator<<=( CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl;

void operator<<=( CORBA::Any &a, const ::tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins &e );
void operator<<=( CORBA::Any &a, ::tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins *e );
CORBA::Boolean operator>>=( const CORBA::Any &a, ::tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins &e );

CORBA::Boolean operator>>=( const CORBA::Any &a, const ::tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins *&e );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins;

void operator<<=( CORBA::Any &a, const tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr obj );
void operator<<=( CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver;

void operator<<=( CORBA::Any &a, const tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr obj );
void operator<<=( CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription;

void operator<<=( CORBA::Any &a, const tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr obj );
void operator<<=( CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr* obj_ptr );
CORBA::Boolean operator>>=( const CORBA::Any &a, tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr &obj );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry;

void operator<<=( CORBA::Any &_a, const SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::LoginInfo,MICO_TID_DEF> &_s );
void operator<<=( CORBA::Any &_a, SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::LoginInfo,MICO_TID_DEF> *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::LoginInfo,MICO_TID_DEF> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, const SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::LoginInfo,MICO_TID_DEF> *&_s );

extern CORBA::StaticTypeInfo *_marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo;

void operator<<=( CORBA::Any &_a, const SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::ValidityTime,MICO_TID_DEF> &_s );
void operator<<=( CORBA::Any &_a, SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::ValidityTime,MICO_TID_DEF> *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::ValidityTime,MICO_TID_DEF> &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, const SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::ValidityTime,MICO_TID_DEF> *&_s );

#endif
