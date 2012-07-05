#ifndef _TECGRAF_OPENSSL_H_
#define _TECGRAF_OPENSSL_H_

#include <openssl/pem.h>
#include "stubs/core.h"

namespace openbus {
namespace openssl {
  EVP_PKEY *byteSeq2PubKey(const unsigned char *, size_t len);
  EVP_PKEY *byteSeq2PrvKey(const unsigned char *, size_t len);
  CORBA::OctetSeq_var PubKey2byteSeq(EVP_PKEY *);
  CORBA::OctetSeq_var PrvKey2byteSeq(EVP_PKEY *);
  CORBA::OctetSeq_var encrypt(EVP_PKEY *, const unsigned char *, size_t len);
  CORBA::OctetSeq_var decrypt(EVP_PKEY *, const unsigned char *, size_t len);
}  
}

#endif
