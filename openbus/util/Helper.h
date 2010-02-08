/**
* \file Helper.h
*
* \brief Facilitador para a utiliza��o de estruturas
* complexas.
*
*/

#ifndef HELPER_H_
#define HELPER_H_

#ifdef OPENBUS_MICO
  #include "../../stubs/mico/registry_service.h"
#else
  #include "../../stubs/orbix/registry_service.hh"
#endif

using namespace tecgraf::openbus::core::v1_05;

/**
* \brief openbus
*/
namespace openbus {

/**
* \brief Utilit�rios para a programa��o.
*/
  namespace util {

    typedef registry_service::FacetList FacetList;
    typedef registry_service::PropertyList PropertyList;
    typedef registry_service::ServiceOffer ServiceOffer;
    typedef registry_service::ServiceOfferList ServiceOfferList;
    typedef registry_service::ServiceOfferList_var ServiceOfferList_var;

  /**
  * \brief Auxilia na constru��o de uma lista de facetas.
  */
    class FacetListHelper {
      private:
        registry_service::FacetList_var facetList;
        CORBA::ULong numElements;
      public:
        FacetListHelper();
        ~FacetListHelper();
        void add(const char* facet);
        registry_service::FacetList_var getFacetList();
    };

  /**
  * \brief Auxilia na constru��o de uma lista de propriedades.
  */
    class PropertyListHelper {
      private:
        registry_service::PropertyList_var propertyList;
        CORBA::ULong numElements;
      public:
        PropertyListHelper();
        ~PropertyListHelper();
        void add(
          const char* key,
          const char* value);
        registry_service::PropertyList_var getPropertyList();
    };
  }
}

#endif 

