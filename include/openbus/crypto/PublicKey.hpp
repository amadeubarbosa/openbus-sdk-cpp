// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_OPENBUS_PUBLIC_KEY_H
#define TECGRAF_SDK_OPENBUS_PUBLIC_KEY_H

#include "openbus/decl.hpp"
#include "openbus/crypto/OpenSSL.hpp"
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <cstddef>

namespace openbus
{

class PublicKey
{
public:
  PublicKey(const CORBA::OctetSeq &key);
  PublicKey(const PublicKey&);
  PublicKey& operator=(const PublicKey &);
  CORBA::OctetSeq encrypt(const unsigned char *, std::size_t);
  bool verify(const unsigned char *sig, std::size_t siglen, 
              const unsigned char *tbs, std::size_t tbslen);
  const CORBA::OctetSeq &octetSeq() const
  {
    return _keySeq;
  }
private:
  openssl::pkey _pkey;
  CORBA::OctetSeq _keySeq;
#ifdef OPENBUS_SDK_MULTITHREAD
  mutable boost::mutex _mutex;
#endif
};

}
#endif
