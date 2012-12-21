#ifndef OPENBUS_CPP_INTEROP_DELEGATION_SERVER_LOGIN_H
#define OPENBUS_CPP_INTEROP_DELEGATION_SERVER_LOGIN_H

#include <fstream>

void loginWithServerCredentials(std::string const& entity, openbus::Connection& connection)
{
  std::ifstream ifs((entity + ".key").c_str());
  assert(ifs.is_open());
  ifs.seekg(0, std::ios::end);

  CORBA::OctetSeq key;
  key.length(ifs.tellg());
  assert(key.length() > 0);
  ifs.seekg(0, std::ios::beg);
  ifs.rdbuf()->sgetn(static_cast<char*>(static_cast<void*>(key.get_buffer())), key.length());

  connection.loginByCertificate(entity.c_str(), key);
}

#endif
