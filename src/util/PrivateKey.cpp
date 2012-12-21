// -*- coding: iso-8859-1 -*-
#include "openbus/Connection.h"
#include "openbus/util/PrivateKey.h"

#include <fstream>

namespace openbus
{
//[TODO]
PrivateKey::PrivateKey()
{
}

//[TODO]
PrivateKey::PrivateKey(const char *key)
{
}

PrivateKey::PrivateKey(const std::string filename)
{
  std::ifstream key(filename.c_str(), std::fstream::binary);
  if (!key)
  {
    throw openbus::InvalidPrivateKey();
  }
  key.seekg(0, std::ios::end);
  const std::size_t size = key.tellg();
  key.seekg(0, std::ios::beg);
  _keySeq.length(size);
  key.read(static_cast<char *> (static_cast<void *> (_keySeq.get_buffer())), size);
}
}
