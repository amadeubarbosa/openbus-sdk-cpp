/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2006 by The Mico Team
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <core.h>


using namespace std;

//--------------------------------------------------------
//  Implementation of stubs
//--------------------------------------------------------






#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::ServiceFailure::ServiceFailure()
{
}

tecgraf::openbus::core::v2_00::services::ServiceFailure::ServiceFailure( const ServiceFailure& _s )
{
  message = ((ServiceFailure&)_s).message;
}

tecgraf::openbus::core::v2_00::services::ServiceFailure::~ServiceFailure()
{
}

tecgraf::openbus::core::v2_00::services::ServiceFailure&
tecgraf::openbus::core::v2_00::services::ServiceFailure::operator=( const ServiceFailure& _s )
{
  message = ((ServiceFailure&)_s).message;
  return *this;
}
#endif

#ifndef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::services::ServiceFailure::ServiceFailure()
{
}

#endif

tecgraf::openbus::core::v2_00::services::ServiceFailure::ServiceFailure( const char* _m0 )
{
  message = _m0;
}

class _Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure : public ::CORBA::StaticTypeInfo {
    typedef ::tecgraf::openbus::core::v2_00::services::ServiceFailure _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure::~_Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  string repoid;
  return
    dc.except_begin( repoid ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->message._for_demarshal() ) &&
    dc.except_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.except_begin( "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0" );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->message.inout() );
  ec.except_end();
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure;

void tecgraf::openbus::core::v2_00::services::ServiceFailure::_throwit() const
{
  #ifdef HAVE_EXCEPTIONS
  #ifdef HAVE_STD_EH
  throw *this;
  #else
  throw ServiceFailure_var( (tecgraf::openbus::core::v2_00::services::ServiceFailure*)_clone() );
  #endif
  #else
  CORBA::Exception::_throw_failed( _clone() );
  #endif
}

const char *tecgraf::openbus::core::v2_00::services::ServiceFailure::_repoid() const
{
  return "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0";
}

void tecgraf::openbus::core::v2_00::services::ServiceFailure::_encode( CORBA::DataEncoder &_en ) const
{
  _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure->marshal( _en, (void*) this );
}

void tecgraf::openbus::core::v2_00::services::ServiceFailure::_encode_any( CORBA::Any & ) const
{
  // use --any to make this work!
  assert(0);
}

CORBA::Exception *tecgraf::openbus::core::v2_00::services::ServiceFailure::_clone() const
{
  return new ServiceFailure( *this );
}

tecgraf::openbus::core::v2_00::services::ServiceFailure *tecgraf::openbus::core::v2_00::services::ServiceFailure::_downcast( CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0" ) )
    return (ServiceFailure *) _ex;
  return NULL;
}

const tecgraf::openbus::core::v2_00::services::ServiceFailure *tecgraf::openbus::core::v2_00::services::ServiceFailure::_downcast( const CORBA::Exception *_ex )
{
  if( _ex && !strcmp( _ex->_repoid(), "IDL:tecgraf/openbus/core/v2_00/services/ServiceFailure:1.0" ) )
    return (ServiceFailure *) _ex;
  return NULL;
}

class _Marshaller__a32_octet : public ::CORBA::StaticTypeInfo {
    typedef CORBA::Octet _MICO_T;
  public:
    ~_Marshaller__a32_octet();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller__a32_octet::~_Marshaller__a32_octet()
{
}

::CORBA::StaticValueType _Marshaller__a32_octet::create() const
{
  return (StaticValueType) new _MICO_T[ 32 ];
}

void _Marshaller__a32_octet::assign( StaticValueType d, const StaticValueType s ) const
{
  for( int i = 0; i < 32; i++ )
    ((CORBA::Octet *) d)[ i ] = ((CORBA::Octet *) s)[ i ];
}

void _Marshaller__a32_octet::free( StaticValueType v ) const
{
  delete[] (_MICO_T *) v;
}

::CORBA::Boolean _Marshaller__a32_octet::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  if( !dc.arr_begin() )
    return FALSE;
  if (!dc.get_octets (&((_MICO_T *)v)[0], 32))
    return FALSE;
  return dc.arr_end();
}

void _Marshaller__a32_octet::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.arr_begin();
  ec.put_octets (&((_MICO_T *)v)[0], 32);
  ec.arr_end();
}

::CORBA::StaticTypeInfo *_marshaller__a32_octet;

class _Marshaller__a256_octet : public ::CORBA::StaticTypeInfo {
    typedef CORBA::Octet _MICO_T;
  public:
    ~_Marshaller__a256_octet();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
};


_Marshaller__a256_octet::~_Marshaller__a256_octet()
{
}

::CORBA::StaticValueType _Marshaller__a256_octet::create() const
{
  return (StaticValueType) new _MICO_T[ 256 ];
}

void _Marshaller__a256_octet::assign( StaticValueType d, const StaticValueType s ) const
{
  for( int i = 0; i < 256; i++ )
    ((CORBA::Octet *) d)[ i ] = ((CORBA::Octet *) s)[ i ];
}

void _Marshaller__a256_octet::free( StaticValueType v ) const
{
  delete[] (_MICO_T *) v;
}

::CORBA::Boolean _Marshaller__a256_octet::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  if( !dc.arr_begin() )
    return FALSE;
  if (!dc.get_octets (&((_MICO_T *)v)[0], 256))
    return FALSE;
  return dc.arr_end();
}

void _Marshaller__a256_octet::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.arr_begin();
  ec.put_octets (&((_MICO_T *)v)[0], 256);
  ec.arr_end();
}

::CORBA::StaticTypeInfo *_marshaller__a256_octet;

struct __tc_init_CORE {
  __tc_init_CORE()
  {
    _marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure = new _Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure;
    _marshaller__a32_octet = new _Marshaller__a32_octet;
    _marshaller__a256_octet = new _Marshaller__a256_octet;
  }

  ~__tc_init_CORE()
  {
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure*>(_marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure);
    delete static_cast<_Marshaller__a32_octet*>(_marshaller__a32_octet);
    delete static_cast<_Marshaller__a256_octet*>(_marshaller__a256_octet);
  }
};

static __tc_init_CORE __init_CORE;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------
