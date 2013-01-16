// -*- coding: iso-8859-1 -*-

#ifndef OPENBUS_ASSISTANT_ERROR_HANDLER_TYPES_H
#define OPENBUS_ASSISTANT_ERROR_HANDLER_TYPES_H

#include <openbus/assistant/overloaded_error_handler.h>
#include <openbus/assistant/exceptions.h>

#include <openbus/Connection.hpp>

#include <boost/mpl/void.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/joint_view.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/add_const.hpp>

#include <stdexcept>
#include <CORBA.h>
#include <stubs/scs.h>
#include <stubs/core.h>
#include <stubs/credential.h>
#include <stubs/access_control.h>
#include <stubs/offer_registry.h>

namespace openbus { namespace assistant {

namespace idl = tecgraf::openbus::core::v2_0;
namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace idl_cr = tecgraf::openbus::core::v2_0::credential;

namespace detail {
  struct dummy_login_error_callback
  {
    template <typename E>
    void operator()(E const&) const {}
  };
  struct dummy_register_error_callback
  {
    template <typename E>
    void operator()(E const&, scs::core::IComponent_var, idl_or::ServicePropertySeq) const {}
  };
  struct dummy_fatal_error_callback
  {
    template <typename E>
    void operator()(E const&) const {}
  };
  struct dummy_find_error_callback
  {
    template <typename E>
    void operator()(E const&) const {}
  };

  template <typename A, typename F>
  struct prepend_to_signature
  {
    typedef typename boost::function_types::result_type<F>::type result_type;
    typedef typename boost::function_types::parameter_types<F>::type parameter_types;
    typedef boost::mpl::joint_view<boost::mpl::vector2<result_type, A>, parameter_types> sequence_type;
    typedef typename boost::function_types::function_type<sequence_type>::type type;
  };
}

typedef boost::mpl::vector<BOOST_PP_SEQ_ENUM(OPENBUS_ASSISTANT_LOGIN_EXCEPTIONS())> login_exception_types;
typedef boost::mpl::vector<BOOST_PP_SEQ_ENUM(OPENBUS_ASSISTANT_REGISTER_EXCEPTIONS())> register_exception_types;
typedef boost::mpl::vector<BOOST_PP_SEQ_ENUM(OPENBUS_ASSISTANT_FATAL_EXCEPTIONS())> fatal_exception_types;
typedef boost::mpl::vector<BOOST_PP_SEQ_ENUM(OPENBUS_ASSISTANT_FIND_EXCEPTIONS())> find_exception_types;

typedef assistant::overloaded_error_handler
<boost::mpl::transform
 <login_exception_types, detail::prepend_to_signature
  <boost::add_reference<boost::add_const<mpl::_> >, void()> >::type
  , detail::dummy_login_error_callback> login_error_callback_type;
typedef assistant::overloaded_error_handler
<boost::mpl::transform
 <register_exception_types, detail::prepend_to_signature
  <boost::add_reference<boost::add_const<mpl::_> >
   , void(scs::core::IComponent_var
          , idl_or::ServicePropertySeq)> >::type
 , detail::dummy_register_error_callback> register_error_callback_type;
typedef assistant::overloaded_error_handler
<boost::mpl::transform
 <fatal_exception_types, detail::prepend_to_signature
  <boost::add_reference<boost::add_const<mpl::_> >, void()> >::type
 , detail::dummy_fatal_error_callback> fatal_error_callback_type;
typedef assistant::overloaded_error_handler
<boost::mpl::transform
 <find_exception_types, detail::prepend_to_signature
  <boost::add_reference<boost::add_const<mpl::_> >, void()> >::type
 , detail::dummy_find_error_callback> find_error_callback_type;

} }

#endif
