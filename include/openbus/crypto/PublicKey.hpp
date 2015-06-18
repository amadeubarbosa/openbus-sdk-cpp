// -*- coding: iso-8859-1-unix -*-
#ifndef TECGRAF_SDK_OPENBUS_PUBLIC_KEY_H
#define TECGRAF_SDK_OPENBUS_PUBLIC_KEY_H

#include "openbus/decl.hpp"
#include "openbus/crypto/OpenSSL.hpp"
#include "openbus_core-2.1C.h"
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <cstddef>

namespace openbus
{
namespace idl = tecgraf::openbus::core::v2_1;
struct InvalidPublicKey : public std::exception
{ 
  const char *what() const throw() 
  { 
    return "openbus::InvalidPublicKey";
  } 
};

class PublicKey
{
public:
  PublicKey(const idl::OctetSeq &key);
  PublicKey(const openssl::pkey &key);
  PublicKey(const PublicKey&);
  PublicKey& operator=(const PublicKey &);
  idl::OctetSeq encrypt(const unsigned char *, std::size_t);
  bool verify(const unsigned char *sig, std::size_t siglen, 
              const unsigned char *tbs, std::size_t tbslen);
  const idl::OctetSeq &octetSeq() const
  {
    return _keySeq;
  }
private:
  openssl::pkey _pkey;
  idl::OctetSeq _keySeq;
#ifdef OPENBUS_SDK_MULTITHREAD
  mutable boost::mutex _mutex;
#endif
};

}
#endif
