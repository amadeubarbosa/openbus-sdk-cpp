// -*- coding: iso-8859-1-unix -*-
#include "openbus/detail/openssl/PrivateKey.hpp"

#include <fstream>

namespace openbus
{

const std::size_t RSASize(2048);

void PrivateKey::set_pkey(const idl::OctetSeq &key)
{
  const unsigned char *buf(key.get_buffer());
  _key = openssl::pkey(d2i_AutoPrivateKey(0, &buf, key.length()));
  assert(_key.get());
}  

EVP_PKEY * PrivateKey::new_key()
{
  openssl::pkey_ctx ctx (EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, 0));
  assert(ctx.get());
  int r(EVP_PKEY_keygen_init(ctx.get()));
  assert(r == 1);
  r = EVP_PKEY_CTX_set_rsa_keygen_bits(ctx.get(), RSASize);
  assert(r == 1);
  EVP_PKEY *key = 0;
  r = EVP_PKEY_keygen(ctx.get(), &key);
  assert((r == 1) && key);
  return key;
}

PrivateKey::PrivateKey()
{
  _key = openssl::pkey(new_key());
  std::size_t buf_size(i2d_PrivateKey(_key.get(), 0));
  openssl::openssl_buffer buf(idl::OctetSeq::allocbuf(
                                static_cast<CORBA::ULong>(buf_size)));
  buf.deleter(idl::OctetSeq::freebuf);
  unsigned char *p(buf.get());
  size_t len(i2d_PrivateKey(_key.get(), &p));
  _keySeq = idl::OctetSeq(
    static_cast<CORBA::ULong>(len),
    static_cast<CORBA::ULong>(len),
    buf.release(), true);
}

PrivateKey::PrivateKey(EVP_PKEY *key)
{
  std::cout << "key: " << key << std::endl;
  if (key == 0)
  {
    key = new_key();
  }
  CRYPTO_add(&key->references, 1, CRYPTO_LOCK_EVP_PKEY);
  _key = openssl::pkey(key);
  std::size_t buf_size(i2d_PrivateKey(key, 0));
  openssl::openssl_buffer buf(idl::OctetSeq::allocbuf(
                                static_cast<CORBA::ULong>(buf_size)));
  buf.deleter(idl::OctetSeq::freebuf);
  unsigned char *p(buf.get());
  size_t len(i2d_PrivateKey(key, &p));
  _keySeq = idl::OctetSeq(
    static_cast<CORBA::ULong>(len),
    static_cast<CORBA::ULong>(len),
    buf.release(), true);
}

PrivateKey::PrivateKey(const idl::OctetSeq &key) : _keySeq(key)
{
  set_pkey(_keySeq);
}

PrivateKey::PrivateKey(const char *key, std::size_t size)
{
  _keySeq.length(static_cast<CORBA::ULong>(size));
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
  const std::size_t size(key.tellg());
  key.seekg(0, std::ios::beg);
  _keySeq.length(static_cast<CORBA::ULong>(size));
  key.rdbuf()->sgetn(static_cast<char *> (static_cast<void *> 
                                          (_keySeq.get_buffer())), size);
  const unsigned char *buf(_keySeq.get_buffer());
  _key = openssl::pkey(d2i_AutoPrivateKey(
                         0, &buf, static_cast<long>(size)));
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

PrivateKey &PrivateKey::operator=(const PrivateKey &o)
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


idl::OctetSeq PrivateKey::pubKey()
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);
#endif
  size_t buf_size(i2d_PUBKEY(_key.get(), 0));
  openssl::openssl_buffer buf(idl::OctetSeq::allocbuf(
                                static_cast<CORBA::ULong>(buf_size)));
  buf.deleter(idl::OctetSeq::freebuf);
  unsigned char *p(buf.get());
  size_t len(i2d_PUBKEY(_key.get(), &p));
  return idl::OctetSeq(
    static_cast<CORBA::ULong>(len),
    static_cast<CORBA::ULong>(len),
    buf.release(),
    true);
}

idl::OctetSeq PrivateKey::decrypt(const unsigned char *data,
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
  
  openssl::openssl_buffer secret(idl::OctetSeq::allocbuf(
                                   static_cast<CORBA::ULong>(secretLen)));
  secret.deleter(idl::OctetSeq::freebuf);
  if(!secret.get())
  {
    throw std::bad_alloc();
  }

  if (EVP_PKEY_decrypt(ctx.get(), secret.get(), &secretLen , data, len) <= 0)
  {
    throw InvalidPrivateKey();
  }
  return idl::OctetSeq(
    static_cast<CORBA::ULong>(secretLen),
    static_cast<CORBA::ULong>(secretLen),
    secret.release(),
    true);
}

}
