/**
* \file Helper.h
*
* \brief Facilitador para a utilização de estruturas
* complexas.
*
*/

#ifndef HELPER_H_
#define HELPER_H_

#ifdef OPENBUS_ORBIX
  #include "stubs/orbix/registry_service.hh"
#else
  #include "stubs/mico/registry_service.h"
#endif

/**
* \brief openbus
*/
namespace openbus {

/**
* \brief Utilitários para a programação.
*/
  namespace util {

    typedef tecgraf::openbus::core::v1_05::registry_service::FacetList FacetList;
    typedef tecgraf::openbus::core::v1_05::registry_service::PropertyList PropertyList;
    typedef tecgraf::openbus::core::v1_05::registry_service::ServiceOffer ServiceOffer;
    typedef tecgraf::openbus::core::v1_05::registry_service::ServiceOfferList ServiceOfferList;
    typedef tecgraf::openbus::core::v1_05::registry_service::ServiceOfferList_var ServiceOfferList_var;

  /**
  * \brief Auxilia na construção de uma lista de facetas.
  */
    class FacetListHelper {
      private:
      tecgraf::openbus::core::v1_05::registry_service::FacetList_var facetList;
        CORBA::ULong numElements;
      public:
        FacetListHelper();
        ~FacetListHelper();
        void add(const char* facet);
        tecgraf::openbus::core::v1_05::registry_service::FacetList_var getFacetList();
    };

  /**
  * \brief Auxilia na construção de uma lista de propriedades.
  */
    class PropertyListHelper {
      private:
      tecgraf::openbus::core::v1_05::registry_service::PropertyList_var propertyList;
        CORBA::ULong numElements;
      public:
        PropertyListHelper();
        ~PropertyListHelper();
        void add(
          const char* key,
          const char* value);
        tecgraf::openbus::core::v1_05::registry_service::PropertyList_var getPropertyList();
    };
  }
}

#endif 

