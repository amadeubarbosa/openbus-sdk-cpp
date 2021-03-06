// -*- coding: iso-8859-1-unix -*-
#include "openbus/detail/openssl/public_key.hpp"

namespace openbus
{

PublicKey::PublicKey(const idl::core::OctetSeq &key)
  : _keySeq(key)
{
  _pkey = openssl::byteSeq2PubKey(_keySeq.get_buffer(), _keySeq.length());
}

PublicKey::PublicKey(const openssl::pkey &key)
  : _pkey(key)
{
}

PublicKey::PublicKey(const PublicKey &o)
{
  boost::lock_guard<boost::mutex> lock(o._mutex);
  _pkey = o._pkey;
  _keySeq = o._keySeq;
}

PublicKey& PublicKey::operator=(const PublicKey &o)
{
  if (this == &o)
  {
    return *this;
  }
  boost::lock_guard<boost::mutex> l1(&_mutex < &o._mutex ? _mutex : o._mutex);
  boost::lock_guard<boost::mutex> l2(&_mutex > &o._mutex ? _mutex : o._mutex);
  _pkey = o._pkey;
  _keySeq = o._keySeq;
  return *this;
}


idl::core::OctetSeq PublicKey::encrypt(const unsigned char *buf, std::size_t size)
{
  boost::lock_guard<boost::mutex> lock(_mutex);
  size_t encryptedLen;
  openssl::pkey_ctx ctx (EVP_PKEY_CTX_new(_pkey.get(), 0));
  assert(!!ctx);
  int r(EVP_PKEY_encrypt_init(ctx.get()));
  assert(r == 1);
  r = EVP_PKEY_encrypt(ctx.get(), 0, &encryptedLen, buf, size);
  assert(r == 1);
  openssl::openssl_buffer encrypted(idl::core::OctetSeq::allocbuf(
                                      static_cast<CORBA::ULong>(encryptedLen)));
  encrypted.deleter(idl::core::OctetSeq::freebuf);
  if(encrypted.get() == 0)
  if(!encrypted)
  {
    throw std::bad_alloc();
  }
  r = EVP_PKEY_encrypt(ctx.get(), encrypted.get(), &encryptedLen, buf, size);
  assert(r == 1);
  return idl::core::OctetSeq(
    static_cast<CORBA::ULong>(encryptedLen),
    static_cast<CORBA::ULong>(encryptedLen),
    encrypted.release(),
    true);
}

bool PublicKey::verify(const unsigned char *sig, std::size_t siglen, 
                       const unsigned char *tbs, std::size_t tbslen)
{
  boost::lock_guard<boost::mutex> lock(_mutex);
  openssl::pkey_ctx ctx (EVP_PKEY_CTX_new(_pkey.get(), 0));
  assert(!!ctx);
  int r(EVP_PKEY_verify_init(ctx.get()));
  if (r != 1)
  {
    return false;
  }
  return (EVP_PKEY_verify(ctx.get(), sig, siglen, tbs, tbslen) == 1);
}

}
