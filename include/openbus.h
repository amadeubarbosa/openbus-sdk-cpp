#ifndef TECGRAF_OPENBUS_H_
#define TECGRAF_OPENBUS_H_

#include <CORBA.h>
#include <string>
#include <log.h>

/* exceptions */
namespace openbus {
  struct AlreadyConnected { };
  struct InvalidORB { };
}

#include "manager.h"

/* forward declarations */
namespace openbus {
  class ConnectionManager;
}

namespace openbus {
  CORBA::ORB* initORB(int argc, char** argv) throw(CORBA::Exception);
  ConnectionManager* getConnectionManager(CORBA::ORB* orb);
}

#endif
