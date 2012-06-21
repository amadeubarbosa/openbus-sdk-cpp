#ifndef _TECGRAF_OPENSSL_H_
#define _TECGRAF_OPENSSL_H_

#include <openssl/pem.h>

namespace openbus {
namespace openssl {
  EVP_PKEY* byteSeq2PubKey(const unsigned char*, size_t len);
  EVP_PKEY* byteSeq2PrvKey(const unsigned char*, size_t len);
  unsigned char* PubKey2byteSeq(EVP_PKEY*, size_t& len);
  unsigned char* PrvKey2byteSeq(EVP_PKEY* key, size_t& len);
  unsigned char* encrypt(EVP_PKEY*, const unsigned char*, size_t l);
  unsigned char* decrypt(EVP_PKEY*, const unsigned char*, size_t l);
}  
}

#endif
