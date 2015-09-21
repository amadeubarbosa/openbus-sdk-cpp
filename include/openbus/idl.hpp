// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_IDL_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_IDL_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "openbus_core-2.1C.h"
#include "openbus_access-2.1C.h"
#include "openbus_creden-2.1C.h"
#include "openbus_export-2.1C.h"
#include "openbus_legacy-2.1C.h"
#include "openbus_offers-2.1C.h"

#include "coreC.h"
#include "access_controlC.h"
#include "credentialC.h"
#include "data_exportC.h"
#include "offer_registryC.h"
#pragma clang diagnostic pop

namespace openbus {
  namespace idl {
    namespace core = tecgraf::openbus::core::v2_1;
    namespace services = core::services;
    namespace access = services::access_control;
    namespace creden = core::credential;
    namespace data_export = core::data_export;
    namespace legacy_support = services::legacy_support;
    namespace offers = services::offer_registry;
    
    namespace legacy {
      namespace core = tecgraf::openbus::core::v2_0;
      namespace services = core::services;
      namespace access = services::access_control;
      namespace creden = core::credential;
      namespace data_export = core::data_export;
      namespace offers = services::offer_registry;
    }
  }
}

#endif
