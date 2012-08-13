#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>

#include "dedicated_clock.h"
#include <CORBA.h>
#include <time.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

namespace offer_registry
 = tecgraf::openbus::core::v2_00::services::offer_registry;
namespace demo = tecgraf::openbus::demo;
namespace services = tecgraf::openbus::core::v2_00::services;
namespace access_control = tecgraf::openbus::core::v2_00::services::access_control;

struct ClockImpl : public POA_tecgraf::openbus::demo::Clock
{
  CORBA::Long getTimeInTicks()
  {
    return time(0);
  }
};

int main(int argc, char** argv)
{
  openbus::extension::Openbus openbus(argc, argv, "localhost", 2089);
  openbus.loginByPassword("demo", "demo");

  scs::core::ComponentId componentId
    = { "DedicatedClock", '1', '0', '0', ""};
  scs::core::ComponentContext clock_component
    (manager->orb(), componentId);
  ClockImpl clock_servant;
  clock_component.addFacet
    ("clock", demo::_tc_Clock->id(), &clock_servant);
  offer_registry::ServicePropertySeq properties;
  properties.length(1);
  properties[0].name = "offer.domain";
  properties[0].value = "Demos";
  openbus.addOffer(clock_component.getIComponent(), properties);

  openbus.wait();

  return 0;
}
