// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_PRIVATE_KEY_H_
#define TECGRAF_SDK_PRIVATE_KEY_H_

#include "stubs/core.h"

#include <string>
#include <cstring>
#include <exception>

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
  PrivateKey() {}
  PrivateKey(CORBA::OctetSeq const& key)
    : _keySeq(key) {}
  PrivateKey(const char *key, std::size_t size)
  {
    _keySeq.length(size);
    std::memcpy(_keySeq.get_buffer(), key, size);
  }
  explicit PrivateKey(std::string const& filename);
  
  const CORBA::OctetSeq &octetSeq() const
  {
    return _keySeq;
  }
private:
  CORBA::OctetSeq _keySeq;
};
}
#endif
