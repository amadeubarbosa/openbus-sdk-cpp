// -*- coding: iso-8859-1 -*-

#include "openbus/util/PublicKey.hpp"

namespace openbus
{

PublicKey::PublicKey(const CORBA::OctetSeq &key)
  : _keySeq(key)
{
  _pkey = openssl::byteSeq2PubKey(_keySeq.get_buffer(), _keySeq.length());
}

CORBA::OctetSeq PublicKey::encrypt(const unsigned char *buf, std::size_t size)
{
  size_t encryptedLen;
  openssl::pkey_ctx ctx (EVP_PKEY_CTX_new(_pkey.get(), 0));
  assert(!!ctx);
  int r = EVP_PKEY_encrypt_init(ctx.get());
  assert(r == 1);
  r = EVP_PKEY_encrypt(ctx.get(), 0, &encryptedLen, buf, size);
  assert(r == 1);
  openssl::openssl_buffer encrypted ((unsigned char*) 
                                     OPENSSL_malloc(encryptedLen));
  if(!encrypted)
  {
    throw std::bad_alloc();
  }
  r = EVP_PKEY_encrypt(ctx.get(), encrypted.get(), &encryptedLen, buf, size);
  assert(r == 1);
  return CORBA::OctetSeq (encryptedLen, encryptedLen, encrypted.get());
}

bool PublicKey::verify(const unsigned char *sig, std::size_t siglen, 
                       const unsigned char *tbs, std::size_t tbslen)
{
  openssl::pkey_ctx ctx (EVP_PKEY_CTX_new(_pkey.get(), 0));
  assert(!!ctx);
  int r = EVP_PKEY_verify_init(ctx.get());
  assert(r == 1);
  return EVP_PKEY_verify(ctx.get(), sig, siglen, tbs, tbslen);
}

}
