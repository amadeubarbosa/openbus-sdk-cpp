#include <cassert>

#include "openbus/util/OpenSSL.h"

namespace openbus {
namespace openssl {
    
EVP_PKEY *byteSeq2PubKey(const unsigned char *buf, size_t len) {
  EVP_PKEY *key = 0;
  key = d2i_PUBKEY(0, &buf, len);
  assert(key);
  return key;
}

CORBA::OctetSeq_var PubKey2byteSeq(EVP_PKEY *key) {
  unsigned char *buf = 0;
  size_t len = i2d_PUBKEY(key, &buf);
  assert(len > 0);
  CORBA::OctetSeq_var seq = new CORBA::OctetSeq(len, len, buf);
  return seq._retn();
}

EVP_PKEY *byteSeq2PrvKey(const unsigned char *buf, size_t len) {
  EVP_PKEY *key = 0;
  key = d2i_AutoPrivateKey(0, &buf, len);
  assert(key);
  return key;  
}

CORBA::OctetSeq_var PrvKey2byteSeq(EVP_PKEY *key) {
  unsigned char *buf = 0;
  size_t len = i2d_PrivateKey(key, &buf);
  assert(len > 0);
  CORBA::OctetSeq_var seq = new CORBA::OctetSeq(len, len, buf);
  return seq._retn();
}

CORBA::OctetSeq_var encrypt(EVP_PKEY *key, const unsigned char *buf, size_t len) {
  EVP_PKEY_CTX *ctx;
  unsigned char *encrypted = 0;
  size_t encryptedLen;
  if (!((ctx = EVP_PKEY_CTX_new(key, 0)) && (EVP_PKEY_encrypt_init(ctx) > 0) 
     && (EVP_PKEY_encrypt(ctx, 0, &encryptedLen, buf, len) > 0))) assert(0);
  
  encrypted = (unsigned char*) OPENSSL_malloc(encryptedLen);
  assert(encrypted);
  
  if (EVP_PKEY_encrypt(ctx, encrypted, &encryptedLen, buf, len) <= 0) assert(0);
  EVP_PKEY_CTX_free(ctx);
  CORBA::OctetSeq_var seq = new CORBA::OctetSeq(encryptedLen, encryptedLen, encrypted);
  OPENSSL_free(encrypted);
  return seq._retn();
}

CORBA::OctetSeq_var decrypt(EVP_PKEY *key, const unsigned char *buf, size_t len) {
  EVP_PKEY_CTX *ctx;
  unsigned char *secret = 0;
  size_t secretLen;
  ctx = EVP_PKEY_CTX_new(key, 0);
  if (ctx) {
    if (!((EVP_PKEY_decrypt_init(ctx) > 0) &&(EVP_PKEY_decrypt(ctx, 0, &secretLen, buf, len) > 0))){
      EVP_PKEY_CTX_free(ctx);
      return 0;
    }
    secret = (unsigned char*) OPENSSL_malloc(secretLen);
    assert(secret);
    if (EVP_PKEY_decrypt(ctx, secret, &secretLen, buf, len) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return 0;
    }
    secret[secretLen] = '\0';
    EVP_PKEY_CTX_free(ctx);
    CORBA::OctetSeq_var seq = new CORBA::OctetSeq(len, len, secret);
    return seq._retn();
  } return 0;
}

}
}
