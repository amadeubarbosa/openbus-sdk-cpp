#ifndef TECGRAF_OPENBUS_H_
#define TECGRAF_OPENBUS_H_

#include <CORBA.h>
#include <string>

#include "connection.h"

namespace openbus {
  class ORB {
  public:
    ~ORB();
    CORBA::ORB* orb() const { return _orb; }
  private:
    CORBA::ORB_var _orb;
    ORB(int argc, char** argv);
    friend ORB* createORB(int argc, char** argv) throw(CORBA::Exception);
  };

  ORB* createORB(int argc, char** argv) throw(CORBA::Exception);

  Connection* connect(const std::string host, const unsigned int port, ORB* orb = 0)
    throw(CORBA::Exception);
}

#endif
