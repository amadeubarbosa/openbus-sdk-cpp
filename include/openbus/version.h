#ifndef OPENBUS_SDK_VERSION_H
#define OPENBUS_SDK_VERSION_H

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/comparison/less.hpp>
#include <boost/preprocessor/arithmetic/div.hpp>
#include <boost/preprocessor/arithmetic/mod.hpp>

#ifndef OPENBUS_IDL_VERSION
#error OPENBUS_IDL_VERSION must be defined with the IDL version
#endif

#define OPENBUS_IDL_VERSION_MAJOR BOOST_PP_DIV(OPENBUS_IDL_VERSION,100)
#define OPENBUS_IDL_VERSION_MINOR_NUMBER BOOST_PP_MOD(OPENBUS_IDL_VERSION,100)
#define OPENBUS_IDL_VERSION_MINOR BOOST_PP_IF(BOOST_PP_LESS(OPENBUS_IDL_VERSION_MINOR_NUMBER, 10) \
                                              , BOOST_PP_CAT(0, OPENBUS_IDL_VERSION_MINOR_NUMBER) \
                                              , OPENBUS_IDL_VERSION_MINOR_NUMBER)
#define OPENBUS_IDL_VERSION_NAMESPACE BOOST_PP_CAT(v,                              \
                                      BOOST_PP_CAT(OPENBUS_IDL_VERSION_MAJOR, \
                                      BOOST_PP_CAT(_, OPENBUS_IDL_VERSION_MINOR)))

#define OPENBUS_IDL_VERSION_STRING BOOST_PP_STRINGIZE(BOOST_PP_CAT(v,   \
                                                                   BOOST_PP_CAT(OPENBUS_IDL_VERSION_MAJOR, \
                                                                                BOOST_PP_CAT(_, OPENBUS_IDL_VERSION_MINOR))))

#endif
