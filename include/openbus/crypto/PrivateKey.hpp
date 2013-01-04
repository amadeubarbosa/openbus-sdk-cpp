// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_OPENBUS_PRIVATE_KEY_H_
#define TECGRAF_SDK_OPENBUS_PRIVATE_KEY_H_

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
  PrivateKey();
  PrivateKey(const CORBA::OctetSeq &key);
  PrivateKey(const char *key, std::size_t size);
  explicit PrivateKey(const std::string &filename);

  CORBA::OctetSeq decrypt(const unsigned char *data, std::size_t len) const;

  CORBA::OctetSeq pubKey();
  const CORBA::OctetSeq &octetSeq() const
  {
    return _keySeq;
  }
private:
  openssl::pkey _key;
  CORBA::OctetSeq _keySeq;
};
}
#endif
