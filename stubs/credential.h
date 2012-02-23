/*
 *  MICO --- an Open Source CORBA implementation
 *  Copyright (c) 1997-2006 by The Mico Team
 *
 *  This file was automatically generated. DO NOT EDIT!
 */

#include <CORBA.h>
#include <mico/throw.h>

#ifndef __CREDENTIAL_H__
#define __CREDENTIAL_H__





#include <core.h>
#include <access_control.h>



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

const CORBA::ULong CredentialContextId = 1112888064UL;
struct CredentialData;
typedef TVarVar< CredentialData > CredentialData_var;
typedef TVarOut< CredentialData > CredentialData_out;


struct CredentialData {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef CredentialData_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  CredentialData();
  ~CredentialData();
  CredentialData( const CredentialData& s );
  CredentialData& operator=( const CredentialData& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  Identifier_var bus;
  Identifier_var login;
  CORBA::ULong ticket;
  HashValue hash;
  services::access_control::SignedCallChain chain;
};

extern CORBA::TypeCodeConst _tc_CredentialData;

struct CredentialReset;
typedef TVarVar< CredentialReset > CredentialReset_var;
typedef TVarOut< CredentialReset > CredentialReset_out;


struct CredentialReset {
  #ifdef HAVE_TYPEDEF_OVERLOAD
  typedef CredentialReset_var _var_type;
  #endif
  #ifdef HAVE_EXPLICIT_STRUCT_OPS
  CredentialReset();
  ~CredentialReset();
  CredentialReset( const CredentialReset& s );
  CredentialReset& operator=( const CredentialReset& s );
  #endif //HAVE_EXPLICIT_STRUCT_OPS

  Identifier_var login;
  EncryptedBlock challenge;
};

extern CORBA::TypeCodeConst _tc_CredentialReset;

}
}
}
}
}


#ifndef MICO_CONF_NO_POA

#endif // MICO_CONF_NO_POA

void operator<<=( CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::credential::CredentialData &_s );
void operator<<=( CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::credential::CredentialData *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::credential::CredentialData &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::credential::CredentialData *&_s );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialData;

void operator<<=( CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::credential::CredentialReset &_s );
void operator<<=( CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::credential::CredentialReset *_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, ::tecgraf::openbus::core::v2_00::credential::CredentialReset &_s );
CORBA::Boolean operator>>=( const CORBA::Any &_a, const ::tecgraf::openbus::core::v2_00::credential::CredentialReset *&_s );

extern CORBA::StaticTypeInfo *_marshaller_tecgraf_openbus_core_v2_00_credential_CredentialReset;

#endif
