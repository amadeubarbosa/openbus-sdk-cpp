/**
* \file Helper.h
*
* \brief Facilitador para a utilização de estruturas
* complexas.
*
*/

#ifndef OPENBUS_SDK_HELPER_H
#define OPENBUS_SDK_HELPER_H

#include <openbus/version.h>

#ifdef OPENBUS_ORBIX
  #include "../../stubs/orbix/registry_service.hh"
#else
  #include "registry_service.h"
#endif

/**
* \brief openbus
*/
namespace openbus {

/**
* \brief Utilitários para a programação.
*/
  namespace util {

    namespace idl_namespace = tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE;

    typedef idl_namespace::registry_service::FacetList FacetList;
    typedef idl_namespace::registry_service::PropertyList PropertyList;
    typedef idl_namespace::registry_service::ServiceOffer ServiceOffer;
    typedef idl_namespace::registry_service::ServiceOfferList ServiceOfferList;
    typedef idl_namespace::registry_service::ServiceOfferList_var ServiceOfferList_var;

  /**
  * \brief Auxilia na construção de uma lista de facetas.
  */
    class FacetListHelper {
      private:
        idl_namespace::registry_service::FacetList_var facetList;
        CORBA::ULong numElements;
      public:
        FacetListHelper();
        ~FacetListHelper();
        void add(const char* facet);
        idl_namespace::registry_service::FacetList_var getFacetList();
    };

  /**
  * \brief Auxilia na construção de uma lista de propriedades.
  */
    class PropertyListHelper {
      private:
        idl_namespace::registry_service::PropertyList_var propertyList;
        CORBA::ULong numElements;
      public:
        PropertyListHelper();
        ~PropertyListHelper();
        void add(
          const char* key,
          const char* value);
        idl_namespace::registry_service::PropertyList_var getPropertyList();
    };
  }
}

#endif 

