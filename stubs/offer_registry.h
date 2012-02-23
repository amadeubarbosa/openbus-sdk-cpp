/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2006 by The Mico Team
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <CORBA.h>
#include <mico/throw.h>

#ifndef __OFFER_REGISTRY_H__
#define __OFFER_REGISTRY_H__






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
namespace offer_registry
{

class ServiceOffer;
typedef ServiceOffer *ServiceOffer_ptr;
typedef ServiceOffer_ptr ServiceOfferRef;
typedef ObjVar< ServiceOffer > ServiceOffer_var;
typedef ObjOut< ServiceOffer > ServiceOffer_out;

class OfferRegistry;
typedef OfferRegistry *OfferRegistry_ptr;
typedef OfferRegistry_ptr OfferRegistryRef;
typedef ObjVar< OfferRegistry > OfferRegistry_var;
typedef ObjOut< OfferRegistry > OfferRegistry_out;

class EntityCategory;
typedef EntityCategory *EntityCategory_ptr;
typedef EntityCategory_ptr EntityCategoryRef;
typedef ObjVar< EntityCategory > EntityCategory_var;
typedef ObjOut< EntityCategory > EntityCategory_out;

class RegisteredEntity;
typedef RegisteredEntity *RegisteredEntity_ptr;
typedef RegisteredEntity_ptr RegisteredEntityRef;
typedef ObjVar< RegisteredEntity > RegisteredEntity_var;
typedef ObjOut< RegisteredEntity > RegisteredEntity_out;

class EntityRegistry;
typedef EntityRegistry *EntityRegistry_ptr;
typedef EntityRegistry_ptr EntityRegistryRef;
typedef ObjVar< EntityRegistry > EntityRegistry_var;
typedef ObjOut< EntityRegistry > EntityRegistry_out;

class InterfaceRegistry;
typedef InterfaceRegistry *InterfaceRegistry_ptr;
typedef InterfaceRegistry_ptr InterfaceRegistryRef;
typedef ObjVar< InterfaceRegistry > InterfaceRegistry_var;
typedef ObjOut< InterfaceRegistry > InterfaceRegistry_out;

}
}
}
}
}
}



#include <scs.h>
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
namespace offer_registry
{

const char* const OfferRegistryFacet = "OfferRegistry_2_00";
const char* const EntityRegistryFacet = "EntityRegistry_2_00";
const char* const InterfaceRegistryFacet = "InterfaceRegistry_2_00";
typedef ::scs::core::IComponent OfferedService;
typedef ::scs::core::IComponent_ptr OfferedService_ptr;
typedef ::scs::core::IComponentRef OfferedServiceRef;
typedef ::scs::core::IComponent_var OfferedService_var;
typedef ::scs::core::IComponent_out OfferedService_out;

struct ServiceProperty;
typedef TVarVar< ServiceProperty > ServiceProperty_var;
typedef TVarOut< ServiceProperty > ServiceProperty_out;


typedef SequenceTmpl< ServiceProperty,MICO_TID_DEF> ServicePropertySeq;
typedef TSeqVar< SequenceTmpl< ServiceProperty,MICO_TID_DEF> > ServicePropertySeq_var;
typedef TSeqOut< SequenceTmpl< ServiceProperty,MICO_TID_DEF> > ServicePropertySeq_out;

struct ServiceOfferDesc;
typedef TVarVar< ServiceOfferDesc > ServiceOfferDesc_var;
typedef TVarOut< ServiceOfferDesc > ServiceOfferDesc_out;


struct ServiceOfferDesc {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef ServiceOfferDesc_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  ServiceOfferDesc();
  ~ServiceOfferDesc();
  ServiceOfferDesc( const ServiceOfferDesc& s );
  ServiceOfferDesc& operator=( const ServiceOfferDesc& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  OfferedService_var service_ref;
  ServicePropertySeq properties;
  ServiceOffer_var ref;
};

struct InvalidProperties : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  InvalidProperties();
  ~InvalidProperties();
  InvalidProperties( const InvalidProperties& s );
  InvalidProperties& operator=( const InvalidProperties& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  InvalidProperties();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  InvalidProperties( const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& _m0 );

  #ifdef HAVE_STD_EH
  InvalidProperties *operator->() { return this; }
  InvalidProperties& operator*() { return *this; }
  operator InvalidProperties*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static InvalidProperties *_downcast( CORBA::Exception *ex );
  static const InvalidProperties *_downcast( const CORBA::Exception *ex );
  ServicePropertySeq properties;
};

#ifdef HAVE_STD_EH
typedef InvalidProperties InvalidProperties_catch;
#else
typedef ExceptVar< InvalidProperties > InvalidProperties_var;
typedef TVarOut< InvalidProperties > InvalidProperties_out;
typedef InvalidProperties_var InvalidProperties_catch;
#endif // HAVE_STD_EH


/*
 * Base class and common definitions for interface ServiceOffer
 */

class ServiceOffer : 
  virtual public CORBA::Object
{
  public:
    virtual ~ServiceOffer();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef ServiceOffer_ptr _ptr_type;
    typedef ServiceOffer_var _var_type;
    #endif

    static ServiceOffer_ptr _narrow( CORBA::Object_ptr obj );
    static ServiceOffer_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static ServiceOffer_ptr _duplicate( ServiceOffer_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static ServiceOffer_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr service_ref() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq* properties() = 0;

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc* describe() = 0;
    virtual void setProperties( const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties ) = 0;
    virtual void remove() = 0;

  protected:
    ServiceOffer() {};
  private:
    ServiceOffer( const ServiceOffer& );
    void operator=( const ServiceOffer& );
};

// Stub for interface ServiceOffer
class ServiceOffer_stub:
  virtual public ServiceOffer
{
  public:
    virtual ~ServiceOffer_stub();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr service_ref();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq* properties();

    ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc* describe();
    void setProperties( const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties );
    void remove();

  private:
    void operator=( const ServiceOffer_stub& );
};

#ifndef MICO_CONF_NO_POA

class ServiceOffer_stub_clp :
  virtual public ServiceOffer_stub,
  virtual public PortableServer::StubBase
{
  public:
    ServiceOffer_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~ServiceOffer_stub_clp ();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr service_ref();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq* properties();

    ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc* describe();
    void setProperties( const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties );
    void remove();

  protected:
    ServiceOffer_stub_clp ();
  private:
    void operator=( const ServiceOffer_stub_clp & );
};

#endif // MICO_CONF_NO_POA

typedef char* FacetName;
typedef CORBA::String_var FacetName_var;
typedef CORBA::String_out FacetName_out;

typedef StringSequenceTmpl<CORBA::String_var> FacetNameSeq;
typedef TSeqVar< StringSequenceTmpl<CORBA::String_var> > FacetNameSeq_var;
typedef TSeqOut< StringSequenceTmpl<CORBA::String_var> > FacetNameSeq_out;

struct ServiceProperty {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef ServiceProperty_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  ServiceProperty();
  ~ServiceProperty();
  ServiceProperty( const ServiceProperty& s );
  ServiceProperty& operator=( const ServiceProperty& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  CORBA::String_var name;
  CORBA::String_var value;
};

typedef SequenceTmpl< ServiceOfferDesc,MICO_TID_DEF> ServiceOfferDescSeq;
typedef TSeqVar< SequenceTmpl< ServiceOfferDesc,MICO_TID_DEF> > ServiceOfferDescSeq_var;
typedef TSeqOut< SequenceTmpl< ServiceOfferDesc,MICO_TID_DEF> > ServiceOfferDescSeq_out;

struct UnauthorizedFacets : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  UnauthorizedFacets();
  ~UnauthorizedFacets();
  UnauthorizedFacets( const UnauthorizedFacets& s );
  UnauthorizedFacets& operator=( const UnauthorizedFacets& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  UnauthorizedFacets();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  UnauthorizedFacets( const ::tecgraf::openbus::core::v2_00::services::offer_registry::FacetNameSeq& _m0 );

  #ifdef HAVE_STD_EH
  UnauthorizedFacets *operator->() { return this; }
  UnauthorizedFacets& operator*() { return *this; }
  operator UnauthorizedFacets*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static UnauthorizedFacets *_downcast( CORBA::Exception *ex );
  static const UnauthorizedFacets *_downcast( const CORBA::Exception *ex );
  FacetNameSeq facets;
};

#ifdef HAVE_STD_EH
typedef UnauthorizedFacets UnauthorizedFacets_catch;
#else
typedef ExceptVar< UnauthorizedFacets > UnauthorizedFacets_var;
typedef TVarOut< UnauthorizedFacets > UnauthorizedFacets_out;
typedef UnauthorizedFacets_var UnauthorizedFacets_catch;
#endif // HAVE_STD_EH

struct InvalidService : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  InvalidService();
  ~InvalidService();
  InvalidService( const InvalidService& s );
  InvalidService& operator=( const InvalidService& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS


  #ifdef HAVE_STD_EH
  InvalidService *operator->() { return this; }
  InvalidService& operator*() { return *this; }
  operator InvalidService*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static InvalidService *_downcast( CORBA::Exception *ex );
  static const InvalidService *_downcast( const CORBA::Exception *ex );
};

#ifdef HAVE_STD_EH
typedef InvalidService InvalidService_catch;
#else
typedef ExceptVar< InvalidService > InvalidService_var;
typedef TVarOut< InvalidService > InvalidService_out;
typedef InvalidService_var InvalidService_catch;
#endif // HAVE_STD_EH


/*
 * Base class and common definitions for interface OfferRegistry
 */

class OfferRegistry : 
  virtual public CORBA::Object
{
  public:
    virtual ~OfferRegistry();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef OfferRegistry_ptr _ptr_type;
    typedef OfferRegistry_var _var_type;
    #endif

    static OfferRegistry_ptr _narrow( CORBA::Object_ptr obj );
    static OfferRegistry_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static OfferRegistry_ptr _duplicate( OfferRegistry_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static OfferRegistry_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr registerService( ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr service_ref, const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* findServices( const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* getServices() = 0;

  protected:
    OfferRegistry() {};
  private:
    OfferRegistry( const OfferRegistry& );
    void operator=( const OfferRegistry& );
};

// Stub for interface OfferRegistry
class OfferRegistry_stub:
  virtual public OfferRegistry
{
  public:
    virtual ~OfferRegistry_stub();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr registerService( ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr service_ref, const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* findServices( const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* getServices();

  private:
    void operator=( const OfferRegistry_stub& );
};

#ifndef MICO_CONF_NO_POA

class OfferRegistry_stub_clp :
  virtual public OfferRegistry_stub,
  virtual public PortableServer::StubBase
{
  public:
    OfferRegistry_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~OfferRegistry_stub_clp ();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr registerService( ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr service_ref, const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* findServices( const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* getServices();

  protected:
    OfferRegistry_stub_clp ();
  private:
    void operator=( const OfferRegistry_stub_clp & );
};

#endif // MICO_CONF_NO_POA

struct RegisteredEntityDesc;
typedef TVarVar< RegisteredEntityDesc > RegisteredEntityDesc_var;
typedef TVarOut< RegisteredEntityDesc > RegisteredEntityDesc_out;


struct RegisteredEntityDesc {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef RegisteredEntityDesc_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  RegisteredEntityDesc();
  ~RegisteredEntityDesc();
  RegisteredEntityDesc( const RegisteredEntityDesc& s );
  RegisteredEntityDesc& operator=( const RegisteredEntityDesc& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  EntityCategory_var category;
  Identifier_var id;
  CORBA::String_var name;
  RegisteredEntity_var ref;
};

typedef char* InterfaceId;
typedef CORBA::String_var InterfaceId_var;
typedef CORBA::String_out InterfaceId_out;

struct InvalidInterface : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  InvalidInterface();
  ~InvalidInterface();
  InvalidInterface( const InvalidInterface& s );
  InvalidInterface& operator=( const InvalidInterface& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  InvalidInterface();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  InvalidInterface( const char* _m0 );

  #ifdef HAVE_STD_EH
  InvalidInterface *operator->() { return this; }
  InvalidInterface& operator*() { return *this; }
  operator InvalidInterface*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static InvalidInterface *_downcast( CORBA::Exception *ex );
  static const InvalidInterface *_downcast( const CORBA::Exception *ex );
  InterfaceId_var ifaceId;
};

#ifdef HAVE_STD_EH
typedef InvalidInterface InvalidInterface_catch;
#else
typedef ExceptVar< InvalidInterface > InvalidInterface_var;
typedef TVarOut< InvalidInterface > InvalidInterface_out;
typedef InvalidInterface_var InvalidInterface_catch;
#endif // HAVE_STD_EH

struct AuthorizationInUse : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  AuthorizationInUse();
  ~AuthorizationInUse();
  AuthorizationInUse( const AuthorizationInUse& s );
  AuthorizationInUse& operator=( const AuthorizationInUse& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  AuthorizationInUse();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  AuthorizationInUse( const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq& _m0 );

  #ifdef HAVE_STD_EH
  AuthorizationInUse *operator->() { return this; }
  AuthorizationInUse& operator*() { return *this; }
  operator AuthorizationInUse*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static AuthorizationInUse *_downcast( CORBA::Exception *ex );
  static const AuthorizationInUse *_downcast( const CORBA::Exception *ex );
  ServiceOfferDescSeq offers;
};

#ifdef HAVE_STD_EH
typedef AuthorizationInUse AuthorizationInUse_catch;
#else
typedef ExceptVar< AuthorizationInUse > AuthorizationInUse_var;
typedef TVarOut< AuthorizationInUse > AuthorizationInUse_out;
typedef AuthorizationInUse_var AuthorizationInUse_catch;
#endif // HAVE_STD_EH

typedef StringSequenceTmpl<CORBA::String_var> InterfaceIdSeq;
typedef TSeqVar< StringSequenceTmpl<CORBA::String_var> > InterfaceIdSeq_var;
typedef TSeqOut< StringSequenceTmpl<CORBA::String_var> > InterfaceIdSeq_out;


/*
 * Base class and common definitions for interface RegisteredEntity
 */

class RegisteredEntity : 
  virtual public CORBA::Object
{
  public:
    virtual ~RegisteredEntity();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef RegisteredEntity_ptr _ptr_type;
    typedef RegisteredEntity_var _var_type;
    #endif

    static RegisteredEntity_ptr _narrow( CORBA::Object_ptr obj );
    static RegisteredEntity_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static RegisteredEntity_ptr _duplicate( RegisteredEntity_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static RegisteredEntity_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr category() = 0;
    virtual char* id() = 0;
    virtual char* name() = 0;

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc* describe() = 0;
    virtual void setName( const char* name ) = 0;
    virtual void remove() = 0;
    virtual CORBA::Boolean grantInterface( const char* ifaceId ) = 0;
    virtual CORBA::Boolean revokeInterface( const char* ifaceId ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* getGrantedInterfaces() = 0;

  protected:
    RegisteredEntity() {};
  private:
    RegisteredEntity( const RegisteredEntity& );
    void operator=( const RegisteredEntity& );
};

// Stub for interface RegisteredEntity
class RegisteredEntity_stub:
  virtual public RegisteredEntity
{
  public:
    virtual ~RegisteredEntity_stub();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr category();
    char* id();
    char* name();

    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc* describe();
    void setName( const char* name );
    void remove();
    CORBA::Boolean grantInterface( const char* ifaceId );
    CORBA::Boolean revokeInterface( const char* ifaceId );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* getGrantedInterfaces();

  private:
    void operator=( const RegisteredEntity_stub& );
};

#ifndef MICO_CONF_NO_POA

class RegisteredEntity_stub_clp :
  virtual public RegisteredEntity_stub,
  virtual public PortableServer::StubBase
{
  public:
    RegisteredEntity_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~RegisteredEntity_stub_clp ();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr category();
    char* id();
    char* name();

    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc* describe();
    void setName( const char* name );
    void remove();
    CORBA::Boolean grantInterface( const char* ifaceId );
    CORBA::Boolean revokeInterface( const char* ifaceId );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* getGrantedInterfaces();

  protected:
    RegisteredEntity_stub_clp ();
  private:
    void operator=( const RegisteredEntity_stub_clp & );
};

#endif // MICO_CONF_NO_POA

struct EntityCategoryDesc;
typedef TVarVar< EntityCategoryDesc > EntityCategoryDesc_var;
typedef TVarOut< EntityCategoryDesc > EntityCategoryDesc_out;


struct EntityCategoryDesc {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef EntityCategoryDesc_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  EntityCategoryDesc();
  ~EntityCategoryDesc();
  EntityCategoryDesc( const EntityCategoryDesc& s );
  EntityCategoryDesc& operator=( const EntityCategoryDesc& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  Identifier_var id;
  CORBA::String_var name;
  EntityCategory_var ref;
};

typedef SequenceTmpl< RegisteredEntityDesc,MICO_TID_DEF> RegisteredEntityDescSeq;
typedef TSeqVar< SequenceTmpl< RegisteredEntityDesc,MICO_TID_DEF> > RegisteredEntityDescSeq_var;
typedef TSeqOut< SequenceTmpl< RegisteredEntityDesc,MICO_TID_DEF> > RegisteredEntityDescSeq_out;

struct EntityCategoryInUse : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  EntityCategoryInUse();
  ~EntityCategoryInUse();
  EntityCategoryInUse( const EntityCategoryInUse& s );
  EntityCategoryInUse& operator=( const EntityCategoryInUse& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  EntityCategoryInUse();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  EntityCategoryInUse( const ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq& _m0 );

  #ifdef HAVE_STD_EH
  EntityCategoryInUse *operator->() { return this; }
  EntityCategoryInUse& operator*() { return *this; }
  operator EntityCategoryInUse*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static EntityCategoryInUse *_downcast( CORBA::Exception *ex );
  static const EntityCategoryInUse *_downcast( const CORBA::Exception *ex );
  RegisteredEntityDescSeq entities;
};

#ifdef HAVE_STD_EH
typedef EntityCategoryInUse EntityCategoryInUse_catch;
#else
typedef ExceptVar< EntityCategoryInUse > EntityCategoryInUse_var;
typedef TVarOut< EntityCategoryInUse > EntityCategoryInUse_out;
typedef EntityCategoryInUse_var EntityCategoryInUse_catch;
#endif // HAVE_STD_EH

struct EntityAlreadyRegistered : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  EntityAlreadyRegistered();
  ~EntityAlreadyRegistered();
  EntityAlreadyRegistered( const EntityAlreadyRegistered& s );
  EntityAlreadyRegistered& operator=( const EntityAlreadyRegistered& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  EntityAlreadyRegistered();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  EntityAlreadyRegistered( const ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc& _m0 );

  #ifdef HAVE_STD_EH
  EntityAlreadyRegistered *operator->() { return this; }
  EntityAlreadyRegistered& operator*() { return *this; }
  operator EntityAlreadyRegistered*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static EntityAlreadyRegistered *_downcast( CORBA::Exception *ex );
  static const EntityAlreadyRegistered *_downcast( const CORBA::Exception *ex );
  RegisteredEntityDesc existing;
};

#ifdef HAVE_STD_EH
typedef EntityAlreadyRegistered EntityAlreadyRegistered_catch;
#else
typedef ExceptVar< EntityAlreadyRegistered > EntityAlreadyRegistered_var;
typedef TVarOut< EntityAlreadyRegistered > EntityAlreadyRegistered_out;
typedef EntityAlreadyRegistered_var EntityAlreadyRegistered_catch;
#endif // HAVE_STD_EH


/*
 * Base class and common definitions for interface EntityCategory
 */

class EntityCategory : 
  virtual public CORBA::Object
{
  public:
    virtual ~EntityCategory();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef EntityCategory_ptr _ptr_type;
    typedef EntityCategory_var _var_type;
    #endif

    static EntityCategory_ptr _narrow( CORBA::Object_ptr obj );
    static EntityCategory_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static EntityCategory_ptr _duplicate( EntityCategory_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static EntityCategory_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual char* id() = 0;
    virtual char* name() = 0;

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc* describe() = 0;
    virtual void setName( const char* name ) = 0;
    virtual void remove() = 0;
    virtual void removeAll() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr registerEntity( const char* id, const char* name ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntities() = 0;

  protected:
    EntityCategory() {};
  private:
    EntityCategory( const EntityCategory& );
    void operator=( const EntityCategory& );
};

// Stub for interface EntityCategory
class EntityCategory_stub:
  virtual public EntityCategory
{
  public:
    virtual ~EntityCategory_stub();
    char* id();
    char* name();

    ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc* describe();
    void setName( const char* name );
    void remove();
    void removeAll();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr registerEntity( const char* id, const char* name );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntities();

  private:
    void operator=( const EntityCategory_stub& );
};

#ifndef MICO_CONF_NO_POA

class EntityCategory_stub_clp :
  virtual public EntityCategory_stub,
  virtual public PortableServer::StubBase
{
  public:
    EntityCategory_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~EntityCategory_stub_clp ();
    char* id();
    char* name();

    ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc* describe();
    void setName( const char* name );
    void remove();
    void removeAll();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr registerEntity( const char* id, const char* name );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntities();

  protected:
    EntityCategory_stub_clp ();
  private:
    void operator=( const EntityCategory_stub_clp & );
};

#endif // MICO_CONF_NO_POA

typedef SequenceTmpl< EntityCategoryDesc,MICO_TID_DEF> EntityCategoryDescSeq;
typedef TSeqVar< SequenceTmpl< EntityCategoryDesc,MICO_TID_DEF> > EntityCategoryDescSeq_var;
typedef TSeqOut< SequenceTmpl< EntityCategoryDesc,MICO_TID_DEF> > EntityCategoryDescSeq_out;

struct EntityCategoryAlreadyExists : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  EntityCategoryAlreadyExists();
  ~EntityCategoryAlreadyExists();
  EntityCategoryAlreadyExists( const EntityCategoryAlreadyExists& s );
  EntityCategoryAlreadyExists& operator=( const EntityCategoryAlreadyExists& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  EntityCategoryAlreadyExists();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  EntityCategoryAlreadyExists( const ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc& _m0 );

  #ifdef HAVE_STD_EH
  EntityCategoryAlreadyExists *operator->() { return this; }
  EntityCategoryAlreadyExists& operator*() { return *this; }
  operator EntityCategoryAlreadyExists*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static EntityCategoryAlreadyExists *_downcast( CORBA::Exception *ex );
  static const EntityCategoryAlreadyExists *_downcast( const CORBA::Exception *ex );
  EntityCategoryDesc existing;
};

#ifdef HAVE_STD_EH
typedef EntityCategoryAlreadyExists EntityCategoryAlreadyExists_catch;
#else
typedef ExceptVar< EntityCategoryAlreadyExists > EntityCategoryAlreadyExists_var;
typedef TVarOut< EntityCategoryAlreadyExists > EntityCategoryAlreadyExists_out;
typedef EntityCategoryAlreadyExists_var EntityCategoryAlreadyExists_catch;
#endif // HAVE_STD_EH


/*
 * Base class and common definitions for interface EntityRegistry
 */

class EntityRegistry : 
  virtual public CORBA::Object
{
  public:
    virtual ~EntityRegistry();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef EntityRegistry_ptr _ptr_type;
    typedef EntityRegistry_var _var_type;
    #endif

    static EntityRegistry_ptr _narrow( CORBA::Object_ptr obj );
    static EntityRegistry_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static EntityRegistry_ptr _duplicate( EntityRegistry_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static EntityRegistry_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr createEntityCategory( const char* id, const char* name ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDescSeq* getEntityCategories() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr getEntityCategory( const char* id ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntities() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr getEntity( const char* id ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getAuthorizedEntities() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntitiesByAuthorizedInterfaces( const ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq& interfaces ) = 0;

  protected:
    EntityRegistry() {};
  private:
    EntityRegistry( const EntityRegistry& );
    void operator=( const EntityRegistry& );
};

// Stub for interface EntityRegistry
class EntityRegistry_stub:
  virtual public EntityRegistry
{
  public:
    virtual ~EntityRegistry_stub();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr createEntityCategory( const char* id, const char* name );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDescSeq* getEntityCategories();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr getEntityCategory( const char* id );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntities();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr getEntity( const char* id );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getAuthorizedEntities();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntitiesByAuthorizedInterfaces( const ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq& interfaces );

  private:
    void operator=( const EntityRegistry_stub& );
};

#ifndef MICO_CONF_NO_POA

class EntityRegistry_stub_clp :
  virtual public EntityRegistry_stub,
  virtual public PortableServer::StubBase
{
  public:
    EntityRegistry_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~EntityRegistry_stub_clp ();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr createEntityCategory( const char* id, const char* name );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDescSeq* getEntityCategories();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr getEntityCategory( const char* id );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntities();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr getEntity( const char* id );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getAuthorizedEntities();
    ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntitiesByAuthorizedInterfaces( const ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq& interfaces );

  protected:
    EntityRegistry_stub_clp ();
  private:
    void operator=( const EntityRegistry_stub_clp & );
};

#endif // MICO_CONF_NO_POA

struct InterfaceInUse : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  InterfaceInUse();
  ~InterfaceInUse();
  InterfaceInUse( const InterfaceInUse& s );
  InterfaceInUse& operator=( const InterfaceInUse& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  InterfaceInUse();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  InterfaceInUse( const ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq& _m0 );

  #ifdef HAVE_STD_EH
  InterfaceInUse *operator->() { return this; }
  InterfaceInUse& operator*() { return *this; }
  operator InterfaceInUse*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static InterfaceInUse *_downcast( CORBA::Exception *ex );
  static const InterfaceInUse *_downcast( const CORBA::Exception *ex );
  RegisteredEntityDescSeq entities;
};

#ifdef HAVE_STD_EH
typedef InterfaceInUse InterfaceInUse_catch;
#else
typedef ExceptVar< InterfaceInUse > InterfaceInUse_var;
typedef TVarOut< InterfaceInUse > InterfaceInUse_out;
typedef InterfaceInUse_var InterfaceInUse_catch;
#endif // HAVE_STD_EH


/*
 * Base class and common definitions for interface InterfaceRegistry
 */

class InterfaceRegistry : 
  virtual public CORBA::Object
{
  public:
    virtual ~InterfaceRegistry();

    #ifdef HAVE_TYPEDEF_OVERLOAD
    typedef InterfaceRegistry_ptr _ptr_type;
    typedef InterfaceRegistry_var _var_type;
    #endif

    static InterfaceRegistry_ptr _narrow( CORBA::Object_ptr obj );
    static InterfaceRegistry_ptr _narrow( CORBA::AbstractBase_ptr obj );
    static InterfaceRegistry_ptr _duplicate( InterfaceRegistry_ptr _obj )
    {
      CORBA::Object::_duplicate (_obj);
      return _obj;
    }

    static InterfaceRegistry_ptr _nil()
    {
      return 0;
    }

    virtual void *_narrow_helper( const char *repoid );

    virtual CORBA::Boolean registerInterface( const char* ifaceId ) = 0;
    virtual CORBA::Boolean removeInterface( const char* ifaceId ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* getInterfaces() = 0;

  protected:
    InterfaceRegistry() {};
  private:
    InterfaceRegistry( const InterfaceRegistry& );
    void operator=( const InterfaceRegistry& );
};

// Stub for interface InterfaceRegistry
class InterfaceRegistry_stub:
  virtual public InterfaceRegistry
{
  public:
    virtual ~InterfaceRegistry_stub();
    CORBA::Boolean registerInterface( const char* ifaceId );
    CORBA::Boolean removeInterface( const char* ifaceId );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* getInterfaces();

  private:
    void operator=( const InterfaceRegistry_stub& );
};

#ifndef MICO_CONF_NO_POA

class InterfaceRegistry_stub_clp :
  virtual public InterfaceRegistry_stub,
  virtual public PortableServer::StubBase
{
  public:
    InterfaceRegistry_stub_clp (PortableServer::POA_ptr, CORBA::Object_ptr);
    virtual ~InterfaceRegistry_stub_clp ();
    CORBA::Boolean registerInterface( const char* ifaceId );
    CORBA::Boolean removeInterface( const char* ifaceId );
    ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* getInterfaces();

  protected:
    InterfaceRegistry_stub_clp ();
  private:
    void operator=( const InterfaceRegistry_stub_clp & );
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
namespace offer_registry
{

class ServiceOffer : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~ServiceOffer ();
    tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static ServiceOffer * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr service_ref() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq* properties() = 0;

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc* describe() = 0;
    virtual void setProperties( const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties ) = 0;
    virtual void remove() = 0;

  protected:
    ServiceOffer () {};

  private:
    ServiceOffer (const ServiceOffer &);
    void operator= (const ServiceOffer &);
};

class OfferRegistry : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~OfferRegistry ();
    tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static OfferRegistry * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr registerService( ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr service_ref, const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* findServices( const ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& properties ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* getServices() = 0;

  protected:
    OfferRegistry () {};

  private:
    OfferRegistry (const OfferRegistry &);
    void operator= (const OfferRegistry &);
};

class RegisteredEntity : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~RegisteredEntity ();
    tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static RegisteredEntity * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr category() = 0;
    virtual char* id() = 0;
    virtual char* name() = 0;

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc* describe() = 0;
    virtual void setName( const char* name ) = 0;
    virtual void remove() = 0;
    virtual CORBA::Boolean grantInterface( const char* ifaceId ) = 0;
    virtual CORBA::Boolean revokeInterface( const char* ifaceId ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* getGrantedInterfaces() = 0;

  protected:
    RegisteredEntity () {};

  private:
    RegisteredEntity (const RegisteredEntity &);
    void operator= (const RegisteredEntity &);
};

class EntityCategory : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~EntityCategory ();
    tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static EntityCategory * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual char* id() = 0;
    virtual char* name() = 0;

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc* describe() = 0;
    virtual void setName( const char* name ) = 0;
    virtual void remove() = 0;
    virtual void removeAll() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr registerEntity( const char* id, const char* name ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntities() = 0;

  protected:
    EntityCategory () {};

  private:
    EntityCategory (const EntityCategory &);
    void operator= (const EntityCategory &);
};

class EntityRegistry : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~EntityRegistry ();
    tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static EntityRegistry * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr createEntityCategory( const char* id, const char* name ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDescSeq* getEntityCategories() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr getEntityCategory( const char* id ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntities() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr getEntity( const char* id ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getAuthorizedEntities() = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* getEntitiesByAuthorizedInterfaces( const ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq& interfaces ) = 0;

  protected:
    EntityRegistry () {};

  private:
    EntityRegistry (const EntityRegistry &);
    void operator= (const EntityRegistry &);
};

class InterfaceRegistry : virtual public PortableServer::StaticImplementation
{
  public:
    virtual ~InterfaceRegistry ();
    tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_ptr _this ();
    bool dispatch (CORBA::StaticServerRequest_ptr);
    virtual void invoke (CORBA::StaticServerRequest_ptr);
    virtual CORBA::Boolean _is_a (const char *);
    virtual CORBA::InterfaceDef_ptr _get_interface ();
    virtual CORBA::RepositoryId _primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr);

    virtual void * _narrow_helper (const char *);
    static InterfaceRegistry * _narrow (PortableServer::Servant);
    virtual CORBA::Object_ptr _make_stub (PortableServer::POA_ptr, CORBA::Object_ptr);

    virtual CORBA::Boolean registerInterface( const char* ifaceId ) = 0;
    virtual CORBA::Boolean removeInterface( const char* ifaceId ) = 0;
    virtual ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* getInterfaces() = 0;

  protected:
    InterfaceRegistry () {};

  private:
    InterfaceRegistry (const InterfaceRegistry &);
    void operator= (const InterfaceRegistry &);
};

}
}
}
}
}
}


#endif // MICO_CONF_NO_POA

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse;

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry;

extern CORBA::StaticTypeInfo *_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty;

extern CORBA::StaticTypeInfo *_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc;

extern CORBA::StaticTypeInfo *_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc;

extern CORBA::StaticTypeInfo *_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc;

#endif
