#ifndef TECGRAF_OPENBUS_H_
#define TECGRAF_OPENBUS_H_

#include <CORBA.h>
#include <string>

#include "connection.h"
#include "multiplexer.h"

namespace openbus {
  class Connection;
  namespace multiplexed {
    class ConnectionMultiplexer;
  }
}

namespace openbus {
  class ORB {
  public:
    ~ORB();
    CORBA::ORB* orb() const { return _orb; }
    //[doubt] readonly?
    multiplexed::ConnectionMultiplexer* getConnectionMultiplexer() { return _multiplexer; }
  private:
    CORBA::ORB_var _orb;
    ORB(int argc, char** argv);
    multiplexed::ConnectionMultiplexer* _multiplexer;
    friend ORB* createORB(int argc, char** argv) throw(CORBA::Exception);
  };

  ORB* createORB(int argc, char** argv) throw(CORBA::Exception);

  Connection* connect(const std::string host, const unsigned int port, ORB* orb = 0)
    throw(CORBA::Exception);
}

#endif
