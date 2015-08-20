// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_PUBLIC_KEY_HPP
#define TECGRAF_SDK_OPENBUS_PUBLIC_KEY_HPP

#include "openbus/idl.hpp"
#include "openbus/detail/decl.hpp"
#include "openbus/detail/openssl/OpenSSL.hpp"
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <cstddef>

namespace openbus
{
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
  PublicKey(const idl::core::OctetSeq &key);
  PublicKey(const openssl::pkey &key);
  PublicKey(const PublicKey&);
  PublicKey& operator=(const PublicKey &);
  idl::core::OctetSeq encrypt(const unsigned char *, std::size_t);
  bool verify(const unsigned char *sig, std::size_t siglen, 
              const unsigned char *tbs, std::size_t tbslen);
  const idl::core::OctetSeq &octetSeq() const
  {
    return _keySeq;
  }
private:
  openssl::pkey _pkey;
  idl::core::OctetSeq _keySeq;
#ifdef OPENBUS_SDK_MULTITHREAD
  mutable boost::mutex _mutex;
#endif
};

}
#endif
