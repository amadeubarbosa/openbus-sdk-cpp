#ifndef OPENBUS_CPP_INTEROP_DELEGATION_SERVER_LOGIN_H
#define OPENBUS_CPP_INTEROP_DELEGATION_SERVER_LOGIN_H

#include "openbus/Connection.h"
#include "openbus/util/PrivateKey.h"

#include <fstream>

void loginWithServerCredentials(std::string const& entity, openbus::Connection& connection)
{
  const openbus::PrivateKey key(entity + ".key");
  connection.loginByCertificate(entity, key);
}

#endif
