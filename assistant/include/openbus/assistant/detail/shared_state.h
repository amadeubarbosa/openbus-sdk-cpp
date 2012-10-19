// -*- coding: iso-8859-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_SHARED_STATE_H
#define OPENBUS_ASSISTANT_DETAIL_SHARED_STATE_H

#include <log/logger.h>

#include <scs/IComponent.h>
#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>

#ifdef ASSISTANT_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif
#include <boost/variant.hpp>
#include <boost/chrono/include.hpp>
#include <boost/shared_ptr.hpp>

#include <CORBA.h>


namespace openbus { namespace assistant {

unsigned int const default_retry_wait = 30u;

namespace assistant_detail {

struct password_authentication_info
{
  std::string username, password;
};

struct certificate_authentication_info
{
  std::string entity;
  idl::OctetSeq private_key;
};

struct shared_auth_authentication_info
{
  boost::function<std::pair<idl_ac::LoginProcess_ptr, idl::OctetSeq>()> callback;
};

typedef boost::variant<password_authentication_info
                       , certificate_authentication_info
                       , shared_auth_authentication_info>
  authentication_info;

struct add_offers_dispatcher;

struct shared_state
{
  typedef boost::function<void(std::string)> login_error_callback_type;
  typedef boost::function<void(scs::core::IComponent_var
                               , idl_or::ServicePropertySeq
                               , std::string)> register_error_callback_type;
  typedef boost::function<void(const char*)> fatal_error_callback_type;

  // connection_ready = true ==> (connection.get() != 0
  //  /\ connection.get() is never modified again)
  // attribution to connection 'happens before' connection_ready = true
  // so that if connection_ready is true, we don't need to lock any mutexes
  // to read connection member variable

  logger::logger logging;
  CORBA::ORB_var const orb;
  assistant_detail::authentication_info const auth_info;
  std::string const host;
  unsigned short const port;
  std::vector<std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> > components;
  std::vector<std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> > queued_components;
  login_error_callback_type login_error;
  register_error_callback_type register_error;
  fatal_error_callback_type fatal_error;
  std::auto_ptr<openbus::Connection> connection;
  boost::chrono::steady_clock::duration retry_wait;
  bool work_exit;
  bool connection_ready;
  bool relogin;
#ifdef ASSISTANT_SDK_MULTITHREAD
  bool new_queued_components;
  boost::thread work_thread;
  boost::mutex mutex;
  // work_cond_var is notified for exiting the work thread and to
  // consume queued components for registration
  boost::condition_variable work_cond_var;
  boost::condition_variable connection_ready_var;
#else
  add_offers_dispatcher* asynchronous_dispatcher;
#endif

  shared_state(CORBA::ORB_var orb, authentication_info auth_info
               , std::string const& host, unsigned short port
               , login_error_callback_type login_error
               , register_error_callback_type register_error
               , fatal_error_callback_type fatal_error)
    : orb(orb), auth_info(auth_info), host(host), port(port)
    , login_error(login_error), register_error(register_error)
    , fatal_error(fatal_error), work_exit(false)
    , connection_ready(false), relogin(false)
#ifdef ASSISTANT_SDK_MULTITHREAD
    , new_queued_components(false)
#else
    , asynchronous_dispatcher(0)
#endif
  {}
};

} } }

#endif
