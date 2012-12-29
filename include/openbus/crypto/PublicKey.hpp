// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_PUBLIC_KEY_H
#define TECGRAF_SDK_PUBLIC_KEY_H

#include "openbus/crypto/OpenSSL.hpp"
#include "stubs/core.h"

#include <cstddef>

namespace openbus
{

class PublicKey
{
public:
  PublicKey(const CORBA::OctetSeq &key);
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
};

}
#endif
