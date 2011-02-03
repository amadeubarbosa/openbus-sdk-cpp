/**
* \file Helper.cpp
*
* \brief Facilitador para a utilização de estruturas
* complexas.
*
*/

#include <openbus/version.h>
#include "openbus/helper.h"

namespace openbus { namespace util {

namespace idl_namespace = tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE;

FacetListHelper::FacetListHelper()
{
  facetList = new idl_namespace::registry_service::FacetList();
  numElements = 0;
}

FacetListHelper::~FacetListHelper()
{
}

void FacetListHelper::add(const char* facet)
{
  facetList->length(numElements + 1);
  facetList[numElements] = facet;
  numElements++;
}

idl_namespace::registry_service::FacetList_var FacetListHelper::getFacetList()
{
  return facetList;
}

PropertyListHelper::PropertyListHelper()
{
  propertyList = new idl_namespace::registry_service::PropertyList();
  numElements = 0;
}

PropertyListHelper::~PropertyListHelper()
{
}

void PropertyListHelper::add(const char* key
                             , const char* value)
{
  propertyList->length(numElements + 1);
  idl_namespace::registry_service::Property_var property = new idl_namespace::registry_service::Property;
  property->name = key;
  idl_namespace::registry_service::PropertyValue_var propertyValue =   \
    new idl_namespace::registry_service::PropertyValue(1);
  propertyValue->length(1);
  propertyValue[(CORBA::ULong) 0] = value;
  property->value = propertyValue;
  propertyList[numElements] = property;
  numElements++;
}

idl_namespace::registry_service::PropertyList_var PropertyListHelper::getPropertyList()
{
  return propertyList;
}
  
} }

