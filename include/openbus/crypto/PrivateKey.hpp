// -*- coding: iso-8859-1-unix -*-
#ifndef TECGRAF_SDK_OPENBUS_PRIVATE_KEY_H_
#define TECGRAF_SDK_OPENBUS_PRIVATE_KEY_H_

#include "openbus/decl.hpp"
#include "openbus/crypto/OpenSSL.hpp"
#include "openbus_core-2.1C.h"

#include <string>
#include <exception>
#include <cstring>
#include <cstddef>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif

namespace openbus
{
namespace idl = tecgraf::openbus::core::v2_1;
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
  OPENBUS_SDK_DECL PrivateKey(const idl::OctetSeq &key);
  OPENBUS_SDK_DECL PrivateKey(const char *key, std::size_t size);
  OPENBUS_SDK_DECL explicit PrivateKey(const std::string &filename);
  OPENBUS_SDK_DECL PrivateKey(const PrivateKey &);
  OPENBUS_SDK_DECL PrivateKey& operator=(const PrivateKey &);
  OPENBUS_SDK_DECL idl::OctetSeq decrypt(const unsigned char *data,
                                         std::size_t len) const;
  OPENBUS_SDK_DECL idl::OctetSeq pubKey();
  const idl::OctetSeq &octetSeq() const
  {
    return _keySeq;
  }
private:
  void set_pkey(const idl::OctetSeq &key);
  openssl::pkey _key;
  idl::OctetSeq _keySeq;
#ifdef OPENBUS_SDK_MULTITHREAD
  mutable boost::mutex _mutex;
#endif
};
}
#endif
