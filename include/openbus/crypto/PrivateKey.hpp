// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_OPENBUS_PRIVATE_KEY_H_
#define TECGRAF_SDK_OPENBUS_PRIVATE_KEY_H_

#include "openbus/decl.hpp"
#include "openbus/crypto/OpenSSL.hpp"

#include <string>
#include <exception>
#include <cstring>
#include <cstddef>

namespace openbus
{
struct InvalidPrivateKey : public std::exception
{ 
  const char *what() const throw() 
  { 
    return "openbus::InvalidPrivateKey";
  } 
};

class PrivateKey
{
public:
  OPENBUS_SDK_DECL PrivateKey();
  OPENBUS_SDK_DECL PrivateKey(const CORBA::OctetSeq &key);
  OPENBUS_SDK_DECL PrivateKey(const char *key, std::size_t size);
  OPENBUS_SDK_DECL explicit PrivateKey(const std::string &filename);

  OPENBUS_SDK_DECL CORBA::OctetSeq decrypt(const unsigned char *data, std::size_t len) const;

  OPENBUS_SDK_DECL CORBA::OctetSeq pubKey();
  const CORBA::OctetSeq &octetSeq() const
  {
    return _keySeq;
  }
private:
  void set_pkey(const CORBA::OctetSeq &key);
  openssl::pkey _key;
  CORBA::OctetSeq _keySeq;
};
}
#endif
