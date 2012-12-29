
// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_PRIVATE_KEY_H_
#define TECGRAF_SDK_PRIVATE_KEY_H_

#include "stubs/core.h"
#include "openbus/util/OpenSSL.hpp"

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
  PrivateKey(CORBA::OctetSeq const& key)
    : _keySeq(key) 
  {
  }
  PrivateKey(const char *key, std::size_t size)
  {
    _keySeq.length(size);
    std::memcpy(_keySeq.get_buffer(), key, size);
  }
  explicit PrivateKey(std::string const& filename);
  
  CORBA::OctetSeq pubKey();
  CORBA::OctetSeq decrypt(const unsigned char* data, std::size_t len) const;
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
