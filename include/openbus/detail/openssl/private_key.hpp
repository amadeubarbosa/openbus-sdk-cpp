// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_PRIVATE_KEY_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_PRIVATE_KEY_HPP

#include "openbus/idl.hpp"
#include "openbus/detail/decl.hpp"
#include "openbus/detail/openssl/openssl.hpp"

#include <string>
#include <exception>
#include <cstring>
#include <cstddef>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif

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
  OPENBUS_SDK_DECL PrivateKey(EVP_PKEY *);
  OPENBUS_SDK_DECL PrivateKey(const idl::core::OctetSeq &key);
  OPENBUS_SDK_DECL PrivateKey(const char *key, std::size_t size);
  OPENBUS_SDK_DECL explicit PrivateKey(const std::string &filename);
  OPENBUS_SDK_DECL PrivateKey(const PrivateKey &);
  OPENBUS_SDK_DECL PrivateKey& operator=(const PrivateKey &);
  OPENBUS_SDK_DECL idl::core::OctetSeq decrypt(const unsigned char *data,
                                         std::size_t len) const;
  OPENBUS_SDK_DECL idl::core::OctetSeq pubKey();
  const idl::core::OctetSeq &octetSeq() const
  {
    return _keySeq;
  }
  EVP_PKEY *key() const
  {
    return _key.get();
  }
private:
  void set_pkey(const idl::core::OctetSeq &key);
  EVP_PKEY *new_key();
  openssl::pkey _key;
  idl::core::OctetSeq _keySeq;
#ifdef OPENBUS_SDK_MULTITHREAD
  mutable boost::mutex _mutex;
#endif
};
}
#endif
