/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2006 by The Mico Team
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <credential.h>


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
namespace credential
{
CORBA::TypeCodeConst _tc_CredentialData;
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::credential::CredentialData::CredentialData()
{
}

tecgraf::openbus::core::v2_00::credential::CredentialData::CredentialData( const CredentialData& _s )
{
  bus = ((CredentialData&)_s).bus;
  login = ((CredentialData&)_s).login;
  ticket = ((CredentialData&)_s).ticket;
  hash = ((CredentialData&)_s).hash;
  chain = ((CredentialData&)_s).chain;
}

tecgraf::openbus::core::v2_00::credential::CredentialData::~CredentialData()
{
}

tecgraf::openbus::core::v2_00::credential::CredentialData&
tecgraf::openbus::core::v2_00::credential::CredentialData::operator=( const CredentialData& _s )
{
  bus = ((CredentialData&)_s).bus;
  login = ((CredentialData&)_s).login;
  ticket = ((CredentialData&)_s).ticket;
  hash = ((CredentialData&)_s).hash;
  chain = ((CredentialData&)_s).chain;
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::credential::CredentialData _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData::~_Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->bus._for_demarshal() ) &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->login._for_demarshal() ) &&
    CORBA::_stc_ulong->demarshal( dc, &((_MICO_T*)v)->ticket ) &&
    _marshaller__a32_octet->demarshal( dc, &((_MICO_T*)v)->hash ) &&
    _marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain->demarshal( dc, &((_MICO_T*)v)->chain ) &&
    dc.struct_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->bus.inout() );
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->login.inout() );
  CORBA::_stc_ulong->marshal( ec, &((_MICO_T*)v)->ticket );
  _marshaller__a32_octet->marshal( ec, &((_MICO_T*)v)->hash );
  _marshaller_tecgraf_openbus_core_v2_00_services_access_control_SignedCallChain->marshal( ec, &((_MICO_T*)v)->chain );
  ec.struct_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData::typecode()
{
  return tecgraf::openbus::core::v2_00::credential::_tc_CredentialData;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::credential::CredentialData &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::credential::CredentialData *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::credential::CredentialData &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::credential::CredentialData *&_s )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData, (void *&)_s);
}

namespace tecgraf
{
namespace openbus
{
namespace core
{
namespace v2_00
{
namespace credential
{
CORBA::TypeCodeConst _tc_CredentialReset;
}
}
}
}
}

#ifdef HAVE_EXPLICIT_STRUCT_OPS
tecgraf::openbus::core::v2_00::credential::CredentialReset::CredentialReset()
{
}

tecgraf::openbus::core::v2_00::credential::CredentialReset::CredentialReset( const CredentialReset& _s )
{
  login = ((CredentialReset&)_s).login;
  challenge = ((CredentialReset&)_s).challenge;
}

tecgraf::openbus::core::v2_00::credential::CredentialReset::~CredentialReset()
{
}

tecgraf::openbus::core::v2_00::credential::CredentialReset&
tecgraf::openbus::core::v2_00::credential::CredentialReset::operator=( const CredentialReset& _s )
{
  login = ((CredentialReset&)_s).login;
  challenge = ((CredentialReset&)_s).challenge;
  return *this;
}
#endif

class _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset : public ::CORBA::StaticTypeInfo {
    typedef tecgraf::openbus::core::v2_00::credential::CredentialReset _MICO_T;
  public:
    ~_Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset();
    StaticValueType create () const;
    void assign (StaticValueType dst, const StaticValueType src) const;
    void free (StaticValueType) const;
    ::CORBA::Boolean demarshal (::CORBA::DataDecoder&, StaticValueType) const;
    void marshal (::CORBA::DataEncoder &, StaticValueType) const;
    ::CORBA::TypeCode_ptr typecode ();
};


_Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset::~_Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset()
{
}

::CORBA::StaticValueType _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset::create() const
{
  return (StaticValueType) new _MICO_T;
}

void _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset::assign( StaticValueType d, const StaticValueType s ) const
{
  *(_MICO_T*) d = *(_MICO_T*) s;
}

void _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset::free( StaticValueType v ) const
{
  delete (_MICO_T*) v;
}

::CORBA::Boolean _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset::demarshal( ::CORBA::DataDecoder &dc, StaticValueType v ) const
{
  return
    dc.struct_begin() &&
    CORBA::_stc_string->demarshal( dc, &((_MICO_T*)v)->login._for_demarshal() ) &&
    _marshaller__a256_octet->demarshal( dc, &((_MICO_T*)v)->challenge ) &&
    dc.struct_end();
}

void _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset::marshal( ::CORBA::DataEncoder &ec, StaticValueType v ) const
{
  ec.struct_begin();
  CORBA::_stc_string->marshal( ec, &((_MICO_T*)v)->login.inout() );
  _marshaller__a256_octet->marshal( ec, &((_MICO_T*)v)->challenge );
  ec.struct_end();
}

::CORBA::TypeCode_ptr _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset::typecode()
{
  return tecgraf::openbus::core::v2_00::credential::_tc_CredentialReset;
}

::CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset;

void operator<<=( CORBA::Any &_a, const tecgraf::openbus::core::v2_00::credential::CredentialReset &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset, &_s);
  _a.from_static_any (_sa);
}

void operator<<=( CORBA::Any &_a, tecgraf::openbus::core::v2_00::credential::CredentialReset *_s )
{
  _a <<= *_s;
  delete _s;
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, tecgraf::openbus::core::v2_00::credential::CredentialReset &_s )
{
  CORBA::StaticAny _sa (_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset, &_s);
  return _a.to_static_any (_sa);
}

CORBA::Boolean operator>>=( const CORBA::Any &_a, const tecgraf::openbus::core::v2_00::credential::CredentialReset *&_s )
{
  return _a.to_static_any (_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset, (void *&)_s);
}

struct __tc_init_CREDENTIAL {
  __tc_init_CREDENTIAL()
  {
    tecgraf::openbus::core::v2_00::credential::_tc_CredentialData = 
    "010000000f0000000c030000010000003d00000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f63726564656e746961"
    "6c2f43726564656e7469616c446174613a312e30000000000f0000004372"
    "6564656e7469616c44617461000005000000040000006275730015000000"
    "50000000010000002e00000049444c3a746563677261662f6f70656e6275"
    "732f636f72652f76325f30302f4964656e7469666965723a312e30000000"
    "0b0000004964656e74696669657200001200000000000000060000006c6f"
    "67696e0000001500000050000000010000002e00000049444c3a74656367"
    "7261662f6f70656e6275732f636f72652f76325f30302f4964656e746966"
    "6965723a312e300000000b0000004964656e746966696572000012000000"
    "00000000070000007469636b657400000500000005000000686173680000"
    "0000150000005c000000010000002d00000049444c3a746563677261662f"
    "6f70656e6275732f636f72652f76325f30302f4861736856616c75653a31"
    "2e30000000000a0000004861736856616c7565000000140000000c000000"
    "010000000a0000002000000006000000636861696e0000000f0000005401"
    "0000010000004b00000049444c3a746563677261662f6f70656e6275732f"
    "636f72652f76325f30302f73657276696365732f6163636573735f636f6e"
    "74726f6c2f5369676e656443616c6c436861696e3a312e30000010000000"
    "5369676e656443616c6c436861696e00020000000a0000007369676e6174"
    "7572650000001500000064000000010000003200000049444c3a74656367"
    "7261662f6f70656e6275732f636f72652f76325f30302f456e6372797074"
    "6564426c6f636b3a312e300000000f000000456e63727970746564426c6f"
    "636b0000140000000c000000010000000a0000000001000008000000656e"
    "636f646564001500000058000000010000002c00000049444c3a74656367"
    "7261662f6f70656e6275732f636f72652f76325f30302f4f637465745365"
    "713a312e3000090000004f6374657453657100000000130000000c000000"
    "010000000a00000000000000";
    _marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData = new _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData;
    tecgraf::openbus::core::v2_00::credential::_tc_CredentialReset = 
    "010000000f00000040010000010000003e00000049444c3a746563677261"
    "662f6f70656e6275732f636f72652f76325f30302f63726564656e746961"
    "6c2f43726564656e7469616c52657365743a312e30000000100000004372"
    "6564656e7469616c52657365740002000000060000006c6f67696e000000"
    "1500000050000000010000002e00000049444c3a746563677261662f6f70"
    "656e6275732f636f72652f76325f30302f4964656e7469666965723a312e"
    "300000000b0000004964656e746966696572000012000000000000000a00"
    "00006368616c6c656e676500000015000000640000000100000032000000"
    "49444c3a746563677261662f6f70656e6275732f636f72652f76325f3030"
    "2f456e63727970746564426c6f636b3a312e300000000f000000456e6372"
    "7970746564426c6f636b0000140000000c000000010000000a0000000001"
    "0000";
    _marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset = new _Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset;
  }

  ~__tc_init_CREDENTIAL()
  {
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData*>(_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData);
    delete static_cast<_Marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset*>(_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset);
  }
};

static __tc_init_CREDENTIAL __init_CREDENTIAL;

//--------------------------------------------------------
//  Implementation of skeletons
//--------------------------------------------------------
