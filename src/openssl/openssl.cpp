// -*- coding: iso-8859-1-unix -*-
#include "openbus/detail/openssl/openssl.hpp"
#include "openbus/detail/openssl/private_key.hpp"
#include "openbus/detail/openssl/public_key.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>

namespace openbus 
{
namespace openssl 
{

pkey byteSeq2PubKey(const unsigned char *buf, size_t len) {
  pkey key (d2i_PUBKEY(0, &buf, static_cast<long>(len)));
  if(!key)
  {
    throw InvalidPublicKey();
  }
  return pkey(key);
}

template <class E>
CORBA::OctetSeq key2byteSeq(pkey key)
{
  size_t buf_size(i2d_PUBKEY(key.get(), 0));
  openssl_buffer buf(CORBA::OctetSeq::allocbuf(
                       static_cast<CORBA::ULong>(buf_size)));
  buf.deleter(CORBA::OctetSeq::freebuf);
  unsigned char *p(buf.get());
  size_t len = i2d_PUBKEY(key.get(), &p);
  if(len <= 0)
  {
    throw E();
  }
  return CORBA::OctetSeq(
    static_cast<CORBA::ULong>(len),
    static_cast<CORBA::ULong>(len),
    buf.release(), true);  
}

CORBA::OctetSeq PubKey2byteSeq(pkey key) 
{
  return key2byteSeq<InvalidPublicKey>(key);
}

pX509 byteSeq2x509(const unsigned char *buf, size_t len)
{
  pX509 crt(d2i_X509(0, &buf, static_cast<long>(len)));
  if (!crt)
  {
    throw InvalidPublicKey(); //?
  }
  return pX509(crt);
}

pkey byteSeq2PrvKey(const unsigned char *buf, size_t len) 
{
  pkey key ( d2i_AutoPrivateKey(0, &buf, static_cast<long>(len)) );
  if(!key)
  {
    throw InvalidPrivateKey();
  }
  return pkey(key);  
}

CORBA::OctetSeq PrvKey2byteSeq(pkey key) 
{
  return key2byteSeq<InvalidPrivateKey>(key);
}

CORBA::OctetSeq encrypt(pkey key, const unsigned char *buf, size_t len) 
{
  size_t encryptedLen;
  pkey_ctx ctx ( EVP_PKEY_CTX_new(key.get(), 0) );
  if(!ctx)
  {
    throw InvalidPrivateKey();
  }

  int r(EVP_PKEY_encrypt_init(ctx.get()));
  if(r < 1)
  {
    throw InvalidPrivateKey();
  }

  r = EVP_PKEY_encrypt(ctx.get(), 0, &encryptedLen, buf, len);
  if(r < 1)
  {
    throw InvalidPrivateKey();
  }

  openssl_buffer encrypted(CORBA::OctetSeq::allocbuf(
                             static_cast<CORBA::ULong>(encryptedLen)));
  encrypted.deleter(CORBA::OctetSeq::freebuf);
  if(!encrypted)
  {
    throw std::bad_alloc();
  }
  
  r = EVP_PKEY_encrypt(ctx.get(), encrypted.get(), &encryptedLen, buf, len);
  if(r < 1)
  {
    throw InvalidPrivateKey();
  }

  return CORBA::OctetSeq(
    static_cast<CORBA::ULong>(encryptedLen),
    static_cast<CORBA::ULong>(encryptedLen),
    encrypted.release(), true);
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

    openssl_buffer secret(CORBA::OctetSeq::allocbuf(
                            static_cast<CORBA::ULong>(secretLen)));
    secret.deleter(CORBA::OctetSeq::freebuf);
    if(!secret)
    {
      throw std::bad_alloc();
    }

    if (EVP_PKEY_decrypt(ctx.get(), secret.get(), &secretLen , buf, len) <= 0)
    {
      throw InvalidPrivateKey();
    }
    CORBA::OctetSeq seq(
      static_cast<CORBA::ULong>(secretLen),
      static_cast<CORBA::ULong>(secretLen),
      secret.release(), true);
    return seq;
  }
  return 0;
}

}
}
