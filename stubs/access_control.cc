/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2006 by The Mico Team
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <access_control.h>


using namespace std;

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------
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
CORBA::TypeCodeConst _tc_EntityCertificate;
}
}
}
}
}
}

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
CORBA::TypeCodeConst _tc_MissingCertificate;
}
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::MissingCertificate()
{
}

tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::MissingCertificate( const MissingCertificate& _s )
{
  entity = ((MissingCertificate&)_s).entity;
}

tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::~MissingCertificate()
{
}

tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate&
tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::operator=( const MissingCertificate& _s )
{
  entity = ((MissingCertificate&)_s).entity;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::MissingCertificate()
{
}

#endif

tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::MissingCertificate( const char* _m0 )
{
  entity = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->entity._for_demarshal() ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/access_control/MissingCertificate:1.0" );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->entity.inout() );
  ec.except_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_MissingCertificate;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate &_e )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate, &_e);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate &_e )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate, &_e);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate *&_e )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate, (void *&)_e);
}

void tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw MissingCertificate_var( (tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/access_control/MissingCertificate:1.0";
}

void tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::_encode_any( CORBA::Any &_a ) const
{
  _a <<= *this;
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::_clone() const
{
  return new MissingCertificate( *this );
}

tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate *tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/MissingCertificate:1.0" ) )
    return (MissingCertificate *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate *tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/MissingCertificate:1.0" ) )
    return (MissingCertificate *) _ex;
  return NULL;
}

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
CORBA::TypeCodeConst _tc_InvalidCertificate;
}
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::InvalidCertificate()
{
}

tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::InvalidCertificate( const InvalidCertificate& _s )
{
  message = ((InvalidCertificate&)_s).message;
}

tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::~InvalidCertificate()
{
}

tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate&
tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::operator=( const InvalidCertificate& _s )
{
  message = ((InvalidCertificate&)_s).message;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::InvalidCertificate()
{
}

#endif

tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::InvalidCertificate( const char* _m0 )
{
  message = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->message._for_demarshal() ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidCertificate:1.0" );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->message.inout() );
  ec.except_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_InvalidCertificate;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate &_e )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate, &_e);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate &_e )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate, &_e);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate *&_e )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate, (void *&)_e);
}

void tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw InvalidCertificate_var( (tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidCertificate:1.0";
}

void tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::_encode_any( CORBA::Any &_a ) const
{
  _a <<= *this;
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::_clone() const
{
  return new InvalidCertificate( *this );
}

tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate *tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidCertificate:1.0" ) )
    return (InvalidCertificate *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate *tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidCertificate:1.0" ) )
    return (InvalidCertificate *) _ex;
  return NULL;
}


/*
 * Base interface for class CertificateRegistry
 */

tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::~CertificateRegistry()
{
}

void *
tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/CertificateRegistry:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr
tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/access_control/CertificateRegistry:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/CertificateRegistry:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/access_control/CertificateRegistry:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr
tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

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
CORBA::TypeCodeConst _tc_CertificateRegistry;
}
}
}
}
}
}
class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_CertificateRegistry;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry;

void
operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr &_obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr *p;
  if (_a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}


/*
 * Stub interface for class CertificateRegistry
 */

tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub::~CertificateRegistry_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/CertificateRegistry:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry *
POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/access_control/CertificateRegistry:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub_clp::CertificateRegistry_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub_clp::CertificateRegistry_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub_clp::~CertificateRegistry_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub::registerCertificate( const char* _par_entity, const tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate& _par_cert )
{
  CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity );
  CORBA::StaticAny _sa_cert( CORBA::_stcseq_octet, &_par_cert );
  CORBA::StaticRequest __req( this, "registerCertificate" );
  __req.add_in_arg( &_sa_entity );
  __req.add_in_arg( &_sa_cert );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate, "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidCertificate:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub_clp::registerCertificate( const char* _par_entity, const tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate& _par_cert )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->registerCertificate(_par_entity, _par_cert);
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

  tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub::registerCertificate(_par_entity, _par_cert);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate* tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub::getCertificate( const char* _par_entity )
{
  CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity );
  CORBA::StaticAny __res( CORBA::_stcseq_octet );

  CORBA::StaticRequest __req( this, "getCertificate" );
  __req.add_in_arg( &_sa_entity );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate, "IDL:tecgraf/openbus/core/v2_00/services/access_control/MissingCertificate:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate*
tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub_clp::getCertificate( const char* _par_entity )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getCertificate(_par_entity);
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

  return tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub::getCertificate(_par_entity);
}

#endif // MICO_CONF_NO_POA

CORBA::Boolean tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub::removeCertificate( const char* _par_entity )
{
  CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "removeCertificate" );
  __req.add_in_arg( &_sa_entity );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

CORBA::Boolean
tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub_clp::removeCertificate( const char* _par_entity )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_narrow (_serv);
    if (_myserv) {
      CORBA::Boolean __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->removeCertificate(_par_entity);
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

  return tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub::removeCertificate(_par_entity);
}

#endif // MICO_CONF_NO_POA

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
CORBA::TypeCodeConst _tc_LoginAuthenticationInfo;
}
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo::LoginAuthenticationInfo()
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo::LoginAuthenticationInfo( const LoginAuthenticationInfo& _s )
{
  hash = ((LoginAuthenticationInfo&)_s).hash;
  data = ((LoginAuthenticationInfo&)_s).data;
}

tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo::~LoginAuthenticationInfo()
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo&
tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo::operator=( const LoginAuthenticationInfo& _s )
{
  hash = ((LoginAuthenticationInfo&)_s).hash;
  data = ((LoginAuthenticationInfo&)_s).data;
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    _marshaller__a32_octet->demarshal( dc, &((_MICO_T*)v)->hash ) &&
    CORBA::_stcseq_octet->demarshal( dc, &((_MICO_T*)v)->data ) &&
    dc.struct_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  _marshaller__a32_octet->marshal( ec, &((_MICO_T*)v)->hash );
  CORBA::_stcseq_octet->marshal( ec, &((_MICO_T*)v)->data );
  ec.struct_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginAuthenticationInfo;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::LoginAuthenticationInfo *&_s )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo, (void *&)_s);
}

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
CORBA::TypeCodeConst _tc_LoginInfo;
}
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::LoginInfo::LoginInfo()
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginInfo::LoginInfo( const LoginInfo& _s )
{
  id = ((LoginInfo&)_s).id;
  entity = ((LoginInfo&)_s).entity;
}

tecgraf::openbus::core::v2_00::services::access_control::LoginInfo::~LoginInfo()
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginInfo&
tecgraf::openbus::core::v2_00::services::access_control::LoginInfo::operator=( const LoginInfo& _s )
{
  id = ((LoginInfo&)_s).id;
  entity = ((LoginInfo&)_s).entity;
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::access_control::LoginInfo _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->id._for_demarshal() ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->entity._for_demarshal() ) &&
    dc.struct_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->id.inout() );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->entity.inout() );
  ec.struct_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginInfo;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::LoginInfo &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginInfo *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginInfo &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::LoginInfo *&_s )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, (void *&)_s);
}

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
CORBA::TypeCodeConst _tc_LoginInfoSeq;
}
}
}
}
}
}

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
CORBA::TypeCodeConst _tc_CallChain;
}
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::CallChain::CallChain()
{
}

tecgraf::openbus::core::v2_00::services::access_control::CallChain::CallChain( const CallChain& _s )
{
  target = ((CallChain&)_s).target;
  callers = ((CallChain&)_s).callers;
}

tecgraf::openbus::core::v2_00::services::access_control::CallChain::~CallChain()
{
}

tecgraf::openbus::core::v2_00::services::access_control::CallChain&
tecgraf::openbus::core::v2_00::services::access_control::CallChain::operator=( const CallChain& _s )
{
  target = ((CallChain&)_s).target;
  callers = ((CallChain&)_s).callers;
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::access_control::CallChain _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->target._for_demarshal() ) &&
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo->demarshal( dc, &((_MICO_T*)v)->callers ) &&
    dc.struct_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->target.inout() );
  _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo->marshal( ec, &((_MICO_T*)v)->callers );
  ec.struct_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_CallChain;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::CallChain &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::CallChain *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::CallChain &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::CallChain *&_s )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain, (void *&)_s);
}

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
CORBA::TypeCodeConst _tc_SignedCallChain;
}
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain::SignedCallChain()
{
}

tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain::SignedCallChain( const SignedCallChain& _s )
{
  signature = ((SignedCallChain&)_s).signature;
  encoded = ((SignedCallChain&)_s).encoded;
}

tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain::~SignedCallChain()
{
}

tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain&
tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain::operator=( const SignedCallChain& _s )
{
  signature = ((SignedCallChain&)_s).signature;
  encoded = ((SignedCallChain&)_s).encoded;
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    _marshaller__a256_octet->demarshal( dc, &((_MICO_T*)v)->signature ) &&
    CORBA::_stcseq_octet->demarshal( dc, &((_MICO_T*)v)->encoded ) &&
    dc.struct_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  _marshaller__a256_octet->marshal( ec, &((_MICO_T*)v)->signature );
  CORBA::_stcseq_octet->marshal( ec, &((_MICO_T*)v)->encoded );
  ec.struct_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_SignedCallChain;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain *&_s )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain, (void *&)_s);
}

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
CORBA::TypeCodeConst _tc_ValidityTime;
}
}
}
}
}
}

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
CORBA::TypeCodeConst _tc_ValidityTimeSeq;
}
}
}
}
}
}

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
CORBA::TypeCodeConst _tc_WrongEncoding;
}
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::WrongEncoding()
{
}

tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::WrongEncoding( const WrongEncoding& _s )
{
}

tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::~WrongEncoding()
{
}

tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding&
tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::operator=( const WrongEncoding& _s )
{
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/access_control/WrongEncoding:1.0" );
  ec.except_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_WrongEncoding;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding &_e )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding, &_e);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding &_e )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding, &_e);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding *&_e )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding, (void *&)_e);
}

void tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw WrongEncoding_var( (tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/access_control/WrongEncoding:1.0";
}

void tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::_encode_any( CORBA::Any &_a ) const
{
  _a <<= *this;
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::_clone() const
{
  return new WrongEncoding( *this );
}

tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding *tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/WrongEncoding:1.0" ) )
    return (WrongEncoding *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding *tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/WrongEncoding:1.0" ) )
    return (WrongEncoding *) _ex;
  return NULL;
}

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
CORBA::TypeCodeConst _tc_AccessDenied;
}
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::AccessDenied()
{
}

tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::AccessDenied( const AccessDenied& _s )
{
}

tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::~AccessDenied()
{
}

tecgraf::openbus::core::v2_00::services::access_control::AccessDenied&
tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::operator=( const AccessDenied& _s )
{
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::access_control::AccessDenied _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessDenied:1.0" );
  ec.except_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_AccessDenied;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::AccessDenied &_e )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied, &_e);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::AccessDenied *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::AccessDenied &_e )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied, &_e);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::AccessDenied *&_e )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied, (void *&)_e);
}

void tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw AccessDenied_var( (tecgraf::openbus::core::v2_00::services::access_control::AccessDenied*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessDenied:1.0";
}

void tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::_encode_any( CORBA::Any &_a ) const
{
  _a <<= *this;
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::_clone() const
{
  return new AccessDenied( *this );
}

tecgraf::openbus::core::v2_00::services::access_control::AccessDenied *tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessDenied:1.0" ) )
    return (AccessDenied *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::access_control::AccessDenied *tecgraf::openbus::core::v2_00::services::access_control::AccessDenied::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessDenied:1.0" ) )
    return (AccessDenied *) _ex;
  return NULL;
}


/*
 * Base interface for class LoginByCertificate
 */

tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::~LoginByCertificate()
{
}

void *
tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginByCertificate:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr
tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginByCertificate:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginByCertificate:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginByCertificate:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr
tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

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
CORBA::TypeCodeConst _tc_LoginByCertificate;
}
}
}
}
}
}
class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginByCertificate;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate;

void
operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr &_obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr *p;
  if (_a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}


/*
 * Stub interface for class LoginByCertificate
 */

tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub::~LoginByCertificate_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginByCertificate:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate *
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginByCertificate:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub_clp::LoginByCertificate_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub_clp::LoginByCertificate_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub_clp::~LoginByCertificate_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

char* tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub::login( const tecgraf::openbus::core::v2_00::OctetSeq& _par_pubkey, const tecgraf::openbus::core::v2_00::EncryptedBlock _par_encrypted, tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out _par_validity )
{
  CORBA::StaticAny _sa_pubkey( CORBA::_stcseq_octet, &_par_pubkey );
  CORBA::StaticAny _sa_encrypted( _marshaller__a256_octet, _par_encrypted );
  CORBA::StaticAny _sa_validity( CORBA::_stc_ulong, &_par_validity );
  tecgraf::openbus::core::v2_00::Identifier _res = NULL;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "login" );
  __req.add_in_arg( &_sa_pubkey );
  __req.add_in_arg( &_sa_encrypted );
  __req.add_out_arg( &_sa_validity );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding, "IDL:tecgraf/openbus/core/v2_00/services/access_control/WrongEncoding:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied, "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessDenied:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

char*
tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub_clp::login( const tecgraf::openbus::core::v2_00::OctetSeq& _par_pubkey, const tecgraf::openbus::core::v2_00::EncryptedBlock _par_encrypted, tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out _par_validity )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::Identifier __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->login(_par_pubkey, _par_encrypted, _par_validity);
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

  return tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub::login(_par_pubkey, _par_encrypted, _par_validity);
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub::cancel()
{
  CORBA::StaticRequest __req( this, "cancel" );

  __req.oneway();

  mico_sii_throw( &__req, 
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub_clp::cancel()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_narrow (_serv);
    if (_myserv) {
      _myserv->cancel();
      _myserv->_remove_ref();
      _postinvoke ();
      return;
    }
    _postinvoke ();
  }

  tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub::cancel();
}

#endif // MICO_CONF_NO_POA


/*
 * Base interface for class AccessControl
 */

tecgraf::openbus::core::v2_00::services::access_control::AccessControl::~AccessControl()
{
}

void *
tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessControl:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr
tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessControl:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessControl:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessControl:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr
tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

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
CORBA::TypeCodeConst _tc_AccessControl;
}
}
}
}
}
}
class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_AccessControl;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl;

void
operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr &_obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr *p;
  if (_a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}


/*
 * Stub interface for class AccessControl
 */

tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::~AccessControl_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessControl:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl *
POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessControl:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp::AccessControl_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp::AccessControl_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp::~AccessControl_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

char* tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::busid()
{
  tecgraf::openbus::core::v2_00::Identifier _res = NULL;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "_get_busid" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

char*
tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp::busid()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::Identifier __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->busid();
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

  return tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::busid();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::OctetSeq* tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::buskey()
{
  CORBA::StaticAny __res( CORBA::_stcseq_octet );

  CORBA::StaticRequest __req( this, "_get_buskey" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return (tecgraf::openbus::core::v2_00::OctetSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::OctetSeq*
tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp::buskey()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::OctetSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->buskey();
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

  return tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::buskey();
}

#endif // MICO_CONF_NO_POA

char* tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::loginByPassword( const char* _par_entity, const tecgraf::openbus::core::v2_00::OctetSeq& _par_pubkey, const tecgraf::openbus::core::v2_00::EncryptedBlock _par_encrypted, tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out _par_validity )
{
  CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity );
  CORBA::StaticAny _sa_pubkey( CORBA::_stcseq_octet, &_par_pubkey );
  CORBA::StaticAny _sa_encrypted( _marshaller__a256_octet, _par_encrypted );
  CORBA::StaticAny _sa_validity( CORBA::_stc_ulong, &_par_validity );
  tecgraf::openbus::core::v2_00::Identifier _res = NULL;
  CORBA::StaticAny __res( CORBA::_stc_string, &_res );

  CORBA::StaticRequest __req( this, "loginByPassword" );
  __req.add_in_arg( &_sa_entity );
  __req.add_in_arg( &_sa_pubkey );
  __req.add_in_arg( &_sa_encrypted );
  __req.add_out_arg( &_sa_validity );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding, "IDL:tecgraf/openbus/core/v2_00/services/access_control/WrongEncoding:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied, "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessDenied:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

char*
tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp::loginByPassword( const char* _par_entity, const tecgraf::openbus::core::v2_00::OctetSeq& _par_pubkey, const tecgraf::openbus::core::v2_00::EncryptedBlock _par_encrypted, tecgraf::openbus::core::v2_00::services::access_control::ValidityTime_out _par_validity )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::Identifier __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->loginByPassword(_par_entity, _par_pubkey, _par_encrypted, _par_validity);
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

  return tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::loginByPassword(_par_entity, _par_pubkey, _par_encrypted, _par_validity);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::startLoginByCertificate( const char* _par_entity, tecgraf::openbus::core::v2_00::EncryptedBlock_out _par_challenge )
{
  CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity );
  CORBA::StaticAny _sa_challenge( _marshaller__a256_octet, _par_challenge );
  tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr _res = tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_nil();
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate, &_res );

  CORBA::StaticRequest __req( this, "startLoginByCertificate" );
  __req.add_in_arg( &_sa_entity );
  __req.add_out_arg( &_sa_challenge );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate, "IDL:tecgraf/openbus/core/v2_00/services/access_control/MissingCertificate:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr
tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp::startLoginByCertificate( const char* _par_entity, tecgraf::openbus::core::v2_00::EncryptedBlock_out _par_challenge )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->startLoginByCertificate(_par_entity, _par_challenge);
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

  return tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::startLoginByCertificate(_par_entity, _par_challenge);
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::logout()
{
  CORBA::StaticRequest __req( this, "logout" );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp::logout()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->logout();
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

  tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::logout();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::ValidityTime tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::renew()
{
  tecgraf::openbus::core::v2_00::services::access_control::ValidityTime _res;
  CORBA::StaticAny __res( CORBA::_stc_ulong, &_res );

  CORBA::StaticRequest __req( this, "renew" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::ValidityTime
tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp::renew()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::access_control::ValidityTime __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->renew();
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

  return tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::renew();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain* tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::signChainFor( const char* _par_target )
{
  CORBA::StaticAny _sa_target( CORBA::_stc_string, &_par_target );
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain );

  CORBA::StaticRequest __req( this, "signChainFor" );
  __req.add_in_arg( &_sa_target );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain*
tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp::signChainFor( const char* _par_target )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->signChainFor(_par_target);
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

  return tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub::signChainFor(_par_target);
}

#endif // MICO_CONF_NO_POA

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
CORBA::TypeCodeConst _tc_InvalidLogins;
}
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::InvalidLogins()
{
}

tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::InvalidLogins( const InvalidLogins& _s )
{
  loginIds = ((InvalidLogins&)_s).loginIds;
}

tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::~InvalidLogins()
{
}

tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins&
tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::operator=( const InvalidLogins& _s )
{
  loginIds = ((InvalidLogins&)_s).loginIds;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::InvalidLogins()
{
}

#endif

tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::InvalidLogins( const tecgraf::openbus::core::v2_00::IdentifierSeq& _m0 )
{
  loginIds = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    CORBA::_stcseq_string->demarshal( dc, &((_MICO_T*)v)->loginIds ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidLogins:1.0" );
  CORBA::_stcseq_string->marshal( ec, &((_MICO_T*)v)->loginIds );
  ec.except_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_InvalidLogins;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins &_e )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins, &_e);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins &_e )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins, &_e);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins *&_e )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins, (void *&)_e);
}

void tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw InvalidLogins_var( (tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidLogins:1.0";
}

void tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::_encode_any( CORBA::Any &_a ) const
{
  _a <<= *this;
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::_clone() const
{
  return new InvalidLogins( *this );
}

tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins *tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidLogins:1.0" ) )
    return (InvalidLogins *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins *tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidLogins:1.0" ) )
    return (InvalidLogins *) _ex;
  return NULL;
}


/*
 * Base interface for class LoginObserver
 */

tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::~LoginObserver()
{
}

void *
tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserver:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr
tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserver:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserver:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserver:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr
tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

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
CORBA::TypeCodeConst _tc_LoginObserver;
}
}
}
}
}
}
class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginObserver;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver;

void
operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr &_obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr *p;
  if (_a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}


/*
 * Stub interface for class LoginObserver
 */

tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_stub::~LoginObserver_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserver:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver *
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserver:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_stub_clp::LoginObserver_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_stub_clp::LoginObserver_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_stub_clp::~LoginObserver_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_stub::entityLogout( const tecgraf::openbus::core::v2_00::services::access_control::LoginInfo& _par_login )
{
  CORBA::StaticAny _sa_login( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, &_par_login );
  CORBA::StaticRequest __req( this, "entityLogout" );
  __req.add_in_arg( &_sa_login );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_stub_clp::entityLogout( const tecgraf::openbus::core::v2_00::services::access_control::LoginInfo& _par_login )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->entityLogout(_par_login);
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

  tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_stub::entityLogout(_par_login);
}

#endif // MICO_CONF_NO_POA


/*
 * Base interface for class LoginObserverSubscription
 */

tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::~LoginObserverSubscription()
{
}

void *
tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserverSubscription:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr
tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserverSubscription:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserverSubscription:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserverSubscription:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr
tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

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
CORBA::TypeCodeConst _tc_LoginObserverSubscription;
}
}
}
}
}
}
class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginObserverSubscription;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription;

void
operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr &_obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr *p;
  if (_a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}


/*
 * Stub interface for class LoginObserverSubscription
 */

tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::~LoginObserverSubscription_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserverSubscription:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription *
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserverSubscription:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub_clp::LoginObserverSubscription_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub_clp::LoginObserverSubscription_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub_clp::~LoginObserverSubscription_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

CORBA::Boolean tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::watchLogin( const char* _par_loginId )
{
  CORBA::StaticAny _sa_loginId( CORBA::_stc_string, &_par_loginId );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "watchLogin" );
  __req.add_in_arg( &_sa_loginId );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

CORBA::Boolean
tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub_clp::watchLogin( const char* _par_loginId )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow (_serv);
    if (_myserv) {
      CORBA::Boolean __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->watchLogin(_par_loginId);
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

  return tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::watchLogin(_par_loginId);
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::forgetLogin( const char* _par_loginId )
{
  CORBA::StaticAny _sa_loginId( CORBA::_stc_string, &_par_loginId );
  CORBA::StaticRequest __req( this, "forgetLogin" );
  __req.add_in_arg( &_sa_loginId );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub_clp::forgetLogin( const char* _par_loginId )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->forgetLogin(_par_loginId);
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

  tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::forgetLogin(_par_loginId);
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::watchLogins( const tecgraf::openbus::core::v2_00::IdentifierSeq& _par_loginIds )
{
  CORBA::StaticAny _sa_loginIds( CORBA::_stcseq_string, &_par_loginIds );
  CORBA::StaticRequest __req( this, "watchLogins" );
  __req.add_in_arg( &_sa_loginIds );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins, "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidLogins:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub_clp::watchLogins( const tecgraf::openbus::core::v2_00::IdentifierSeq& _par_loginIds )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->watchLogins(_par_loginIds);
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

  tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::watchLogins(_par_loginIds);
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::forgetLogins( const tecgraf::openbus::core::v2_00::IdentifierSeq& _par_loginIds )
{
  CORBA::StaticAny _sa_loginIds( CORBA::_stcseq_string, &_par_loginIds );
  CORBA::StaticRequest __req( this, "forgetLogins" );
  __req.add_in_arg( &_sa_loginIds );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub_clp::forgetLogins( const tecgraf::openbus::core::v2_00::IdentifierSeq& _par_loginIds )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow (_serv);
    if (_myserv) {
      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _myserv->forgetLogins(_par_loginIds);
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

  tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::forgetLogins(_par_loginIds);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::getWatchedLogins()
{
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo );

  CORBA::StaticRequest __req( this, "getWatchedLogins" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    0);
  return (tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq*
tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub_clp::getWatchedLogins()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getWatchedLogins();
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

  return tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::getWatchedLogins();
}

#endif // MICO_CONF_NO_POA

void tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::remove()
{
  CORBA::StaticRequest __req( this, "remove" );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
}


#ifndef MICO_CONF_NO_POA

void
tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub_clp::remove()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow (_serv);
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

  tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub::remove();
}

#endif // MICO_CONF_NO_POA


/*
 * Base interface for class LoginRegistry
 */

tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::~LoginRegistry()
{
}

void *
tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow_helper( const char *_repoid )
{
  if( strcmp( _repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginRegistry:1.0" ) == 0 )
    return (void *)this;
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr
tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow( CORBA::Object_ptr _obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr _o;
  if( !CORBA::is_nil( _obj ) ) {
    void *_p;
    if( (_p = _obj->_narrow_helper( "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginRegistry:1.0" )))
      return _duplicate( (tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr) _p );
    if (!strcmp (_obj->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginRegistry:1.0") || _obj->_is_a_remote ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginRegistry:1.0")) {
      _o = new tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub;
      _o->CORBA::Object::operator=( *_obj );
      return _o;
    }
  }
  return _nil();
}

tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr
tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow( CORBA::AbstractBase_ptr _obj )
{
  return _narrow (_obj->_to_object());
}

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
CORBA::TypeCodeConst _tc_LoginRegistry;
}
}
}
}
}
}
class _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    void release (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry::~_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry::create() const
{
  return (StaticValueType) new _MICO_T( 0 );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = ::tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_duplicate( *(_MICO_T*) s );
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry::free( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
  delete (_MICO_T*) v;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry::release( StaticValueType v ) const
{
  ::CORBA::release( *(_MICO_T *) v );
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj;
  if (!::CORBA::_stc_Object->demarshal(dc, &obj))
    return FALSE;
  *(_MICO_T *) v = ::tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow( obj );
  ::CORBA::Boolean ret = ::CORBA::is_nil (obj) || !::CORBA::is_nil (*(_MICO_T *)v);
  ::CORBA::release (obj);
  return ret;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::Object_ptr obj = *(_MICO_T *) v;
  ::CORBA::_stc_Object->marshal( ec, &obj );
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry::typecode()
{
  return tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginRegistry;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry;

void
operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr _obj )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry, &_obj);
  _a.from_static_any (_sa);
}

void
operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr* _obj_ptr )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry, _obj_ptr);
  _a.from_static_any (_sa);
  CORBA::release (*_obj_ptr);
}

CORBA::Boolean
operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr &_obj )
{
  tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr *p;
  if (_a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry, (void *&)p)) {
    _obj = *p;
    return TRUE;
  }
  return FALSE;
}


/*
 * Stub interface for class LoginRegistry
 */

tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::~LoginRegistry_stub()
{
}

#ifndef MICO_CONF_NO_POA

void *
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow_helper (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginRegistry:1.0") == 0) {
    return (void *) this;
  }
  return NULL;
}

POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry *
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow (PortableServer::Servant serv) 
{
  void * p;
  if ((p = serv->_narrow_helper ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginRegistry:1.0")) != NULL) {
    serv->_add_ref ();
    return (POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry *) p;
  }
  return NULL;
}

tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub_clp::LoginRegistry_stub_clp ()
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub_clp::LoginRegistry_stub_clp (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
  : CORBA::Object(*obj), PortableServer::StubBase(poa)
{
}

tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub_clp::~LoginRegistry_stub_clp ()
{
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::getAllLogins()
{
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo );

  CORBA::StaticRequest __req( this, "getAllLogins" );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq*
tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub_clp::getAllLogins()
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getAllLogins();
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

  return tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::getAllLogins();
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::getEntityLogins( const char* _par_entity )
{
  CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity );
  CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo );

  CORBA::StaticRequest __req( this, "getEntityLogins" );
  __req.add_in_arg( &_sa_entity );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq*
tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub_clp::getEntityLogins( const char* _par_entity )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getEntityLogins(_par_entity);
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

  return tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::getEntityLogins(_par_entity);
}

#endif // MICO_CONF_NO_POA

CORBA::Boolean tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::invalidateLogin( const char* _par_loginId )
{
  CORBA::StaticAny _sa_loginId( CORBA::_stc_string, &_par_loginId );
  CORBA::Boolean _res;
  CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );

  CORBA::StaticRequest __req( this, "invalidateLogin" );
  __req.add_in_arg( &_sa_loginId );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

CORBA::Boolean
tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub_clp::invalidateLogin( const char* _par_loginId )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow (_serv);
    if (_myserv) {
      CORBA::Boolean __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->invalidateLogin(_par_loginId);
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

  return tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::invalidateLogin(_par_loginId);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginInfo* tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::getLoginInfo( const char* _par_loginId, tecgraf::openbus::core::v2_00::OctetSeq_out _par_pubkey )
{
  CORBA::StaticAny _sa_loginId( CORBA::_stc_string, &_par_loginId );
  CORBA::StaticAny _sa_pubkey( CORBA::_stcseq_octet );
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo );

  CORBA::StaticRequest __req( this, "getLoginInfo" );
  __req.add_in_arg( &_sa_loginId );
  __req.add_out_arg( &_sa_pubkey );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins, "IDL:tecgraf/openbus/core/v2_00/services/access_control/InvalidLogins:1.0",
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  _par_pubkey = (tecgraf::openbus::core::v2_00::OctetSeq*) _sa_pubkey._retn();
  return (tecgraf::openbus::core::v2_00::services::access_control::LoginInfo*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginInfo*
tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub_clp::getLoginInfo( const char* _par_loginId, tecgraf::openbus::core::v2_00::OctetSeq_out _par_pubkey )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::access_control::LoginInfo* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getLoginInfo(_par_loginId, _par_pubkey);
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

  return tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::getLoginInfo(_par_loginId, _par_pubkey);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::ValidityTimeSeq* tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::getValidity( const tecgraf::openbus::core::v2_00::IdentifierSeq& _par_loginIds )
{
  CORBA::StaticAny _sa_loginIds( CORBA::_stcseq_string, &_par_loginIds );
  CORBA::StaticAny __res( CORBA::_stcseq_ulong );

  CORBA::StaticRequest __req( this, "getValidity" );
  __req.add_in_arg( &_sa_loginIds );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return (tecgraf::openbus::core::v2_00::services::access_control::ValidityTimeSeq*) __res._retn();
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::ValidityTimeSeq*
tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub_clp::getValidity( const tecgraf::openbus::core::v2_00::IdentifierSeq& _par_loginIds )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::access_control::ValidityTimeSeq* __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->getValidity(_par_loginIds);
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

  return tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::getValidity(_par_loginIds);
}

#endif // MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::subscribeObserver( tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr _par_callback )
{
  CORBA::StaticAny _sa_callback( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver, &_par_callback );
  tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr _res = tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_nil();
  CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription, &_res );

  CORBA::StaticRequest __req( this, "subscribeObserver" );
  __req.add_in_arg( &_sa_callback );
  __req.set_result( &__res );

  __req.invoke();

  mico_sii_throw( &__req, 
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure, "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0",
    0);
  return _res;
}


#ifndef MICO_CONF_NO_POA

tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr
tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub_clp::subscribeObserver( tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr _par_callback )
{
  PortableServer::Servant _serv = _preinvoke ();
  if (_serv) {
    POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry * _myserv = POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow (_serv);
    if (_myserv) {
      tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr __res;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        __res = _myserv->subscribeObserver(_par_callback);
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

  return tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub::subscribeObserver(_par_callback);
}

#endif // MICO_CONF_NO_POA

class _Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo : public ::CORBA::StaticTypeInfo {
    typedef SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::LoginInfo,MICO_TID_DEF> _MICO_T;
    static ::CORBA::TypeCode_ptr _tc;
  public:
    ~_Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::~_Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo()
{
  if (_tc)
    delete _tc;
}

::CORBA::StaticValueType _Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  ::CORBA::ULong len;
  if( !dc.seq_begin( len ) )
    return FALSE;
  ((_MICO_T *) v)->length( len );
  for( ::CORBA::ULong i = 0; i < len; i++ ) {
    if( !_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo->demarshal( dc, &(*(_MICO_T*)v)[i] ) )
      return FALSE;
  }
  return dc.seq_end();
}

void _Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ::CORBA::ULong len = ((_MICO_T *) v)->length();
  ec.seq_begin( len );
  for( ::CORBA::ULong i = 0; i < len; i++ )
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo->marshal( ec, &(*(_MICO_T*)v)[i] );
  ec.seq_end();
}

::CORBA::TypeCode_ptr _Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::typecode()
{
  if (!_tc)
    _tc = (new ::CORBA::TypeCode (
    "010000001300000038010000010000000f00000028010000010000004500"
    "000049444c3a746563677261662f6f70656e6275732f636f72652f76325f"
    "30302f73657276696365732f6163636573735f636f6e74726f6c2f4c6f67"
    "696e496e666f3a312e30000000000a0000004c6f67696e496e666f000000"
    "0200000003000000696400001500000050000000010000002e0000004944"
    "4c3a746563677261662f6f70656e6275732f636f72652f76325f30302f49"
    "64656e7469666965723a312e300000000b0000004964656e746966696572"
    "0000120000000000000007000000656e7469747900001500000050000000"
    "010000002e00000049444c3a746563677261662f6f70656e6275732f636f"
    "72652f76325f30302f4964656e7469666965723a312e300000000b000000"
    "4964656e7469666965720000120000000000000000000000"))->mk_constant();
  return _tc;
}

::CORBA::TypeCode_ptr _Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo::_tc = 0;
::CORBA::StaticTypeInfo *_marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo;

void operator<<=( CORBA::Any &_a, const SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::LoginInfo,MICO_TID_DEF> &_s )
{
  CORBA::StaticAny _sa (_marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::LoginInfo,MICO_TID_DEF> *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::LoginInfo,MICO_TID_DEF> &_s )
{
  CORBA::StaticAny _sa (_marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const SequenceTmpl< tecgraf::openbus::core::v2_00::services::access_control::LoginInfo,MICO_TID_DEF> *&_s )
{
  return _a.to_static_any (_marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, (void *&)_s);
}


struct __tc_init_ACCESS_CONTROL {
  __tc_init_ACCESS_CONTROL()
  {
    tecgraf::openbus::core::v2_00::services::access_control::_tc_EntityCertificate = 
    "010000001500000084000000010000004d00000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f456e74697479436572746966696361"
    "74653a312e300000000012000000456e7469747943657274696669636174"
    "65000000130000000c000000010000000a00000000000000";
    tecgraf::openbus::core::v2_00::services::access_control::_tc_MissingCertificate = 
    "0100000016000000d8000000010000004e00000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f4d697373696e674365727469666963"
    "6174653a312e30000000130000004d697373696e67436572746966696361"
    "746500000100000007000000656e74697479000015000000500000000100"
    "00002e00000049444c3a746563677261662f6f70656e6275732f636f7265"
    "2f76325f30302f4964656e7469666965723a312e300000000b0000004964"
    "656e74696669657200001200000000000000";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_InvalidCertificate = 
    "010000001600000088000000010000004e00000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f496e76616c69644365727469666963"
    "6174653a312e3000000013000000496e76616c6964436572746966696361"
    "7465000001000000080000006d657373616765001200000000000000";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_CertificateRegistry = 
    "010000000e00000070000000010000004f00000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f436572746966696361746552656769"
    "737472793a312e3000001400000043657274696669636174655265676973"
    "74727900";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginAuthenticationInfo = 
    "010000000f00000058010000010000005300000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f4c6f67696e41757468656e74696361"
    "74696f6e496e666f3a312e300000180000004c6f67696e41757468656e74"
    "69636174696f6e496e666f00020000000500000068617368000000001500"
    "00005c000000010000002d00000049444c3a746563677261662f6f70656e"
    "6275732f636f72652f76325f30302f4861736856616c75653a312e300000"
    "00000a0000004861736856616c7565000000140000000c00000001000000"
    "0a0000002000000005000000646174610000000015000000580000000100"
    "00002c00000049444c3a746563677261662f6f70656e6275732f636f7265"
    "2f76325f30302f4f637465745365713a312e3000090000004f6374657453"
    "657100000000130000000c000000010000000a00000000000000";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginInfo = 
    "010000000f00000028010000010000004500000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f4c6f67696e496e666f3a312e300000"
    "00000a0000004c6f67696e496e666f000000020000000300000069640000"
    "1500000050000000010000002e00000049444c3a746563677261662f6f70"
    "656e6275732f636f72652f76325f30302f4964656e7469666965723a312e"
    "300000000b0000004964656e746966696572000012000000000000000700"
    "0000656e7469747900001500000050000000010000002e00000049444c3a"
    "746563677261662f6f70656e6275732f636f72652f76325f30302f496465"
    "6e7469666965723a312e300000000b0000004964656e7469666965720000"
    "1200000000000000";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginInfoSeq = 
    "0100000015000000a4010000010000004800000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f4c6f67696e496e666f5365713a312e"
    "30000d0000004c6f67696e496e666f536571000000001300000038010000"
    "010000000f00000028010000010000004500000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f4c6f67696e496e666f3a312e300000"
    "00000a0000004c6f67696e496e666f000000020000000300000069640000"
    "1500000050000000010000002e00000049444c3a746563677261662f6f70"
    "656e6275732f636f72652f76325f30302f4964656e7469666965723a312e"
    "300000000b0000004964656e746966696572000012000000000000000700"
    "0000656e7469747900001500000050000000010000002e00000049444c3a"
    "746563677261662f6f70656e6275732f636f72652f76325f30302f496465"
    "6e7469666965723a312e300000000b0000004964656e7469666965720000"
    "120000000000000000000000";
    tecgraf::openbus::core::v2_00::services::access_control::_tc_CallChain = 
    "010000000f00000080020000010000004500000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f43616c6c436861696e3a312e300000"
    "00000a00000043616c6c436861696e000000020000000700000074617267"
    "657400001500000050000000010000002e00000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f4964656e7469666965"
    "723a312e300000000b0000004964656e7469666965720000120000000000"
    "00000800000063616c6c6572730015000000a40100000100000048000000"
    "49444c3a746563677261662f6f70656e6275732f636f72652f76325f3030"
    "2f73657276696365732f6163636573735f636f6e74726f6c2f4c6f67696e"
    "496e666f5365713a312e30000d0000004c6f67696e496e666f5365710000"
    "00001300000038010000010000000f000000280100000100000045000000"
    "49444c3a746563677261662f6f70656e6275732f636f72652f76325f3030"
    "2f73657276696365732f6163636573735f636f6e74726f6c2f4c6f67696e"
    "496e666f3a312e30000000000a0000004c6f67696e496e666f0000000200"
    "000003000000696400001500000050000000010000002e00000049444c3a"
    "746563677261662f6f70656e6275732f636f72652f76325f30302f496465"
    "6e7469666965723a312e300000000b0000004964656e7469666965720000"
    "120000000000000007000000656e74697479000015000000500000000100"
    "00002e00000049444c3a746563677261662f6f70656e6275732f636f7265"
    "2f76325f30302f4964656e7469666965723a312e300000000b0000004964"
    "656e7469666965720000120000000000000000000000";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_SignedCallChain = 
    "010000000f00000054010000010000004b00000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f5369676e656443616c6c436861696e"
    "3a312e300000100000005369676e656443616c6c436861696e0002000000"
    "0a0000007369676e61747572650000001500000064000000010000003200"
    "000049444c3a746563677261662f6f70656e6275732f636f72652f76325f"
    "30302f456e63727970746564426c6f636b3a312e300000000f000000456e"
    "63727970746564426c6f636b0000140000000c000000010000000a000000"
    "0001000008000000656e636f646564001500000058000000010000002c00"
    "000049444c3a746563677261662f6f70656e6275732f636f72652f76325f"
    "30302f4f637465745365713a312e3000090000004f637465745365710000"
    "0000130000000c000000010000000a00000000000000";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_ValidityTime = 
    "010000001500000068000000010000004800000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f56616c696469747954696d653a312e"
    "30000d00000056616c696469747954696d650000000005000000";
    tecgraf::openbus::core::v2_00::services::access_control::_tc_ValidityTimeSeq = 
    "0100000015000000e8000000010000004b00000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f56616c696469747954696d65536571"
    "3a312e3000001000000056616c696469747954696d655365710013000000"
    "78000000010000001500000068000000010000004800000049444c3a7465"
    "63677261662f6f70656e6275732f636f72652f76325f30302f7365727669"
    "6365732f6163636573735f636f6e74726f6c2f56616c696469747954696d"
    "653a312e30000d00000056616c696469747954696d650000000005000000"
    "00000000";
    tecgraf::openbus::core::v2_00::services::access_control::_tc_WrongEncoding = 
    "01000000160000006c000000010000004900000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f57726f6e67456e636f64696e673a31"
    "2e30000000000e00000057726f6e67456e636f64696e6700000000000000"
    ;
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_AccessDenied = 
    "010000001600000068000000010000004800000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f41636365737344656e6965643a312e"
    "30000d00000041636365737344656e6965640000000000000000";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginByCertificate = 
    "010000000e0000006f000000010000004e00000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f4c6f67696e42794365727469666963"
    "6174653a312e30000000130000004c6f67696e4279436572746966696361"
    "746500";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_AccessControl = 
    "010000000e00000066000000010000004900000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f416363657373436f6e74726f6c3a31"
    "2e30000000000e000000416363657373436f6e74726f6c00";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_InvalidLogins = 
    "01000000160000003c010000010000004900000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f496e76616c69644c6f67696e733a31"
    "2e30000000000e000000496e76616c69644c6f67696e7300000001000000"
    "090000006c6f67696e4964730000000015000000b8000000010000003100"
    "000049444c3a746563677261662f6f70656e6275732f636f72652f76325f"
    "30302f4964656e7469666965725365713a312e30000000000e0000004964"
    "656e74696669657253657100000013000000600000000100000015000000"
    "50000000010000002e00000049444c3a746563677261662f6f70656e6275"
    "732f636f72652f76325f30302f4964656e7469666965723a312e30000000"
    "0b0000004964656e7469666965720000120000000000000000000000";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginObserver = 
    "010000000e00000066000000010000004900000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f4c6f67696e4f627365727665723a31"
    "2e30000000000e0000004c6f67696e4f6273657276657200";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginObserverSubscription = 
    "010000000e0000007e000000010000005500000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f4c6f67696e4f627365727665725375"
    "62736372697074696f6e3a312e30000000001a0000004c6f67696e4f6273"
    "6572766572537562736372697074696f6e00";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription;
    tecgraf::openbus::core::v2_00::services::access_control::_tc_LoginRegistry = 
    "010000000e00000066000000010000004900000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f73657276696365732f"
    "6163636573735f636f6e74726f6c2f4c6f67696e52656769737472793a31"
    "2e30000000000e0000004c6f67696e526567697374727900";
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry = new _Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry;
    _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo = new _Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo;
  }

  ~__tc_init_ACCESS_CONTROL()
  {
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_MissingCertificate);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidCertificate);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CertificateRegistry);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginAuthenticationInfo);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_CallChain);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_WrongEncoding);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessDenied);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_AccessControl);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_InvalidLogins);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry*>(_marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginRegistry);
    delete static_cast<_Marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo*>(_marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo);
  }
};

static __tc_init_ACCESS_CONTROL __init_ACCESS_CONTROL;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------

// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry
POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::~CertificateRegistry()
{
}

::tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/CertificateRegistry:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/access_control/CertificateRegistry:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/access_control/CertificateRegistry:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( __req->op_name(), "registerCertificate" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::Identifier_var _par_entity;
      CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity._for_demarshal() );
      ::tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate _par_cert;
      CORBA::StaticAny _sa_cert( CORBA::_stcseq_octet, &_par_cert );

      __req->add_in_arg( &_sa_entity );
      __req->add_in_arg( &_sa_cert );

      if( !__req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        registerCertificate( _par_entity.inout(), _par_cert );
      #ifdef HAVE_EXCEPTIONS
      } catch( ::tecgraf::openbus::core::v2_00::services::access_control::InvalidCertificate_catch &_ex ) {
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
    if( strcmp( __req->op_name(), "getCertificate" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::Identifier_var _par_entity;
      CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity._for_demarshal() );

      ::tecgraf::openbus::core::v2_00::services::access_control::EntityCertificate* _res;
      CORBA::StaticAny __res( CORBA::_stcseq_octet );
      __req->add_in_arg( &_sa_entity );
      __req->set_result( &__res );

      if( !__req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _res = getCertificate( _par_entity.inout() );
        __res.value( CORBA::_stcseq_octet, _res );
      #ifdef HAVE_EXCEPTIONS
      } catch( ::tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate_catch &_ex ) {
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
      delete _res;
      return true;
    }
    if( strcmp( __req->op_name(), "removeCertificate" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::Identifier_var _par_entity;
      CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity._for_demarshal() );

      CORBA::Boolean _res;
      CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
      __req->add_in_arg( &_sa_entity );
      __req->set_result( &__res );

      if( !__req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _res = removeCertificate( _par_entity.inout() );
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
POA_tecgraf::openbus::core::v2_00::services::access_control::CertificateRegistry::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}


// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::~LoginByCertificate()
{
}

::tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginByCertificate:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginByCertificate:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginByCertificate:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( __req->op_name(), "login" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::OctetSeq _par_pubkey;
      CORBA::StaticAny _sa_pubkey( CORBA::_stcseq_octet, &_par_pubkey );
      ::tecgraf::openbus::core::v2_00::EncryptedBlock_slice* _par_encrypted = ::tecgraf::openbus::core::v2_00::EncryptedBlock_alloc();
      CORBA::StaticAny _sa_encrypted( _marshaller__a256_octet, _par_encrypted );
      ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime _par_validity;
      CORBA::StaticAny _sa_validity( CORBA::_stc_ulong, &_par_validity );

      ::tecgraf::openbus::core::v2_00::Identifier _res;
      CORBA::StaticAny __res( CORBA::_stc_string, &_res );
      __req->add_in_arg( &_sa_pubkey );
      __req->add_in_arg( &_sa_encrypted );
      __req->add_out_arg( &_sa_validity );
      __req->set_result( &__res );

      if( !__req->read_args() )
        return true;

      #ifdef HAVE_EXCEPTIONS
      try {
      #endif
        _res = login( _par_pubkey, _par_encrypted, _par_validity );
      #ifdef HAVE_EXCEPTIONS
      } catch( ::tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding_catch &_ex ) {
        __req->set_exception( _ex->_clone() );
        __req->write_results();
        return true;
      } catch( ::tecgraf::openbus::core::v2_00::services::access_control::AccessDenied_catch &_ex ) {
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
      ::tecgraf::openbus::core::v2_00::EncryptedBlock_free( _par_encrypted );
      CORBA::string_free( _res );
      return true;
    }
    if( strcmp( __req->op_name(), "cancel" ) == 0 ) {

      if( !__req->read_args() )
        return true;

      cancel();
      __req->write_results();
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
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}


// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::access_control::AccessControl
POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::~AccessControl()
{
}

::tecgraf::openbus::core::v2_00::services::access_control::AccessControl_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessControl:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessControl:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/access_control/AccessControl:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::access_control::AccessControl_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    switch (mico_string_hash (__req->op_name(), 11)) {
    case 0:
      if( strcmp( __req->op_name(), "loginByPassword" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_entity;
        CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity._for_demarshal() );
        ::tecgraf::openbus::core::v2_00::OctetSeq _par_pubkey;
        CORBA::StaticAny _sa_pubkey( CORBA::_stcseq_octet, &_par_pubkey );
        ::tecgraf::openbus::core::v2_00::EncryptedBlock_slice* _par_encrypted = ::tecgraf::openbus::core::v2_00::EncryptedBlock_alloc();
        CORBA::StaticAny _sa_encrypted( _marshaller__a256_octet, _par_encrypted );
        ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime _par_validity;
        CORBA::StaticAny _sa_validity( CORBA::_stc_ulong, &_par_validity );

        ::tecgraf::openbus::core::v2_00::Identifier _res;
        CORBA::StaticAny __res( CORBA::_stc_string, &_res );
        __req->add_in_arg( &_sa_entity );
        __req->add_in_arg( &_sa_pubkey );
        __req->add_in_arg( &_sa_encrypted );
        __req->add_out_arg( &_sa_validity );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = loginByPassword( _par_entity.inout(), _par_pubkey, _par_encrypted, _par_validity );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::access_control::WrongEncoding_catch &_ex ) {
          __req->set_exception( _ex->_clone() );
          __req->write_results();
          return true;
        } catch( ::tecgraf::openbus::core::v2_00::services::access_control::AccessDenied_catch &_ex ) {
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
        ::tecgraf::openbus::core::v2_00::EncryptedBlock_free( _par_encrypted );
        CORBA::string_free( _res );
        return true;
      }
      break;
    case 1:
      if( strcmp( __req->op_name(), "logout" ) == 0 ) {

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          logout();
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
    case 4:
      if( strcmp( __req->op_name(), "_get_busid" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier _res;
        CORBA::StaticAny __res( CORBA::_stc_string, &_res );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = busid();
        __req->write_results();
        CORBA::string_free( _res );
        return true;
      }
      break;
    case 5:
      if( strcmp( __req->op_name(), "startLoginByCertificate" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_entity;
        CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity._for_demarshal() );
        ::tecgraf::openbus::core::v2_00::EncryptedBlock_slice* _par_challenge = ::tecgraf::openbus::core::v2_00::EncryptedBlock_alloc();
        CORBA::StaticAny _sa_challenge( _marshaller__a256_octet, _par_challenge );

        ::tecgraf::openbus::core::v2_00::services::access_control::LoginByCertificate_ptr _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginByCertificate, &_res );
        __req->add_in_arg( &_sa_entity );
        __req->add_out_arg( &_sa_challenge );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = startLoginByCertificate( _par_entity.inout(), _par_challenge );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::access_control::MissingCertificate_catch &_ex ) {
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
        ::tecgraf::openbus::core::v2_00::EncryptedBlock_free( _par_challenge );
        CORBA::release( _res );
        return true;
      }
      break;
    case 7:
      if( strcmp( __req->op_name(), "signChainFor" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_target;
        CORBA::StaticAny _sa_target( CORBA::_stc_string, &_par_target._for_demarshal() );

        ::tecgraf::openbus::core::v2_00::services::access_control::SignedCallChain* _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain );
        __req->add_in_arg( &_sa_target );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = signChainFor( _par_target.inout() );
          __res.value( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain, _res );
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
    case 8:
      if( strcmp( __req->op_name(), "renew" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTime _res;
        CORBA::StaticAny __res( CORBA::_stc_ulong, &_res );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = renew();
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
    case 10:
      if( strcmp( __req->op_name(), "_get_buskey" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::OctetSeq* _res;
        CORBA::StaticAny __res( CORBA::_stcseq_octet );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = buskey();
        __res.value( CORBA::_stcseq_octet, _res );
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
POA_tecgraf::openbus::core::v2_00::services::access_control::AccessControl::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}


// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::access_control::LoginObserver
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::~LoginObserver()
{
}

::tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserver:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserver:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserver:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    if( strcmp( __req->op_name(), "entityLogout" ) == 0 ) {
      ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo _par_login;
      CORBA::StaticAny _sa_login( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, &_par_login );

      __req->add_in_arg( &_sa_login );

      if( !__req->read_args() )
        return true;

      entityLogout( _par_login );
      __req->write_results();
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
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserver::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}


// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::~LoginObserverSubscription()
{
}

::tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserverSubscription:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserverSubscription:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginObserverSubscription:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    switch (mico_string_hash (__req->op_name(), 11)) {
    case 0:
      if( strcmp( __req->op_name(), "watchLogin" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_loginId;
        CORBA::StaticAny _sa_loginId( CORBA::_stc_string, &_par_loginId._for_demarshal() );

        CORBA::Boolean _res;
        CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
        __req->add_in_arg( &_sa_loginId );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = watchLogin( _par_loginId.inout() );
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
      if( strcmp( __req->op_name(), "forgetLogins" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::IdentifierSeq _par_loginIds;
        CORBA::StaticAny _sa_loginIds( CORBA::_stcseq_string, &_par_loginIds );

        __req->add_in_arg( &_sa_loginIds );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          forgetLogins( _par_loginIds );
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
      if( strcmp( __req->op_name(), "watchLogins" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::IdentifierSeq _par_loginIds;
        CORBA::StaticAny _sa_loginIds( CORBA::_stcseq_string, &_par_loginIds );

        __req->add_in_arg( &_sa_loginIds );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          watchLogins( _par_loginIds );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins_catch &_ex ) {
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
    case 6:
      if( strcmp( __req->op_name(), "forgetLogin" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_loginId;
        CORBA::StaticAny _sa_loginId( CORBA::_stc_string, &_par_loginId._for_demarshal() );

        __req->add_in_arg( &_sa_loginId );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          forgetLogin( _par_loginId.inout() );
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
    case 7:
      if( strcmp( __req->op_name(), "getWatchedLogins" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* _res;
        CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        _res = getWatchedLogins();
        __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, _res );
        __req->write_results();
        delete _res;
        return true;
      }
      break;
    case 8:
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
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}


// PortableServer Skeleton Class for interface tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::~LoginRegistry()
{
}

::tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_this ()
{
  CORBA::Object_var obj = PortableServer::ServantBase::_this();
  return ::tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_narrow (obj);
}

CORBA::Boolean
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_is_a (const char * repoid)
{
  if (strcmp (repoid, "IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginRegistry:1.0") == 0) {
    return TRUE;
  }
  return FALSE;
}

CORBA::InterfaceDef_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_get_interface ()
{
  CORBA::InterfaceDef_ptr ifd = PortableServer::ServantBase::_get_interface ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginRegistry:1.0");

  if (CORBA::is_nil (ifd)) {
    mico_throw (CORBA::OBJ_ADAPTER (0, CORBA::COMPLETED_NO));
  }

  return ifd;
}

CORBA::RepositoryId
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_primary_interface (const PortableServer::ObjectId &, PortableServer::POA_ptr)
{
  return CORBA::string_dup ("IDL:tecgraf/openbus/core/v2_00/services/access_control/LoginRegistry:1.0");
}

CORBA::Object_ptr
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::_make_stub (PortableServer::POA_ptr poa, CORBA::Object_ptr obj)
{
  return new ::tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry_stub_clp (poa, obj);
}

bool
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::dispatch (CORBA::StaticServerRequest_ptr __req)
{
  #ifdef HAVE_EXCEPTIONS
  try {
  #endif
    switch (mico_string_hash (__req->op_name(), 11)) {
    case 0:
      if( strcmp( __req->op_name(), "getAllLogins" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* _res;
        CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getAllLogins();
          __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, _res );
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
      if( strcmp( __req->op_name(), "getEntityLogins" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_entity;
        CORBA::StaticAny _sa_entity( CORBA::_stc_string, &_par_entity._for_demarshal() );

        ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfoSeq* _res;
        CORBA::StaticAny __res( _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo );
        __req->add_in_arg( &_sa_entity );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getEntityLogins( _par_entity.inout() );
          __res.value( _marshaller__seq_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, _res );
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
      if( strcmp( __req->op_name(), "invalidateLogin" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_loginId;
        CORBA::StaticAny _sa_loginId( CORBA::_stc_string, &_par_loginId._for_demarshal() );

        CORBA::Boolean _res;
        CORBA::StaticAny __res( CORBA::_stc_boolean, &_res );
        __req->add_in_arg( &_sa_loginId );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = invalidateLogin( _par_loginId.inout() );
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
    case 7:
      if( strcmp( __req->op_name(), "getLoginInfo" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::Identifier_var _par_loginId;
        CORBA::StaticAny _sa_loginId( CORBA::_stc_string, &_par_loginId._for_demarshal() );
        ::tecgraf::openbus::core::v2_00::OctetSeq* _par_pubkey;
        CORBA::StaticAny _sa_pubkey( CORBA::_stcseq_octet );

        ::tecgraf::openbus::core::v2_00::services::access_control::LoginInfo* _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo );
        __req->add_in_arg( &_sa_loginId );
        __req->add_out_arg( &_sa_pubkey );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getLoginInfo( _par_loginId.inout(), _par_pubkey );
          _sa_pubkey.value( CORBA::_stcseq_octet, _par_pubkey );
          __res.value( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginInfo, _res );
        #ifdef HAVE_EXCEPTIONS
        } catch( ::tecgraf::openbus::core::v2_00::services::access_control::InvalidLogins_catch &_ex ) {
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
        delete _par_pubkey;
        delete _res;
        return true;
      }
      if( strcmp( __req->op_name(), "getValidity" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::IdentifierSeq _par_loginIds;
        CORBA::StaticAny _sa_loginIds( CORBA::_stcseq_string, &_par_loginIds );

        ::tecgraf::openbus::core::v2_00::services::access_control::ValidityTimeSeq* _res;
        CORBA::StaticAny __res( CORBA::_stcseq_ulong );
        __req->add_in_arg( &_sa_loginIds );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = getValidity( _par_loginIds );
          __res.value( CORBA::_stcseq_ulong, _res );
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
      if( strcmp( __req->op_name(), "subscribeObserver" ) == 0 ) {
        ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserver_var _par_callback;
        CORBA::StaticAny _sa_callback( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserver, &_par_callback._for_demarshal() );

        ::tecgraf::openbus::core::v2_00::services::access_control::LoginObserverSubscription_ptr _res;
        CORBA::StaticAny __res( _marshaller_tecgraf_openbus_core_v2_00_services_access_control_LoginObserverSubscription, &_res );
        __req->add_in_arg( &_sa_callback );
        __req->set_result( &__res );

        if( !__req->read_args() )
          return true;

        #ifdef HAVE_EXCEPTIONS
        try {
        #endif
          _res = subscribeObserver( _par_callback.inout() );
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
POA_tecgraf::openbus::core::v2_00::services::access_control::LoginRegistry::invoke (CORBA::StaticServerRequest_ptr __req)
{
  if (dispatch (__req)) {
      return;
  }

  CORBA::Exception * ex = 
    new CORBA::BAD_OPERATION (0, CORBA::COMPLETED_NO);
  __req->set_exception (ex);
  __req->write_results();
}

