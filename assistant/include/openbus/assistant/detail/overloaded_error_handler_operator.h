// -*- coding: iso-latin-1 -*-

#if !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING

#ifndef OPENBUS_ASSISTANT_DETAIL_OVERLOADED_ERROR_HANDLER_OPERATOR_H
#define OPENBUS_ASSISTANT_DETAIL_OVERLOADED_ERROR_HANDLER_OPERATOR_H

#include <openbus/assistant/exceptions.h>

#include <boost/preprocessor/iterate.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/mpl/size.hpp>

namespace openbus { namespace assistant { namespace detail {

namespace mpl = boost::mpl;

template <std::size_t N, typename F, typename D>
struct overloaded_error_handler_operator;

#define BOOST_PP_ITERATION_PARAMS_1 (3, (1, BOOST_PP_SEQ_SIZE(OPENBUS_ASSISTANT_ALL_EXCEPTIONS()) \
                                         , "openbus/assistant/detail/overloaded_error_handler_operator.h"))
#include BOOST_PP_ITERATE()

} } }
#endif
#else

#define N BOOST_PP_ITERATION()
#define OPENBUS_ASSISTANT_DETAIL_OVERLOAD_ERROR_HANDLER_PARAMETER(Z, N, DATA) \
  BOOST_PP_COMMA_IF(N) typename boost::mpl::at_c<N, DATA>::type BOOST_PP_CAT(a, N)

template <typename F, typename D>
struct overloaded_error_handler_operator<N, F, D>
{
  template <typename T>
  overloaded_error_handler_operator(T object)
    : f(object) {}

  typedef typename function_types::parameter_types<F>::type parameter_types;

  typename function_types::result_type<F>::type operator()
  (BOOST_PP_REPEAT(N, OPENBUS_ASSISTANT_DETAIL_OVERLOAD_ERROR_HANDLER_PARAMETER
                   , parameter_types)) const
  {
    f(BOOST_PP_ENUM_PARAMS(N, a));
  }

  boost::function<F> f;
};

#undef N
#endif
