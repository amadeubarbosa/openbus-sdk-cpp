#ifndef _TECGRAF_OPENSSL_H_
#define _TECGRAF_OPENSSL_H_

#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/sha.h>

namespace openbus {
namespace openssl {
  EVP_PKEY* byteSeq2EVPPkey(const unsigned char*, size_t len);
  unsigned char* EVPPkey2byteSeq(EVP_PKEY*, size_t& len);

  unsigned char* encrypt(EVP_PKEY*, const unsigned char*, size_t l);
  unsigned char* decrypt(EVP_PKEY*, const unsigned char*, size_t l);
}  
}

#endif
