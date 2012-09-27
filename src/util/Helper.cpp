/**
* \file Helper.cpp
*
* \brief Facilitador para a utilização de estruturas
* complexas.
*
*/

#include <cstdio>
#include "util/Helper.h"

namespace openbus {
  namespace util {

    FacetListHelper::FacetListHelper() {
      facetList = new tecgraf::openbus::core::v1_05::registry_service::FacetList();
      numElements = 0;
    }

    FacetListHelper::~FacetListHelper() {
    }

    void FacetListHelper::add(const char* facet) {
      if (facet) {
        facetList->length(numElements + 1);
        facetList[numElements] = facet;
        numElements++;
      }
    }

    tecgraf::openbus::core::v1_05::registry_service::FacetList_var FacetListHelper::getFacetList() {
      return facetList;
    }

    PropertyListHelper::PropertyListHelper() {
      propertyList = new tecgraf::openbus::core::v1_05::registry_service::PropertyList();
      numElements = 0;
    }

    PropertyListHelper::~PropertyListHelper() {
    }

    void PropertyListHelper::add(
      const char* key,
      const char* value)
    {
      propertyList->length(numElements + 1);
      tecgraf::openbus::core::v1_05::registry_service::Property_var property = new tecgraf::openbus::core::v1_05::registry_service::Property;
      property->name = key;
      tecgraf::openbus::core::v1_05::registry_service::PropertyValue_var propertyValue = \
        new tecgraf::openbus::core::v1_05::registry_service::PropertyValue(1);
      propertyValue->length(1);
      propertyValue[(CORBA::ULong) 0] = value;
      property->value = propertyValue;
      propertyList[numElements] = property;
      numElements++;
    }

    tecgraf::openbus::core::v1_05::registry_service::PropertyList_var PropertyListHelper::getPropertyList() {
      return propertyList;
    }
  }
}

