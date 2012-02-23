/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2006 by The Mico Team
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <CORBA.h>
#include <mico/throw.h>

#ifndef __CORE_H__
#define __CORE_H__








namespace tecgraf
{
namespace openbus
{
namespace core
{
namespace v2_00
{

const char* const Version = "2_00";
const char* const BusEntity = "OpenBus";
const char* const BusObjectKey = "OpenBus";
typedef char* Identifier;
typedef CORBA::String_var Identifier_var;
typedef CORBA::String_out Identifier_out;

typedef StringSequenceTmpl<CORBA::String_var> IdentifierSeq;
typedef TSeqVar< StringSequenceTmpl<CORBA::String_var> > IdentifierSeq_var;
typedef TSeqOut< StringSequenceTmpl<CORBA::String_var> > IdentifierSeq_out;

typedef CORBA::Octet HashValue[ 32 ];
typedef CORBA::Octet HashValue_slice;
typedef ArrayFixVar< CORBA::Octet,HashValue_slice,HashValue,32 > HashValue_var;
typedef HashValue HashValue_out;
enum _dummy_HashValue { _dummy_HashValue_0 };
typedef ArrayFixForAny < CORBA::Octet,HashValue_slice,HashValue,32,_dummy_HashValue> HashValue_forany;
#undef MICO_ARRAY_ARG
#define MICO_ARRAY_ARG CORBA::Octet
DECLARE_ARRAY_ALLOC(HashValue,MICO_ARRAY_ARG,HashValue_slice,32)
DECLARE_ARRAY_DUP(HashValue,MICO_ARRAY_ARG,HashValue_slice,32)
DECLARE_ARRAY_FREE(HashValue,MICO_ARRAY_ARG,HashValue_slice,32)
DECLARE_ARRAY_COPY(HashValue,MICO_ARRAY_ARG,HashValue_slice,32)
typedef CORBA::Octet EncryptedBlock[ 256 ];
typedef CORBA::Octet EncryptedBlock_slice;
typedef ArrayFixVar< CORBA::Octet,EncryptedBlock_slice,EncryptedBlock,256 > EncryptedBlock_var;
typedef EncryptedBlock EncryptedBlock_out;
enum _dummy_EncryptedBlock { _dummy_EncryptedBlock_0 };
typedef ArrayFixForAny < CORBA::Octet,EncryptedBlock_slice,EncryptedBlock,256,_dummy_EncryptedBlock> EncryptedBlock_forany;
#undef MICO_ARRAY_ARG
#define MICO_ARRAY_ARG CORBA::Octet
DECLARE_ARRAY_ALLOC(EncryptedBlock,MICO_ARRAY_ARG,EncryptedBlock_slice,256)
DECLARE_ARRAY_DUP(EncryptedBlock,MICO_ARRAY_ARG,EncryptedBlock_slice,256)
DECLARE_ARRAY_FREE(EncryptedBlock,MICO_ARRAY_ARG,EncryptedBlock_slice,256)
DECLARE_ARRAY_COPY(EncryptedBlock,MICO_ARRAY_ARG,EncryptedBlock_slice,256)
typedef SequenceTmpl< CORBA::Octet,MICO_TID_OCTET> OctetSeq;
typedef TSeqVar< SequenceTmpl< CORBA::Octet,MICO_TID_OCTET> > OctetSeq_var;
typedef TSeqOut< SequenceTmpl< CORBA::Octet,MICO_TID_OCTET> > OctetSeq_out;



namespace services
{

struct ServiceFailure : public CORBA::UserException {
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  ServiceFailure();
  ~ServiceFailure();
  ServiceFailure( const ServiceFailure& s );
  ServiceFailure& operator=( const ServiceFailure& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  #ifndef HAVE_EXPLICIT_STRUCT_OPS
  ServiceFailure();
  #endif //HAVE_EXPLICIT_STRUCT_OPS
  ServiceFailure( const char* _m0 );

  #ifdef HAVE_STD_EH
  ServiceFailure *operator->() { return this; }
  ServiceFailure& operator*() { return *this; }
  operator ServiceFailure*() { return this; }
  #endif // HAVE_STD_EH

  void _throwit() const;
  const char *_repoid() const;
  void _encode( CORBA::DataEncoder &en ) const;
  void _encode_any( CORBA::Any &a ) const;
  CORBA::Exception *_clone() const;
  static ServiceFailure *_downcast( CORBA::Exception *ex );
  static const ServiceFailure *_downcast( const CORBA::Exception *ex );
  CORBA::String_var message;
};

#ifdef HAVE_STD_EH
typedef ServiceFailure ServiceFailure_catch;
#else
typedef ExceptVar< ServiceFailure > ServiceFailure_var;
typedef TVarOut< ServiceFailure > ServiceFailure_out;
typedef ServiceFailure_var ServiceFailure_catch;
#endif // HAVE_STD_EH

}
}
}
}
}


#ifndef MICO_CONF_NO_POA

#endif // MICO_CONF_NO_POA

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_services_ServiceFailure;

extern CORBA::StaticTypeInfo *_marshaller__a32_octet;

extern CORBA::StaticTypeInfo *_marshaller__a256_octet;

#endif
