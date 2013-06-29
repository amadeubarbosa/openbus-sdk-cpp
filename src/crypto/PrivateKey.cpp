// -*- coding: iso-8859-1 -*-
#include "openbus/crypto/PrivateKey.hpp"

#include <fstream>

namespace openbus
{

const std::size_t RSASize = 2048;

void PrivateKey::set_pkey(const CORBA::OctetSeq &key)
{
  const unsigned char *buf = key.get_buffer();
  _key = openssl::pkey(d2i_AutoPrivateKey(0, &buf, key.length()));
  assert(_key.get());
}  

PrivateKey::PrivateKey()
{
  openssl::pkey_ctx ctx (EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, 0));
  assert(ctx.get());
  int r = EVP_PKEY_keygen_init(ctx.get());
  assert(r == 1);
  r = EVP_PKEY_CTX_set_rsa_keygen_bits(ctx.get(), RSASize);
  assert(r == 1);
  EVP_PKEY *key = 0;
  r = EVP_PKEY_keygen(ctx.get(), &key);
  assert((r == 1) && key);
  _key = openssl::pkey(key);
  unsigned char* buf = 0;
  std::size_t len = i2d_PrivateKey(_key.get(), &buf);
  assert(len > 0);
  _keySeq = CORBA::OctetSeq(len, len, buf);
}

PrivateKey::PrivateKey(const CORBA::OctetSeq &key) : _keySeq(key)
{
  set_pkey(_keySeq);
}

PrivateKey::PrivateKey(const char *key, std::size_t size)
{
  _keySeq.length(size);
  std::memcpy(_keySeq.get_buffer(), key, size);
  set_pkey(_keySeq);
}
 
PrivateKey::PrivateKey(const std::string &filename)
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
  key.rdbuf()->sgetn(static_cast<char *> (static_cast<void *> 
                                          (_keySeq.get_buffer())), size);
  const unsigned char *buf = _keySeq.get_buffer();
  _key = openssl::pkey(d2i_AutoPrivateKey(0, &buf, size));
  assert(_key.get());
}

PrivateKey::PrivateKey(const PrivateKey &o)
{
#ifdef OPENUBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> _lock lock(o.mutex);
#endif
  _key = o._key;
  _keySeq = o._keySeq;
}

PrivateKey& PrivateKey::operator=(const PrivateKey &o)
{
  if (this == &o)
  {
    return *this;
  }
#ifdef OPENUBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> l1(&_mutex < &o._mutex ? _mutex : o._mutex);
  boost::lock_guard<boost::mutex> l2(&_mutex > &o._mutex ? _mutex : o._mutex);
#endif
  _key = o._key;
  _keySeq = o._keySeq;
  return *this;
}


CORBA::OctetSeq PrivateKey::pubKey()
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);
#endif
  unsigned char *buf = 0;
  std::size_t len = i2d_PUBKEY(_key.get(), &buf);
  assert(len > 0);
  return CORBA::OctetSeq (len, len, buf);
}

CORBA::OctetSeq PrivateKey::decrypt(const unsigned char *data, 
                                    std::size_t len) const
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);
#endif
  size_t secretLen;
  openssl::pkey_ctx ctx (EVP_PKEY_CTX_new(_key.get(), 0));
  assert(ctx.get());
  if (EVP_PKEY_decrypt_init(ctx.get()) <= 0)
  {
    throw InvalidPrivateKey();
  }

  if (EVP_PKEY_decrypt(ctx.get(), 0, &secretLen , data, len) <= 0)
  {
    throw InvalidPrivateKey();
  }
  
  openssl::openssl_buffer secret ((unsigned char*) OPENSSL_malloc(secretLen));
  if(!secret)
  {
    throw std::bad_alloc();
  }

  if (EVP_PKEY_decrypt(ctx.get(), secret.get(), &secretLen , data, len) <= 0)
  {
    throw InvalidPrivateKey();
  }
  CORBA::OctetSeq seq (secretLen, secretLen, secret.get());
  return seq;
}

}
