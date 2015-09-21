// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_OPENSSL_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_OPENSSL_HPP

#include "openbus/detail/decl.hpp"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <tao/ORB.h>
#pragma clang diagnostic pop
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace openbus 
{
namespace openssl 
{

struct pkey_ctx
{
  explicit pkey_ctx(EVP_PKEY_CTX *ctx) : ctx(ctx) 
  {
  }

  pkey_ctx() : ctx(0) 
  {
  }

  ~pkey_ctx()
  {
    if(ctx)
    {
      EVP_PKEY_CTX_free(ctx);
    }
  }

  EVP_PKEY_CTX *get() const 
  { 
    return ctx; 
  }

  bool is_empty() const 
  { 
    return ctx == 0; 
  }

  typedef bool(pkey_ctx::*unspecified_bool_type)() const;

  operator unspecified_bool_type() const
  {
    unspecified_bool_type n = 0;
    return is_empty()?n:&pkey_ctx::is_empty;
  }

  bool operator!() const 
  { 
    return is_empty(); 
  }
private:
  EVP_PKEY_CTX *ctx;
};

struct openssl_buffer
{
  explicit openssl_buffer(unsigned char *buffer)
    : buffer(buffer), deleter_(CRYPTO_free)
  {
  }

  ~openssl_buffer()
  {
    if(buffer)
    {
      deleter_(buffer);
    }
  }

  void deleter(boost::function<void (unsigned char *)> p)
  {
    deleter_ = p;
  }

  unsigned char &operator[](std::size_t s)
  {
    return buffer[s];
  }

  unsigned char const &operator[](std::size_t s) const
  {
    return buffer[s];
  }

  unsigned char *release()
  {
    unsigned char *tmp = buffer;
    buffer = 0;
    return tmp; 
  }

  bool is_empty() const 
  { 
    return buffer == 0; 
  }

  typedef bool(openssl_buffer::* unspecified_bool_type)() const;

  operator unspecified_bool_type () const
  {
    unspecified_bool_type n = 0;
    return is_empty()? n : &openssl_buffer::is_empty;
  }

  unsigned char *get() const 
  { 
    return buffer; 
  }
private:
  unsigned char *buffer;
  boost::function<void (unsigned char *)> deleter_;
};

struct pkey
{
  explicit pkey(EVP_PKEY *key) : key(key, & ::EVP_PKEY_free)
  {
  }

  pkey()
  {
  }

  EVP_PKEY *get() const 
  { 
    return key.get(); 
  }

  bool is_empty() const
  {
    return !key;
  }

  typedef bool(pkey::* unspecified_bool_type)() const;

  operator unspecified_bool_type () const
  {
    unspecified_bool_type n = 0;
    return is_empty()?n:&pkey::is_empty;
  }

  bool operator!() const 
  { 
    return is_empty(); 
  }
private:
  boost::shared_ptr<EVP_PKEY> key;
};

struct pX509
{
  explicit pX509(X509 *crt) : crt(crt, & ::X509_free)
  {
  }

  pX509()
  {
  }

  X509 *get() const
  {
    return crt.get();
  }

  bool is_empty() const
  {
    return !crt;
  }

  typedef bool(pX509::* unspecified_bool_type)() const;

  operator unspecified_bool_type () const
  {
    unspecified_bool_type n = 0;
    return is_empty()?n:&pX509::is_empty;
  }

  bool operator!() const 
  { 
    return is_empty(); 
  }
private:
  boost::shared_ptr<X509> crt;
};
  
OPENBUS_SDK_DECL pkey byteSeq2PubKey(const unsigned char *, size_t len);
OPENBUS_SDK_DECL pX509 byteSeq2x509(const unsigned char *, size_t len);
OPENBUS_SDK_DECL pkey byteSeq2PrvKey(const unsigned char *, size_t len);
OPENBUS_SDK_DECL CORBA::OctetSeq PubKey2byteSeq(pkey);
OPENBUS_SDK_DECL CORBA::OctetSeq PrvKey2byteSeq(pkey);
OPENBUS_SDK_DECL CORBA::OctetSeq encrypt(pkey, const unsigned char *, size_t len);
OPENBUS_SDK_DECL CORBA::OctetSeq decrypt(pkey, const unsigned char *, size_t len);
}
}

#endif
