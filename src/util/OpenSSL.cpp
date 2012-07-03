#include <cassert>

#include "openbus/util/OpenSSL.h"

namespace openbus {
namespace openssl {
    
EVP_PKEY* byteSeq2PubKey(const unsigned char* buf, size_t len) {
  EVP_PKEY* key = 0;
  key = d2i_PUBKEY(0, &buf, len);
  assert(key);
  return key;
}

unsigned char* PubKey2byteSeq(EVP_PKEY* key, size_t& len) {
  unsigned char* buf = 0;
  len = i2d_PUBKEY(key, &buf);
  assert(len > 0);
  return buf;
}

EVP_PKEY* byteSeq2PrvKey(const unsigned char* buf, size_t len) {
  EVP_PKEY* key = 0;
  key = d2i_AutoPrivateKey(0, &buf, len);
  assert(key);
  return key;  
}

unsigned char* PrvKey2byteSeq(EVP_PKEY* key, size_t& len) {
  unsigned char* buf = 0;
  len = i2d_PrivateKey(key, &buf);
  assert(len > 0);
  return buf;
}

unsigned char* encrypt(EVP_PKEY* key, const unsigned char* buf, size_t l) {
  EVP_PKEY_CTX* ctx;
  unsigned char* encrypted = 0;
  size_t encryptedLen;
  if (!((ctx = EVP_PKEY_CTX_new(key, 0)) && (EVP_PKEY_encrypt_init(ctx) > 0) 
     &&(EVP_PKEY_encrypt(ctx, 0, &encryptedLen, buf, l) > 0))) assert(0);

  encrypted = (unsigned char*) OPENSSL_malloc(encryptedLen);
  assert(encrypted);
  
  if (EVP_PKEY_encrypt(ctx, encrypted, &encryptedLen, buf, l) <= 0) assert(0);
  EVP_PKEY_CTX_free(ctx);
  return encrypted;
}

unsigned char* decrypt(EVP_PKEY* key, const unsigned char* buf, size_t l) {
  EVP_PKEY_CTX* ctx;
  unsigned char* secret = 0;
  size_t secretLen;
  ctx = EVP_PKEY_CTX_new(key, 0);
  if (ctx) {
    if (!((EVP_PKEY_decrypt_init(ctx) > 0) &&(EVP_PKEY_decrypt(ctx, 0, &secretLen, buf, l) > 0))) {
      EVP_PKEY_CTX_free(ctx);
      return 0;
    }
    secret = (unsigned char*) OPENSSL_malloc(secretLen);
    assert(secret);
    if (EVP_PKEY_decrypt(ctx, secret, &secretLen, buf, l) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        return 0;
    }
    secret[secretLen] = '\0';
    EVP_PKEY_CTX_free(ctx);
    return secret;
  } return 0;
}

}
}
