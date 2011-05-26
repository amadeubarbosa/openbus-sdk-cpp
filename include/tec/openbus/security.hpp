#ifndef TEC_OPENBUS_SECURITY_HPP
#define TEC_OPENBUS_SECURITY_HPP

#include <tec/openbus/util/sequence_iterator.hpp>

#include <tec/ssl/bio.hpp>
#include <tec/ssl/rsa.hpp>
#include <tec/ssl/sha256.hpp>

#include <CORBA.h>
#include <openssl/pem.h>

#include "access_control_service.h"

namespace tec { namespace openbus {

void initialize_security_by_login(CORBA::ORB_ptr orb, const char* username, const char* password
                                  , tecgraf::openbus::core::v1_06::access_control_service::IAccessControlService_ptr acs);

inline std::pair<ssl::rsa, std::vector<CORBA::Octet> > login_by_password
  (tecgraf::openbus::core::v1_06::access_control_service::IAccessControlService_var acs
   , std::string const& username, std::string const& password)
{
  tecgraf::openbus::core::v1_06::access_control_service::Token* token = acs->getToken();
  CORBA::Octet const* buffer = token->token_octet.get_buffer();
  ssl::memory_bio bio(buffer, token->token_octet.length());

  if(RSA* p = PEM_read_bio_RSAPublicKey(bio.raw(), 0, 0, 0))
  {
    ssl::rsa server_rsa(p);
    std::vector<unsigned char> encrypted_username;
    std::vector<unsigned char> encrypted_password;
    server_rsa.public_encrypt(username.begin(), username.end(), std::back_inserter(encrypted_username));
    server_rsa.public_encrypt(password.begin(), password.end(), std::back_inserter(encrypted_password));

    ssl::rsa my_key = ssl::rsa::generate_key();

    using tecgraf::openbus::core::v1_06::OctetSeq;
    OctetSeq octet_public_key;
    {
      ssl::memory_bio bio;
      if(!PEM_write_bio_RSAPublicKey(bio.raw(), my_key.raw()))
        throw std::runtime_error("");

      bio.pop();
      octet_public_key.length(bio.length());
      tec::openbus::util::sequence_iterator<OctetSeq>
        first(octet_public_key);
      std::copy(bio.begin(), bio.end(), first);
    }

    OctetSeq octet_username (encrypted_username.size(), encrypted_username.size()
                             , &encrypted_username[0], false);
    OctetSeq octet_password (encrypted_password.size(), encrypted_password.size()
                             , &encrypted_password[0], false);
    OctetSeq* public_key_signature = 0;

    acs->loginByPassword(octet_username, octet_password, octet_public_key, public_key_signature);

    std::vector<unsigned char> sha256;
    ssl::sha256(octet_public_key.get_buffer(), octet_public_key.get_buffer()
                + octet_public_key.length(), std::back_inserter(sha256));
    if(server_rsa.verify(public_key_signature->get_buffer()
                         , public_key_signature->get_buffer() + public_key_signature->length()
                         , sha256.begin(), sha256.end()))
    {
      std::cout << "Received signature" << std::endl;
      std::vector<unsigned char> signature(public_key_signature->get_buffer()
                                           , public_key_signature->get_buffer()
                                           + public_key_signature->length());
      return std::make_pair(my_key, signature);
    }
    else
      throw std::runtime_error("");
  }
  else
    throw std::runtime_error("");
}

} }

#endif
