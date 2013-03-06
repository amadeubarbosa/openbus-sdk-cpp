#ifndef OPENBUS_CPP_INTEROP_DELEGATION_SERVER_LOGIN_H
#define OPENBUS_CPP_INTEROP_DELEGATION_SERVER_LOGIN_H

#include "openbus/Connection.hpp"
#include "openbus/crypto/PrivateKey.hpp"

#include <fstream>

void loginWithServerCredentials(std::string const& entity, openbus::Connection& connection)
{
  const openbus::PrivateKey key(entity + ".key");
  connection.loginByCertificate(entity, key);
}

#endif
