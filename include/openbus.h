#ifndef TECGRAF_OPENBUS_H_
#define TECGRAF_OPENBUS_H_

#include <CORBA.h>
#include <string>

/* exceptions */
namespace openbus {
  struct AlreadyConnected { };
  struct InvalidORB { };
}

#include "connection.h"
#include "multiplexer.h"

/* forward declarations */
namespace openbus {
  class Connection;
  namespace multiplexed {
    class ConnectionMultiplexer;
  }
}

namespace openbus {
  CORBA::ORB* createORB(int argc, char** argv) throw(CORBA::Exception);
  Connection* connect(const std::string host, const unsigned int port, CORBA::ORB* orb = 0)
    throw(CORBA::Exception, AlreadyConnected, InvalidORB);
}

namespace openbus {
  namespace multiplexed {
    CORBA::ORB* createORB(int argc, char** argv) throw(CORBA::Exception);
    Connection* connect(const std::string host, const unsigned int port, CORBA::ORB* orb = 0)
      throw(CORBA::Exception, InvalidORB);
    multiplexed::ConnectionMultiplexer* getConnectionMultiplexer(CORBA::ORB* orb);
  }
}

#endif
