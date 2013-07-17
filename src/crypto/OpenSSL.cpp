// -*- coding: iso-8859-1-unix-*-
#include "openbus/crypto/OpenSSL.hpp"
#include "openbus/crypto/PrivateKey.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>

namespace openbus 
{
namespace openssl 
{

pkey byteSeq2PubKey(const unsigned char *buf, size_t len) {
  pkey key (d2i_PUBKEY(0, &buf, len));
  if(!key)
  {
    throw InvalidPrivateKey();
  }
  return pkey(key);
}

CORBA::OctetSeq PubKey2byteSeq(pkey key) 
{
  unsigned char *buf = 0;
  size_t len = i2d_PUBKEY(key.get(), &buf);
  if(len <= 0)
  {
    throw InvalidPrivateKey();
  }
  return CORBA::OctetSeq (len, len, buf);
}

pkey byteSeq2PrvKey(const unsigned char *buf, size_t len) 
{
  pkey key ( d2i_AutoPrivateKey(0, &buf, len) );
  if(!key)
  {
    throw InvalidPrivateKey();
  }
  return pkey(key);  
}

CORBA::OctetSeq PrvKey2byteSeq(pkey key) 
{
  unsigned char *buf = 0;
  size_t len = i2d_PrivateKey(key.get(), &buf);
  if(len <= 0)
  {
    throw InvalidPrivateKey();
  }
  return CORBA::OctetSeq (len, len, buf);
}

CORBA::OctetSeq encrypt(pkey key, const unsigned char *buf, size_t len) 
{
  size_t encryptedLen;
  pkey_ctx ctx ( EVP_PKEY_CTX_new(key.get(), 0) );
  if(!ctx)
  {
    throw InvalidPrivateKey();
  }

  int r = EVP_PKEY_encrypt_init(ctx.get());
  if(r < 1)
  {
    throw InvalidPrivateKey();
  }

  r = EVP_PKEY_encrypt(ctx.get(), 0, &encryptedLen, buf, len);
  if(r < 1)
  {
    throw InvalidPrivateKey();
  }

  openssl_buffer encrypted ((unsigned char*) OPENSSL_malloc(encryptedLen));
  if(!encrypted)
  {
    throw std::bad_alloc();
  }
  
  r = EVP_PKEY_encrypt(ctx.get(), encrypted.get(), &encryptedLen, buf, len);
  if(r < 1)
  {
    throw InvalidPrivateKey();
  }

  return CORBA::OctetSeq (encryptedLen, encryptedLen, encrypted.get());
}

CORBA::OctetSeq decrypt(pkey key, const unsigned char *buf, size_t len) 
{
  size_t secretLen;
  pkey_ctx ctx (EVP_PKEY_CTX_new(key.get(), 0));
  if (ctx)
  {
    if (EVP_PKEY_decrypt_init(ctx.get()) <= 0)
    {
      throw InvalidPrivateKey();
    }

    if (EVP_PKEY_decrypt(ctx.get(), 0, &secretLen , buf, len) <= 0)
    {
      throw InvalidPrivateKey();
    }

    openssl_buffer secret ((unsigned char*) OPENSSL_malloc(secretLen));
    if(!secret)
    {
      throw std::bad_alloc();
    }

    if (EVP_PKEY_decrypt(ctx.get(), secret.get(), &secretLen , buf, len) <= 0)
    {
      throw InvalidPrivateKey();
    }
    CORBA::OctetSeq seq (secretLen, secretLen, secret.get());
    return seq;
  }
  return 0;
}

}
}
