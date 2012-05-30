#ifndef TECGRAF_OPENBUS_H_
#define TECGRAF_OPENBUS_H_

#include <CORBA.h>
#include <manager.h>
#include <log.h>

namespace openbus {
  CORBA::ORB* initORB(int argc, char** argv) throw(CORBA::Exception);
}

#endif
