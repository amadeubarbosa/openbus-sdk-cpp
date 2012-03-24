#ifndef TECGRAF_OPENBUS_H_
#define TECGRAF_OPENBUS_H_

#include "connection.h"
#include <CORBA.h>
#include <string>

namespace openbus {
  class ORB {
    public:
      ~ORB();
      CORBA::ORB* orb() const
        { return _orb; }
      friend ORB* createORB(int argc, char** argv) throw(CORBA::Exception);
    private:
      ORB(int argc, char** argv);
      CORBA::ORB_var _orb;
  };
  
  ORB* createORB(int argc, char** argv) throw(CORBA::Exception);
  Connection* connect(const std::string host, const unsigned int port, ORB* orb = 0)
    throw(CORBA::Exception);
}

#endif
