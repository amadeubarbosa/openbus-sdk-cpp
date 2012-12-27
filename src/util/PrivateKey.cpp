// -*- coding: iso-8859-1 -*-
#include "openbus/util/PrivateKey.h"

#include <fstream>

namespace openbus
{

PrivateKey::PrivateKey(std::string const& filename)
{
  std::ifstream key(filename.c_str(), std::fstream::binary);
  if (!key)
  {
    throw InvalidPrivateKey();
  }
  key.seekg(0, std::ios::end);
  const std::size_t size = key.tellg();
  key.seekg(0, std::ios::beg);
  _keySeq.length(size);
  key.rdbuf()->sgetn(static_cast<char *> (static_cast<void *> (_keySeq.get_buffer())), size);
}

}
