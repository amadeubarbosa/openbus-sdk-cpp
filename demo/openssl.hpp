// -*- condig: iso-8859-1-unix

#ifndef TECGRAF_SDK_OPENBUS_DEMO_OPENSSL_HPP
#define TECGRAF_SDK_OPENBUS_DEMO_OPENSSL_HPP

#include <openssl/evp.h>
#include <fstream>
#include <string>

namespace openbus { namespace demo { namespace openssl {

EVP_PKEY * read_priv_key(const std::string &filename)
{
  std::ifstream key(filename.c_str(), std::fstream::binary);
  if (!key)
  {
    return 0;
  }
  key.seekg(0, std::ios::end);
  const std::size_t size(key.tellg());
  key.seekg(0, std::ios::beg);
  unsigned char *buf(new unsigned char[size]);
  key.rdbuf()->sgetn(reinterpret_cast<char *>(buf), size);
  const unsigned char *cbuf(buf);
  return d2i_AutoPrivateKey(0, &cbuf, static_cast<long>(size));
}

}}}

#endif
