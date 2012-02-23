/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2006 by The Mico Team
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <offer_registry.h>


using namespace std;

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------


#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc::ServiceOfferDesc()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc::ServiceOfferDesc( const ServiceOfferDesc& _s )
{
  service_ref = ((ServiceOfferDesc&)_s).service_ref;
  properties = ((ServiceOfferDesc&)_s).properties;
  ref = ((ServiceOfferDesc&)_s).ref;
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc::~ServiceOfferDesc()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc&
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc::operator=( const ServiceOfferDesc& _s )
{
  service_ref = ((ServiceOfferDesc&)_s).service_ref;
  properties = ((ServiceOfferDesc&)_s).properties;
  ref = ((ServiceOfferDesc&)_s).ref;
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    _marshaller_scs_core_IComponent->demarshal( dc, &((_MICO_T*)v)->service_ref._for_demarshal() ) &&
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty->demarshal( dc, &((_MICO_T*)v)->properties ) &&
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer->demarshal( dc, &((_MICO_T*)v)->ref._for_demarshal() ) &&
    dc.struct_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  _marshaller_scs_core_IComponent->marshal( ec, &((_MICO_T*)v)->service_ref.inout() );
  _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty->marshal( ec, &((_MICO_T*)v)->properties );
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer->marshal( ec, &((_MICO_T*)v)->ref.inout() );
  ec.struct_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc;


#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::InvalidProperties()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::InvalidProperties( const InvalidProperties& _s )
{
  properties = ((InvalidProperties&)_s).properties;
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::~InvalidProperties()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties&
tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::operator=( const InvalidProperties& _s )
{
  properties = ((InvalidProperties&)_s).properties;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::InvalidProperties()
{
}

#endif

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::InvalidProperties( const tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& _m0 )
{
  properties = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty->demarshal( dc, &((_MICO_T*)v)->properties ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidProperties:1.0" );
  _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty->marshal( ec, &((_MICO_T*)v)->properties );
  ec.except_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties;

void tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw InvalidProperties_var( (tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidProperties:1.0";
}

void tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::_encode_any( CORBA::Any & ) const
{
  // use --any to make this work!
  assert(0);
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::_clone() const
{
  return new InvalidProperties( *this );
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidProperties:1.0" ) )
    return (InvalidProperties *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidProperties:1.0" ) )
    return (InvalidProperties *) _ex;
  return NULL;
}


/*
 * Base interface for class ServiceOffer
 */

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::~ServiceOffer()
{
}

void *
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/ServiceOffer:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/ServiceOffer:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/ServiceOffer:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/ServiceOffer:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer;


/*
 * Stub interface for class ServiceOffer
 */

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::~ServiceOffer_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/ServiceOffer:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/ServiceOffer:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub_clp::ServiceOffer_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub_clp::ServiceOffer_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub_clp::~ServiceOffer_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::service_ref()
{
  tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr _res = tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService::_nil();
  CORBA::StaticAny __res( _marshaller_scs_core_IComponent, &_res );

  CORBA::StaticRequest __req( this, "_get_service_ref" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub_clp::service_ref()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->service_ref();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::service_ref();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq* tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::properties()
{
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty );

  CORBA::StaticRequest __req( this, "_get_properties" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq*
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub_clp::properties()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->properties();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::properties();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc* tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::describe()
{
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc );

  CORBA::StaticRequest __req( this, "describe" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc*
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub_clp::describe()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->describe();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::describe();
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::setProperties( const tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& _par_properties )
{
  CORBA::StaticAny _sa_properties( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty, &_par_properties );
  CORBA::StaticRequest __req( this, "setProperties" );
  __req.add_in_arg( &_sa_properties );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidProperties:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub_clp::setProperties( const tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& _par_properties )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->setProperties(_par_properties);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return;
    }
    _postinvoke ();
  }

  tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::setProperties(_par_properties);
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::remove()
{
  CORBA::StaticRequest __req( this, "remove" );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub_clp::remove()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->remove();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return;
    }
    _postinvoke ();
  }

  tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub::remove();
}

#endif // MICO_CONF_NO_POA



#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceProperty::ServiceProperty()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceProperty::ServiceProperty( const ServiceProperty& _s )
{
  name = ((ServiceProperty&)_s).name;
  value = ((ServiceProperty&)_s).value;
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceProperty::~ServiceProperty()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceProperty&
tecgraf::openbus::core::v2_00::services::offer_registry::ServiceProperty::operator=( const ServiceProperty& _s )
{
  name = ((ServiceProperty&)_s).name;
  value = ((ServiceProperty&)_s).value;
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::offer_registry::ServiceProperty _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->name._for_demarshal() ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->value._for_demarshal() ) &&
    dc.struct_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->name.inout() );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->value.inout() );
  ec.struct_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty;



#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::UnauthorizedFacets()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::UnauthorizedFacets( const UnauthorizedFacets& _s )
{
  facets = ((UnauthorizedFacets&)_s).facets;
}

tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::~UnauthorizedFacets()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets&
tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::operator=( const UnauthorizedFacets& _s )
{
  facets = ((UnauthorizedFacets&)_s).facets;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::UnauthorizedFacets()
{
}

#endif

tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::UnauthorizedFacets( const tecgraf::openbus::core::v2_00::services::offer_registry::FacetNameSeq& _m0 )
{
  facets = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    CORBA::_stcseq_string->demarshal( dc, &((_MICO_T*)v)->facets ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/UnauthorizedFacets:1.0" );
  CORBA::_stcseq_string->marshal( ec, &((_MICO_T*)v)->facets );
  ec.except_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets;

void tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw UnauthorizedFacets_var( (tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/UnauthorizedFacets:1.0";
}

void tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::_encode_any( CORBA::Any & ) const
{
  // use --any to make this work!
  assert(0);
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::_clone() const
{
  return new UnauthorizedFacets( *this );
}

tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets *tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/UnauthorizedFacets:1.0" ) )
    return (UnauthorizedFacets *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets *tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/UnauthorizedFacets:1.0" ) )
    return (UnauthorizedFacets *) _ex;
  return NULL;
}


#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::InvalidService()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::InvalidService( const InvalidService& _s )
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::~InvalidService()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService&
tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::operator=( const InvalidService& _s )
{
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidService:1.0" );
  ec.except_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService;

void tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw InvalidService_var( (tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidService:1.0";
}

void tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::_encode_any( CORBA::Any & ) const
{
  // use --any to make this work!
  assert(0);
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::_clone() const
{
  return new InvalidService( *this );
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidService:1.0" ) )
    return (InvalidService *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidService:1.0" ) )
    return (InvalidService *) _ex;
  return NULL;
}


/*
 * Base interface for class OfferRegistry
 */

tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::~OfferRegistry()
{
}

void *
tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/OfferRegistry:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/OfferRegistry:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/OfferRegistry:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/OfferRegistry:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry;


/*
 * Stub interface for class OfferRegistry
 */

tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub::~OfferRegistry_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/OfferRegistry:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/OfferRegistry:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub_clp::OfferRegistry_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub_clp::OfferRegistry_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub_clp::~OfferRegistry_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub::registerService( tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr _par_service_ref, const tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& _par_properties )
{
  CORBA::StaticAny _sa_service_ref( _marshaller_scs_core_IComponent, &_par_service_ref );
  CORBA::StaticAny _sa_properties( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty, &_par_properties );
  tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr _res = tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_nil();
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer, &_res );

  CORBA::StaticRequest __req( this, "registerService" );
  __req.add_in_arg( &_sa_service_ref );
  __req.add_in_arg( &_sa_properties );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidService:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidProperties:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/UnauthorizedFacets:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub_clp::registerService( tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr _par_service_ref, const tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& _par_properties )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->registerService(_par_service_ref, _par_properties);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub::registerService(_par_service_ref, _par_properties);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub::findServices( const tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& _par_properties )
{
  CORBA::StaticAny _sa_properties( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty, &_par_properties );
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc );

  CORBA::StaticRequest __req( this, "findServices" );
  __req.add_in_arg( &_sa_properties );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq*
tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub_clp::findServices( const tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq& _par_properties )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->findServices(_par_properties);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub::findServices(_par_properties);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub::getServices()
{
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc );

  CORBA::StaticRequest __req( this, "getServices" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq*
tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub_clp::getServices()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getServices();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub::getServices();
}

#endif // MICO_CONF_NO_POA

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc::RegisteredEntityDesc()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc::RegisteredEntityDesc( const RegisteredEntityDesc& _s )
{
  category = ((RegisteredEntityDesc&)_s).category;
  id = ((RegisteredEntityDesc&)_s).id;
  name = ((RegisteredEntityDesc&)_s).name;
  ref = ((RegisteredEntityDesc&)_s).ref;
}

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc::~RegisteredEntityDesc()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc&
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc::operator=( const RegisteredEntityDesc& _s )
{
  category = ((RegisteredEntityDesc&)_s).category;
  id = ((RegisteredEntityDesc&)_s).id;
  name = ((RegisteredEntityDesc&)_s).name;
  ref = ((RegisteredEntityDesc&)_s).ref;
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory->demarshal( dc, &((_MICO_T*)v)->category._for_demarshal() ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->id._for_demarshal() ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->name._for_demarshal() ) &&
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity->demarshal( dc, &((_MICO_T*)v)->ref._for_demarshal() ) &&
    dc.struct_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory->marshal( ec, &((_MICO_T*)v)->category.inout() );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->id.inout() );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->name.inout() );
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity->marshal( ec, &((_MICO_T*)v)->ref.inout() );
  ec.struct_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc;



#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::InvalidInterface()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::InvalidInterface( const InvalidInterface& _s )
{
  ifaceId = ((InvalidInterface&)_s).ifaceId;
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::~InvalidInterface()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface&
tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::operator=( const InvalidInterface& _s )
{
  ifaceId = ((InvalidInterface&)_s).ifaceId;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::InvalidInterface()
{
}

#endif

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::InvalidInterface( const char* _m0 )
{
  ifaceId = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->ifaceId._for_demarshal() ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidInterface:1.0" );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->ifaceId.inout() );
  ec.except_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface;

void tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw InvalidInterface_var( (tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidInterface:1.0";
}

void tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::_encode_any( CORBA::Any & ) const
{
  // use --any to make this work!
  assert(0);
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::_clone() const
{
  return new InvalidInterface( *this );
}

tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidInterface:1.0" ) )
    return (InvalidInterface *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface *tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidInterface:1.0" ) )
    return (InvalidInterface *) _ex;
  return NULL;
}


#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::AuthorizationInUse()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::AuthorizationInUse( const AuthorizationInUse& _s )
{
  offers = ((AuthorizationInUse&)_s).offers;
}

tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::~AuthorizationInUse()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse&
tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::operator=( const AuthorizationInUse& _s )
{
  offers = ((AuthorizationInUse&)_s).offers;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::AuthorizationInUse()
{
}

#endif

tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::AuthorizationInUse( const tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq& _m0 )
{
  offers = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc->demarshal( dc, &((_MICO_T*)v)->offers ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/AuthorizationInUse:1.0" );
  _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc->marshal( ec, &((_MICO_T*)v)->offers );
  ec.except_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse;

void tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw AuthorizationInUse_var( (tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/AuthorizationInUse:1.0";
}

void tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::_encode_any( CORBA::Any & ) const
{
  // use --any to make this work!
  assert(0);
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::_clone() const
{
  return new AuthorizationInUse( *this );
}

tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse *tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/AuthorizationInUse:1.0" ) )
    return (AuthorizationInUse *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse *tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/AuthorizationInUse:1.0" ) )
    return (AuthorizationInUse *) _ex;
  return NULL;
}



/*
 * Base interface for class RegisteredEntity
 */

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::~RegisteredEntity()
{
}

void *
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/RegisteredEntity:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/RegisteredEntity:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/RegisteredEntity:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/RegisteredEntity:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity;


/*
 * Stub interface for class RegisteredEntity
 */

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::~RegisteredEntity_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/RegisteredEntity:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/RegisteredEntity:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::RegisteredEntity_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::RegisteredEntity_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::~RegisteredEntity_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::category()
{
  tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr _res = tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_nil();
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory, &_res );

  CORBA::StaticRequest __req( this, "_get_category" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::category()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->category();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::category();
}

#endif // MICO_CONF_NO_POA

char* tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::id()
{
  tecgraf::openbus::core::v2_00::Identifier _res = NULL;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "_get_id" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

char*
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::id()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::Identifier __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->id();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::id();
}

#endif // MICO_CONF_NO_POA

char* tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::name()
{
  char* _res = NULL;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "_get_name" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

char*
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::name()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (_serv);
    if (_myserv) {
      char* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->name();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::name();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc* tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::describe()
{
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc );

  CORBA::StaticRequest __req( this, "describe" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc*
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::describe()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->describe();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::describe();
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::setName( const char* _par_name )
{
  CORBA::StaticAny _sa_name( CORBA::_stc_string, &_par_name );
  CORBA::StaticRequest __req( this, "setName" );
  __req.add_in_arg( &_sa_name );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::setName( const char* _par_name )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->setName(_par_name);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return;
    }
    _postinvoke ();
  }

  tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::setName(_par_name);
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::remove()
{
  CORBA::StaticRequest __req( this, "remove" );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::remove()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->remove();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return;
    }
    _postinvoke ();
  }

  tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::remove();
}

#endif // MICO_CONF_NO_POA

CORBA::Boolean tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::grantInterface( const char* _par_ifaceId )
{
  CORBA::StaticAny _sa_ifaceId( CORBA::_stc_string, &_par_ifaceId );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "grantInterface" );
  __req.add_in_arg( &_sa_ifaceId );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidInterface:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

CORBA::Boolean
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::grantInterface( const char* _par_ifaceId )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (_serv);
    if (_myserv) {
      CORBA::Boolean __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->grantInterface(_par_ifaceId);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::grantInterface(_par_ifaceId);
}

#endif // MICO_CONF_NO_POA

CORBA::Boolean tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::revokeInterface( const char* _par_ifaceId )
{
  CORBA::StaticAny _sa_ifaceId( CORBA::_stc_string, &_par_ifaceId );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "revokeInterface" );
  __req.add_in_arg( &_sa_ifaceId );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidInterface:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/AuthorizationInUse:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

CORBA::Boolean
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::revokeInterface( const char* _par_ifaceId )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (_serv);
    if (_myserv) {
      CORBA::Boolean __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->revokeInterface(_par_ifaceId);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::revokeInterface(_par_ifaceId);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::getGrantedInterfaces()
{
  CORBA::StaticAny __res( CORBA::_stcseq_string );

  CORBA::StaticRequest __req( this, "getGrantedInterfaces" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq*
tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp::getGrantedInterfaces()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getGrantedInterfaces();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub::getGrantedInterfaces();
}

#endif // MICO_CONF_NO_POA

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc::EntityCategoryDesc()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc::EntityCategoryDesc( const EntityCategoryDesc& _s )
{
  id = ((EntityCategoryDesc&)_s).id;
  name = ((EntityCategoryDesc&)_s).name;
  ref = ((EntityCategoryDesc&)_s).ref;
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc::~EntityCategoryDesc()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc&
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc::operator=( const EntityCategoryDesc& _s )
{
  id = ((EntityCategoryDesc&)_s).id;
  name = ((EntityCategoryDesc&)_s).name;
  ref = ((EntityCategoryDesc&)_s).ref;
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->id._for_demarshal() ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->name._for_demarshal() ) &&
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory->demarshal( dc, &((_MICO_T*)v)->ref._for_demarshal() ) &&
    dc.struct_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->id.inout() );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->name.inout() );
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory->marshal( ec, &((_MICO_T*)v)->ref.inout() );
  ec.struct_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc;



#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::EntityCategoryInUse()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::EntityCategoryInUse( const EntityCategoryInUse& _s )
{
  entities = ((EntityCategoryInUse&)_s).entities;
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::~EntityCategoryInUse()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse&
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::operator=( const EntityCategoryInUse& _s )
{
  entities = ((EntityCategoryInUse&)_s).entities;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::EntityCategoryInUse()
{
}

#endif

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::EntityCategoryInUse( const tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq& _m0 )
{
  entities = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc->demarshal( dc, &((_MICO_T*)v)->entities ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategoryInUse:1.0" );
  _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc->marshal( ec, &((_MICO_T*)v)->entities );
  ec.except_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse;

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw EntityCategoryInUse_var( (tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategoryInUse:1.0";
}

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::_encode_any( CORBA::Any & ) const
{
  // use --any to make this work!
  assert(0);
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::_clone() const
{
  return new EntityCategoryInUse( *this );
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse *tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategoryInUse:1.0" ) )
    return (EntityCategoryInUse *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse *tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategoryInUse:1.0" ) )
    return (EntityCategoryInUse *) _ex;
  return NULL;
}


#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::EntityAlreadyRegistered()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::EntityAlreadyRegistered( const EntityAlreadyRegistered& _s )
{
  existing = ((EntityAlreadyRegistered&)_s).existing;
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::~EntityAlreadyRegistered()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered&
tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::operator=( const EntityAlreadyRegistered& _s )
{
  existing = ((EntityAlreadyRegistered&)_s).existing;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::EntityAlreadyRegistered()
{
}

#endif

tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::EntityAlreadyRegistered( const tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc& _m0 )
{
  existing = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc->demarshal( dc, &((_MICO_T*)v)->existing ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityAlreadyRegistered:1.0" );
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc->marshal( ec, &((_MICO_T*)v)->existing );
  ec.except_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered;

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw EntityAlreadyRegistered_var( (tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityAlreadyRegistered:1.0";
}

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::_encode_any( CORBA::Any & ) const
{
  // use --any to make this work!
  assert(0);
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::_clone() const
{
  return new EntityAlreadyRegistered( *this );
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered *tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityAlreadyRegistered:1.0" ) )
    return (EntityAlreadyRegistered *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered *tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityAlreadyRegistered:1.0" ) )
    return (EntityAlreadyRegistered *) _ex;
  return NULL;
}


/*
 * Base interface for class EntityCategory
 */

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::~EntityCategory()
{
}

void *
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategory:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategory:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategory:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategory:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory;


/*
 * Stub interface for class EntityCategory
 */

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::~EntityCategory_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategory:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategory:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::EntityCategory_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::EntityCategory_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::~EntityCategory_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

char* tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::id()
{
  tecgraf::openbus::core::v2_00::Identifier _res = NULL;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "_get_id" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

char*
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::id()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::Identifier __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->id();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::id();
}

#endif // MICO_CONF_NO_POA

char* tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::name()
{
  char* _res = NULL;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "_get_name" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

char*
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::name()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow (_serv);
    if (_myserv) {
      char* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->name();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::name();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc* tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::describe()
{
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc );

  CORBA::StaticRequest __req( this, "describe" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc*
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::describe()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->describe();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::describe();
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::setName( const char* _par_name )
{
  CORBA::StaticAny _sa_name( CORBA::_stc_string, &_par_name );
  CORBA::StaticRequest __req( this, "setName" );
  __req.add_in_arg( &_sa_name );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::setName( const char* _par_name )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->setName(_par_name);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return;
    }
    _postinvoke ();
  }

  tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::setName(_par_name);
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::remove()
{
  CORBA::StaticRequest __req( this, "remove" );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategoryInUse:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::remove()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->remove();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return;
    }
    _postinvoke ();
  }

  tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::remove();
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::removeAll()
{
  CORBA::StaticRequest __req( this, "removeAll" );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::removeAll()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->removeAll();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return;
    }
    _postinvoke ();
  }

  tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::removeAll();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::registerEntity( const char* _par_id, const char* _par_name )
{
  CORBA::StaticAny _sa_id( CORBA::_stc_string, &_par_id );
  CORBA::StaticAny _sa_name( CORBA::_stc_string, &_par_name );
  tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr _res = tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_nil();
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity, &_res );

  CORBA::StaticRequest __req( this, "registerEntity" );
  __req.add_in_arg( &_sa_id );
  __req.add_in_arg( &_sa_name );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityAlreadyRegistered:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::registerEntity( const char* _par_id, const char* _par_name )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->registerEntity(_par_id, _par_name);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::registerEntity(_par_id, _par_name);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::getEntities()
{
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc );

  CORBA::StaticRequest __req( this, "getEntities" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq*
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp::getEntities()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getEntities();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub::getEntities();
}

#endif // MICO_CONF_NO_POA



#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::EntityCategoryAlreadyExists()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::EntityCategoryAlreadyExists( const EntityCategoryAlreadyExists& _s )
{
  existing = ((EntityCategoryAlreadyExists&)_s).existing;
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::~EntityCategoryAlreadyExists()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists&
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::operator=( const EntityCategoryAlreadyExists& _s )
{
  existing = ((EntityCategoryAlreadyExists&)_s).existing;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::EntityCategoryAlreadyExists()
{
}

#endif

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::EntityCategoryAlreadyExists( const tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc& _m0 )
{
  existing = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc->demarshal( dc, &((_MICO_T*)v)->existing ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategoryAlreadyExists:1.0" );
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc->marshal( ec, &((_MICO_T*)v)->existing );
  ec.except_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists;

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw EntityCategoryAlreadyExists_var( (tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategoryAlreadyExists:1.0";
}

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::_encode_any( CORBA::Any & ) const
{
  // use --any to make this work!
  assert(0);
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::_clone() const
{
  return new EntityCategoryAlreadyExists( *this );
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists *tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategoryAlreadyExists:1.0" ) )
    return (EntityCategoryAlreadyExists *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists *tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategoryAlreadyExists:1.0" ) )
    return (EntityCategoryAlreadyExists *) _ex;
  return NULL;
}


/*
 * Base interface for class EntityRegistry
 */

tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::~EntityRegistry()
{
}

void *
tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityRegistry:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityRegistry:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityRegistry:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityRegistry:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry;


/*
 * Stub interface for class EntityRegistry
 */

tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::~EntityRegistry_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityRegistry:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityRegistry:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp::EntityRegistry_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp::EntityRegistry_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp::~EntityRegistry_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::createEntityCategory( const char* _par_id, const char* _par_name )
{
  CORBA::StaticAny _sa_id( CORBA::_stc_string, &_par_id );
  CORBA::StaticAny _sa_name( CORBA::_stc_string, &_par_name );
  tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr _res = tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_nil();
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory, &_res );

  CORBA::StaticRequest __req( this, "createEntityCategory" );
  __req.add_in_arg( &_sa_id );
  __req.add_in_arg( &_sa_name );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategoryAlreadyExists:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp::createEntityCategory( const char* _par_id, const char* _par_name )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->createEntityCategory(_par_id, _par_name);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::createEntityCategory(_par_id, _par_name);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDescSeq* tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getEntityCategories()
{
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc );

  CORBA::StaticRequest __req( this, "getEntityCategories" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDescSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDescSeq*
tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp::getEntityCategories()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDescSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getEntityCategories();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getEntityCategories();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getEntityCategory( const char* _par_id )
{
  CORBA::StaticAny _sa_id( CORBA::_stc_string, &_par_id );
  tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr _res = tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_nil();
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory, &_res );

  CORBA::StaticRequest __req( this, "getEntityCategory" );
  __req.add_in_arg( &_sa_id );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp::getEntityCategory( const char* _par_id )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getEntityCategory(_par_id);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getEntityCategory(_par_id);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getEntities()
{
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc );

  CORBA::StaticRequest __req( this, "getEntities" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq*
tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp::getEntities()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getEntities();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getEntities();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getEntity( const char* _par_id )
{
  CORBA::StaticAny _sa_id( CORBA::_stc_string, &_par_id );
  tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr _res = tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_nil();
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity, &_res );

  CORBA::StaticRequest __req( this, "getEntity" );
  __req.add_in_arg( &_sa_id );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp::getEntity( const char* _par_id )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getEntity(_par_id);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getEntity(_par_id);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getAuthorizedEntities()
{
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc );

  CORBA::StaticRequest __req( this, "getAuthorizedEntities" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq*
tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp::getAuthorizedEntities()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getAuthorizedEntities();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getAuthorizedEntities();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getEntitiesByAuthorizedInterfaces( const tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq& _par_interfaces )
{
  CORBA::StaticAny _sa_interfaces( CORBA::_stcseq_string, &_par_interfaces );
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc );

  CORBA::StaticRequest __req( this, "getEntitiesByAuthorizedInterfaces" );
  __req.add_in_arg( &_sa_interfaces );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq*
tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp::getEntitiesByAuthorizedInterfaces( const tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq& _par_interfaces )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getEntitiesByAuthorizedInterfaces(_par_interfaces);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub::getEntitiesByAuthorizedInterfaces(_par_interfaces);
}

#endif // MICO_CONF_NO_POA


#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::InterfaceInUse()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::InterfaceInUse( const InterfaceInUse& _s )
{
  entities = ((InterfaceInUse&)_s).entities;
}

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::~InterfaceInUse()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse&
tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::operator=( const InterfaceInUse& _s )
{
  entities = ((InterfaceInUse&)_s).entities;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::InterfaceInUse()
{
}

#endif

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::InterfaceInUse( const tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq& _m0 )
{
  entities = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc->demarshal( dc, &((_MICO_T*)v)->entities ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceInUse:1.0" );
  _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc->marshal( ec, &((_MICO_T*)v)->entities );
  ec.except_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse;

void tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw InterfaceInUse_var( (tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceInUse:1.0";
}

void tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::_encode_any( CORBA::Any & ) const
{
  // use --any to make this work!
  assert(0);
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::_clone() const
{
  return new InterfaceInUse( *this );
}

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse *tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceInUse:1.0" ) )
    return (InterfaceInUse *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse *tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceInUse:1.0" ) )
    return (InterfaceInUse *) _ex;
  return NULL;
}


/*
 * Base interface for class InterfaceRegistry
 */

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::~InterfaceRegistry()
{
}

void *
tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceRegistry:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceRegistry:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceRegistry:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceRegistry:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_ptr
tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry::~_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry;


/*
 * Stub interface for class InterfaceRegistry
 */

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub::~InterfaceRegistry_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceRegistry:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry *
POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceRegistry:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub_clp::InterfaceRegistry_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub_clp::InterfaceRegistry_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub_clp::~InterfaceRegistry_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

CORBA::Boolean tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub::registerInterface( const char* _par_ifaceId )
{
  CORBA::StaticAny _sa_ifaceId( CORBA::_stc_string, &_par_ifaceId );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "registerInterface" );
  __req.add_in_arg( &_sa_ifaceId );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InvalidInterface:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

CORBA::Boolean
tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub_clp::registerInterface( const char* _par_ifaceId )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_narrow (_serv);
    if (_myserv) {
      CORBA::Boolean __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->registerInterface(_par_ifaceId);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub::registerInterface(_par_ifaceId);
}

#endif // MICO_CONF_NO_POA

CORBA::Boolean tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub::removeInterface( const char* _par_ifaceId )
{
  CORBA::StaticAny _sa_ifaceId( CORBA::_stc_string, &_par_ifaceId );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "removeInterface" );
  __req.add_in_arg( &_sa_ifaceId );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceInUse:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

CORBA::Boolean
tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub_clp::removeInterface( const char* _par_ifaceId )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_narrow (_serv);
    if (_myserv) {
      CORBA::Boolean __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->removeInterface(_par_ifaceId);
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub::removeInterface(_par_ifaceId);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub::getInterfaces()
{
  CORBA::StaticAny __res( CORBA::_stcseq_string );

  CORBA::StaticRequest __req( this, "getInterfaces" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq*
tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub_clp::getInterfaces()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getInterfaces();
      #ifdef HAVE_EXCEPTIONS
      }
      catch (...) {
        _myserv->_remove_ref();
        _postinvoke();
        throw;
      }
      #endif

      _myserv->_remove_ref();
      _postinvoke ();
      return __res;
    }
    _postinvoke ();
  }

  return tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub::getInterfaces();
}

#endif // MICO_CONF_NO_POA

class _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty : public ::CORBA::StaticTypeInfo {
    typedef SequenceTmpl< tecgraf::openbus::core::v2_00::services::offer_registry::ServiceProperty,MICO_TID_DEF> _MICO_T;
  public:
    ~_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::~_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty()
{
}

::CORBA::StaticValueType _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::ULong len;
  if( !dc.seq_begin( len ) )
    return FALSE;
  ((_MICO_T *) v)->length( len );
  for( ::CORBA::ULong i = 0; i < len; i++ ) {
    if( !_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty->demarshal( dc, &(*(_MICO_T*)v)[i] ) )
      return FALSE;
  }
  return dc.seq_end();
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::ULong len = ((_MICO_T *) v)->length();
  ec.seq_begin( len );
  for( ::CORBA::ULong i = 0; i < len; i++ )
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty->marshal( ec, &(*(_MICO_T*)v)[i] );
  ec.seq_end();
}

::CORBA::StaticTypeInfo *_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty;

class _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc : public ::CORBA::StaticTypeInfo {
    typedef SequenceTmpl< tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc,MICO_TID_DEF> _MICO_T;
  public:
    ~_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::~_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc()
{
}

::CORBA::StaticValueType _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::ULong len;
  if( !dc.seq_begin( len ) )
    return FALSE;
  ((_MICO_T *) v)->length( len );
  for( ::CORBA::ULong i = 0; i < len; i++ ) {
    if( !_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc->demarshal( dc, &(*(_MICO_T*)v)[i] ) )
      return FALSE;
  }
  return dc.seq_end();
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::ULong len = ((_MICO_T *) v)->length();
  ec.seq_begin( len );
  for( ::CORBA::ULong i = 0; i < len; i++ )
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc->marshal( ec, &(*(_MICO_T*)v)[i] );
  ec.seq_end();
}

::CORBA::StaticTypeInfo *_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc;

class _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc : public ::CORBA::StaticTypeInfo {
    typedef SequenceTmpl< tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc,MICO_TID_DEF> _MICO_T;
  public:
    ~_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::~_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc()
{
}

::CORBA::StaticValueType _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::ULong len;
  if( !dc.seq_begin( len ) )
    return FALSE;
  ((_MICO_T *) v)->length( len );
  for( ::CORBA::ULong i = 0; i < len; i++ ) {
    if( !_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc->demarshal( dc, &(*(_MICO_T*)v)[i] ) )
      return FALSE;
  }
  return dc.seq_end();
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::ULong len = ((_MICO_T *) v)->length();
  ec.seq_begin( len );
  for( ::CORBA::ULong i = 0; i < len; i++ )
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc->marshal( ec, &(*(_MICO_T*)v)[i] );
  ec.seq_end();
}

::CORBA::StaticTypeInfo *_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc;

class _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc : public ::CORBA::StaticTypeInfo {
    typedef SequenceTmpl< tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc,MICO_TID_DEF> _MICO_T;
  public:
    ~_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::~_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc()
{
}

::CORBA::StaticValueType _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::ULong len;
  if( !dc.seq_begin( len ) )
    return FALSE;
  ((_MICO_T *) v)->length( len );
  for( ::CORBA::ULong i = 0; i < len; i++ ) {
    if( !_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc->demarshal( dc, &(*(_MICO_T*)v)[i] ) )
      return FALSE;
  }
  return dc.seq_end();
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::ULong len = ((_MICO_T *) v)->length();
  ec.seq_begin( len );
  for( ::CORBA::ULong i = 0; i < len; i++ )
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc->marshal( ec, &(*(_MICO_T*)v)[i] );
  ec.seq_end();
}

::CORBA::StaticTypeInfo *_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc;

struct __tc_init_OFFER_REGISTRY {
  __tc_init_OFFER_REGISTRY()
  {
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse;
    _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry = new _Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry;
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty = new _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty;
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc = new _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc;
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc = new _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc;
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc = new _Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc;
  }

  ~__tc_init_OFFER_REGISTRY()
  {
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidProperties);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_UnauthorizedFacets);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidService);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_OfferRegistry);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InvalidInterface);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_AuthorizationInUse);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryInUse);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityAlreadyRegistered);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryAlreadyExists);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityRegistry);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceInUse);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry*>(_marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_InterfaceRegistry);
    delete static_cast<_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty*>(_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty);
    delete static_cast<_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc*>(_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc);
    delete static_cast<_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc*>(_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc);
    delete static_cast<_Marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc*>(_marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc);
  }
};

static __tc_init_OFFER_REGISTRY __init_OFFER_REGISTRY;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer
POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::~ServiceOffer()
{
}

::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/ServiceOffer:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/ServiceOffer:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/ServiceOffer:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    switch (mico_string_hash (__req->op_name(), 7)) {
    case 0:
      if( strcmp( __req->op_name(), "_get_service_ref" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_ptr _res;
        CORBA::StaticAny __res( _marshaller_scs_core_IComponent, &_res );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = service_ref();
        __req->write_results();
        CORBA::release( _res );
        return true;
      }
      break;
    case 1:
      if( strcmp( __req->op_name(), "remove" ) == 0 ) {

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          remove();
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        return true;
      }
      break;
    case 2:
      if( strcmp( __req->op_name(), "describe" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDesc* _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = describe();
        __res.value( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc, _res );
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    case 4:
      if( strcmp( __req->op_name(), "setProperties" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq _par_properties;
        CORBA::StaticAny _sa_properties( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty, &_par_properties );

        __req->add_in_arg( &_sa_properties );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          setProperties( _par_properties );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        return true;
      }
      break;
    case 5:
      if( strcmp( __req->op_name(), "_get_properties" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq* _res;
        CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = properties();
        __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty, _res );
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_catch &_ex ) {
    __req->set_exception( _ex->_clone() );
    __req->write_results();
    return true;
  } catch( ... ) {
    CORBA::UNKNOWN _ex (CORBA::OMGVMCID | 1, CORBA::COMPLETED_MAYBE);
    __req->set_exception (_ex->_clone());
    __req->write_results ();
    return true;
  }
  #endif

  return false;
}

void
POA_tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}


// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry
POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::~OfferRegistry()
{
}

::tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/OfferRegistry:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/OfferRegistry:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/OfferRegistry:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( __req->op_name(), "registerService" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::services::offer_registry::OfferedService_var _par_service_ref;
      CORBA::StaticAny _sa_service_ref( _marshaller_scs_core_IComponent, &_par_service_ref._for_demarshal() );
      ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq _par_properties;
      CORBA::StaticAny _sa_properties( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty, &_par_properties );

      ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOffer_ptr _res;
      CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOffer, &_res );
      __req->add_in_arg( &_sa_service_ref );
      __req->add_in_arg( &_sa_properties );
      __req->set_result( &__res );

      if( !__req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _res = registerService( _par_service_ref.inout(), _par_properties );
      #ifdef HAVE_EXCEPTIONS
      } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::InvalidService_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::InvalidProperties_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::UnauthorizedFacets_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      }
      #endif
      __req->write_results();
      CORBA::release( _res );
      return true;
    }
    if( strcmp( __req->op_name(), "findServices" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::services::offer_registry::ServicePropertySeq _par_properties;
      CORBA::StaticAny _sa_properties( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceProperty, &_par_properties );

      ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* _res;
      CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc );
      __req->add_in_arg( &_sa_properties );
      __req->set_result( &__res );

      if( !__req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _res = findServices( _par_properties );
        __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc, _res );
      #ifdef HAVE_EXCEPTIONS
      } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      }
      #endif
      __req->write_results();
      delete _res;
      return true;
    }
    if( strcmp( __req->op_name(), "getServices" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::services::offer_registry::ServiceOfferDescSeq* _res;
      CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc );
      __req->set_result( &__res );

      if( !__req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _res = getServices();
        __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_ServiceOfferDesc, _res );
      #ifdef HAVE_EXCEPTIONS
      } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      }
      #endif
      __req->write_results();
      delete _res;
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_catch &_ex ) {
    __req->set_exception( _ex->_clone() );
    __req->write_results();
    return true;
  } catch( ... ) {
    CORBA::UNKNOWN _ex (CORBA::OMGVMCID | 1, CORBA::COMPLETED_MAYBE);
    __req->set_exception (_ex->_clone());
    __req->write_results ();
    return true;
  }
  #endif

  return false;
}

void
POA_tecgraf::openbus::core::v2_00::services::offer_registry::OfferRegistry::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}


// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity
POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::~RegisteredEntity()
{
}

::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/RegisteredEntity:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/RegisteredEntity:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/RegisteredEntity:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    switch (mico_string_hash (__req->op_name(), 13)) {
    case 0:
      if( strcmp( __req->op_name(), "getGrantedInterfaces" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* _res;
        CORBA::StaticAny __res( CORBA::_stcseq_string );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getGrantedInterfaces();
          __res.value( CORBA::_stcseq_string, _res );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    case 1:
      if( strcmp( __req->op_name(), "grantInterface" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceId_var _par_ifaceId;
        CORBA::StaticAny _sa_ifaceId( CORBA::_stc_string, &_par_ifaceId._for_demarshal() );

        CORBA::Boolean _res;
        CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
        __req->add_in_arg( &_sa_ifaceId );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = grantInterface( _par_ifaceId.inout() );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        return true;
      }
      if( strcmp( __req->op_name(), "_get_name" ) == 0 ) {
        char* _res;
        CORBA::StaticAny __res( CORBA::_stc_string, &_res );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = name();
        __req->write_results();
        CORBA::string_free( _res );
        return true;
      }
      break;
    case 2:
      if( strcmp( __req->op_name(), "describe" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDesc* _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = describe();
        __res.value( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc, _res );
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    case 3:
      if( strcmp( __req->op_name(), "setName" ) == 0 ) {
        CORBA::String_var _par_name;
        CORBA::StaticAny _sa_name( CORBA::_stc_string, &_par_name._for_demarshal() );

        __req->add_in_arg( &_sa_name );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          setName( _par_name.inout() );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        return true;
      }
      if( strcmp( __req->op_name(), "_get_category" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory, &_res );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = category();
        __req->write_results();
        CORBA::release( _res );
        return true;
      }
      break;
    case 6:
      if( strcmp( __req->op_name(), "remove" ) == 0 ) {

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          remove();
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        return true;
      }
      if( strcmp( __req->op_name(), "revokeInterface" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceId_var _par_ifaceId;
        CORBA::StaticAny _sa_ifaceId( CORBA::_stc_string, &_par_ifaceId._for_demarshal() );

        CORBA::Boolean _res;
        CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
        __req->add_in_arg( &_sa_ifaceId );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = revokeInterface( _par_ifaceId.inout() );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::AuthorizationInUse_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        return true;
      }
      break;
    case 10:
      if( strcmp( __req->op_name(), "_get_id" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier _res;
        CORBA::StaticAny __res( CORBA::_stc_string, &_res );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = id();
        __req->write_results();
        CORBA::string_free( _res );
        return true;
      }
      break;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_catch &_ex ) {
    __req->set_exception( _ex->_clone() );
    __req->write_results();
    return true;
  } catch( ... ) {
    CORBA::UNKNOWN _ex (CORBA::OMGVMCID | 1, CORBA::COMPLETED_MAYBE);
    __req->set_exception (_ex->_clone());
    __req->write_results ();
    return true;
  }
  #endif

  return false;
}

void
POA_tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}


// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::~EntityCategory()
{
}

::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategory:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategory:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityCategory:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    switch (mico_string_hash (__req->op_name(), 13)) {
    case 1:
      if( strcmp( __req->op_name(), "_get_name" ) == 0 ) {
        char* _res;
        CORBA::StaticAny __res( CORBA::_stc_string, &_res );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = name();
        __req->write_results();
        CORBA::string_free( _res );
        return true;
      }
      break;
    case 2:
      if( strcmp( __req->op_name(), "describe" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDesc* _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = describe();
        __res.value( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc, _res );
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    case 3:
      if( strcmp( __req->op_name(), "setName" ) == 0 ) {
        CORBA::String_var _par_name;
        CORBA::StaticAny _sa_name( CORBA::_stc_string, &_par_name._for_demarshal() );

        __req->add_in_arg( &_sa_name );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          setName( _par_name.inout() );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        return true;
      }
      break;
    case 6:
      if( strcmp( __req->op_name(), "remove" ) == 0 ) {

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          remove();
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryInUse_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        return true;
      }
      break;
    case 7:
      if( strcmp( __req->op_name(), "removeAll" ) == 0 ) {

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          removeAll();
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        return true;
      }
      if( strcmp( __req->op_name(), "registerEntity" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_id;
        CORBA::StaticAny _sa_id( CORBA::_stc_string, &_par_id._for_demarshal() );
        CORBA::String_var _par_name;
        CORBA::StaticAny _sa_name( CORBA::_stc_string, &_par_name._for_demarshal() );

        ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity, &_res );
        __req->add_in_arg( &_sa_id );
        __req->add_in_arg( &_sa_name );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = registerEntity( _par_id.inout(), _par_name.inout() );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityAlreadyRegistered_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        CORBA::release( _res );
        return true;
      }
      break;
    case 10:
      if( strcmp( __req->op_name(), "_get_id" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier _res;
        CORBA::StaticAny __res( CORBA::_stc_string, &_res );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = id();
        __req->write_results();
        CORBA::string_free( _res );
        return true;
      }
      break;
    case 12:
      if( strcmp( __req->op_name(), "getEntities" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* _res;
        CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getEntities();
          __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc, _res );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_catch &_ex ) {
    __req->set_exception( _ex->_clone() );
    __req->write_results();
    return true;
  } catch( ... ) {
    CORBA::UNKNOWN _ex (CORBA::OMGVMCID | 1, CORBA::COMPLETED_MAYBE);
    __req->set_exception (_ex->_clone());
    __req->write_results ();
    return true;
  }
  #endif

  return false;
}

void
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}


// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::~EntityRegistry()
{
}

::tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityRegistry:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityRegistry:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/EntityRegistry:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    switch (mico_string_hash (__req->op_name(), 11)) {
    case 0:
      if( strcmp( __req->op_name(), "createEntityCategory" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_id;
        CORBA::StaticAny _sa_id( CORBA::_stc_string, &_par_id._for_demarshal() );
        CORBA::String_var _par_name;
        CORBA::StaticAny _sa_name( CORBA::_stc_string, &_par_name._for_demarshal() );

        ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory, &_res );
        __req->add_in_arg( &_sa_id );
        __req->add_in_arg( &_sa_name );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = createEntityCategory( _par_id.inout(), _par_name.inout() );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryAlreadyExists_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        CORBA::release( _res );
        return true;
      }
      break;
    case 1:
      if( strcmp( __req->op_name(), "getEntity" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_id;
        CORBA::StaticAny _sa_id( CORBA::_stc_string, &_par_id._for_demarshal() );

        ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntity_ptr _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntity, &_res );
        __req->add_in_arg( &_sa_id );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getEntity( _par_id.inout() );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        CORBA::release( _res );
        return true;
      }
      break;
    case 2:
      if( strcmp( __req->op_name(), "getAuthorizedEntities" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* _res;
        CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getAuthorizedEntities();
          __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc, _res );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    case 4:
      if( strcmp( __req->op_name(), "getEntitiesByAuthorizedInterfaces" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq _par_interfaces;
        CORBA::StaticAny _sa_interfaces( CORBA::_stcseq_string, &_par_interfaces );

        ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* _res;
        CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc );
        __req->add_in_arg( &_sa_interfaces );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getEntitiesByAuthorizedInterfaces( _par_interfaces );
          __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc, _res );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    case 5:
      if( strcmp( __req->op_name(), "getEntities" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::RegisteredEntityDescSeq* _res;
        CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getEntities();
          __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_RegisteredEntityDesc, _res );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    case 6:
      if( strcmp( __req->op_name(), "getEntityCategory" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_id;
        CORBA::StaticAny _sa_id( CORBA::_stc_string, &_par_id._for_demarshal() );

        ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategory_ptr _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategory, &_res );
        __req->add_in_arg( &_sa_id );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getEntityCategory( _par_id.inout() );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        CORBA::release( _res );
        return true;
      }
      break;
    case 8:
      if( strcmp( __req->op_name(), "getEntityCategories" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::offer_registry::EntityCategoryDescSeq* _res;
        CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getEntityCategories();
          __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_offer_registry_EntityCategoryDesc, _res );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        }
        #endif
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_catch &_ex ) {
    __req->set_exception( _ex->_clone() );
    __req->write_results();
    return true;
  } catch( ... ) {
    CORBA::UNKNOWN _ex (CORBA::OMGVMCID | 1, CORBA::COMPLETED_MAYBE);
    __req->set_exception (_ex->_clone());
    __req->write_results ();
    return true;
  }
  #endif

  return false;
}

void
POA_tecgraf::openbus::core::v2_00::services::offer_registry::EntityRegistry::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}


// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry
POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::~InterfaceRegistry()
{
}

::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceRegistry:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceRegistry:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/offer_registry/InterfaceRegistry:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( __req->op_name(), "registerInterface" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceId_var _par_ifaceId;
      CORBA::StaticAny _sa_ifaceId( CORBA::_stc_string, &_par_ifaceId._for_demarshal() );

      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      __req->add_in_arg( &_sa_ifaceId );
      __req->set_result( &__res );

      if( !__req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _res = registerInterface( _par_ifaceId.inout() );
      #ifdef HAVE_EXCEPTIONS
      } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::InvalidInterface_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      }
      #endif
      __req->write_results();
      return true;
    }
    if( strcmp( __req->op_name(), "removeInterface" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceId_var _par_ifaceId;
      CORBA::StaticAny _sa_ifaceId( CORBA::_stc_string, &_par_ifaceId._for_demarshal() );

      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      __req->add_in_arg( &_sa_ifaceId );
      __req->set_result( &__res );

      if( !__req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _res = removeInterface( _par_ifaceId.inout() );
      #ifdef HAVE_EXCEPTIONS
      } catch( ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceInUse_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      }
      #endif
      __req->write_results();
      return true;
    }
    if( strcmp( __req->op_name(), "getInterfaces" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceIdSeq* _res;
      CORBA::StaticAny __res( CORBA::_stcseq_string );
      __req->set_result( &__res );

      if( !__req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _res = getInterfaces();
        __res.value( CORBA::_stcseq_string, _res );
      #ifdef HAVE_EXCEPTIONS
      } catch( ::tecgraf::openbus::core::v2_00::services::ServiceFailure_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      }
      #endif
      __req->write_results();
      delete _res;
      return true;
    }
  #ifdef HAVE_EXCEPTIONS
  } catch( CORBA::SystemException_catch &_ex ) {
    __req->set_exception( _ex->_clone() );
    __req->write_results();
    return true;
  } catch( ... ) {
    CORBA::UNKNOWN _ex (CORBA::OMGVMCID | 1, CORBA::COMPLETED_MAYBE);
    __req->set_exception (_ex->_clone());
    __req->write_results ();
    return true;
  }
  #endif

  return false;
}

void
POA_tecgraf::openbus::core::v2_00::services::offer_registry::InterfaceRegistry::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}

