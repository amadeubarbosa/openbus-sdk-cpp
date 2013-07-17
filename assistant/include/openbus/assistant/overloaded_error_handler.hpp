// -*- coding: iso-8859-1-unix -*-

#if !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING

#ifndef OPENBUS_ASSISTANT_OVERLOADED_ERROR_HANDLER_H
#define OPENBUS_ASSISTANT_OVERLOADED_ERROR_HANDLER_H

#include <openbus/assistant/exceptions.hpp>

#ifdef BOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX
#if BOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX < BOOST_PP_SEQ_SIZE(OPENBUS_ASSISTANT_ALL_EXCEPTIONS())
#error The BOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX must be at least the size of the sequence in openbus/assistant/exceptions.h or must not be defined at all. Add -DBOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX=N with N big enough
#endif
#else
#define BOOST_FUNCTIONAL_OVERLOADED_FUNCTION_CONFIG_OVERLOAD_MAX BOOST_PP_SEQ_SIZE(OPENBUS_ASSISTANT_ALL_EXCEPTIONS())
#endif

#include <boost/preprocessor/iterate.hpp>
#include <boost/functional/overloaded_function.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>

namespace openbus { namespace assistant {

namespace mpl = boost::mpl;

namespace detail {

template <std::size_t N, typename Seq>
struct overloaded_error_handler_aux;

#define BOOST_PP_ITERATION_PARAMS_1 (3, (2, BOOST_PP_SEQ_SIZE(OPENBUS_ASSISTANT_ALL_EXCEPTIONS()) \
                                         , "openbus/assistant/overloaded_error_handler.hpp"))
#include BOOST_PP_ITERATE()

}

template <typename Seq, typename Default>
struct overloaded_error_handler : detail::overloaded_error_handler_aux<mpl::size<Seq>::value, Seq>
{
  typedef detail::overloaded_error_handler_aux<mpl::size<Seq>::value, Seq> base_type;
  typedef overloaded_error_handler<Seq, Default> self_type;
  template <typename T>
  overloaded_error_handler(T const& object)
    : base_type(object) {}
  overloaded_error_handler()
    : base_type(Default()) {}
};

} }
#endif
#else

#define N BOOST_PP_ITERATION()
#define OPENBUS_ASSISTANT_DETAIL_OVERLOAD_ERROR_HANDLER_PARAMETER(Z, I, DATA) \
  BOOST_PP_COMMA_IF(I) typename boost::mpl::at_c<DATA, I>::type

template <typename Seq>
struct overloaded_error_handler_aux<N, Seq>
  : boost::overloaded_function
    <BOOST_PP_REPEAT(N, OPENBUS_ASSISTANT_DETAIL_OVERLOAD_ERROR_HANDLER_PARAMETER, Seq)>
{
  typedef boost::overloaded_function
  <BOOST_PP_REPEAT(N, OPENBUS_ASSISTANT_DETAIL_OVERLOAD_ERROR_HANDLER_PARAMETER, Seq)>
  base_type;

  template <typename T>
  overloaded_error_handler_aux(T const& object)
    : base_type(BOOST_PP_ENUM_PARAMS(N, object BOOST_PP_INTERCEPT)) {}
};

#undef N
#endif
