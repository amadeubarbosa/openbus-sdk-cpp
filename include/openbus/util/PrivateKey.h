// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_PRIVATE_KEY_H_
#define TECGRAF_SDK_PRIVATE_KEY_H_

#include "stubs/core.h"

#include <string>

namespace openbus
{
class PrivateKey
{
public:
  PrivateKey();
  PrivateKey(const char *key);
  PrivateKey(const std::string filename);
  
  const CORBA::OctetSeq &octetSeq() const
  {
    return _keySeq;
  }
private:
  CORBA::OctetSeq _keySeq;
};
}
#endif
