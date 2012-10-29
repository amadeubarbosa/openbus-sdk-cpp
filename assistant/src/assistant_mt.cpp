// -*- coding: iso-latin-1 -*-

#include <openbus/assistant.h>
#include <openbus/assistant/detail/exception_logging.h>
#include <openbus/assistant/detail/register_information.h>
#include <openbus/assistant/detail/execute_with_retry.h>
#include <openbus/assistant/detail/exception_message.h>
#include <openbus/assistant/detail/create_connection_and_login.h>

namespace openbus { namespace assistant {

namespace assistant_detail {

struct wait_until_cancelled
{
  template <typename Lock, typename VarCond, typename Pred, typename TimePoint>
  void operator()(Lock& lock, VarCond& var_cond, Pred p, TimePoint time_point) const
  {
    while(!p() && var_cond.wait_until(lock, time_point) != boost::cv_status::timeout)
      ;
    if(p())
      throw wait_predicate_signalled();
  }
};

void wait_until_timeout_and_signal_exit::operator()() const
{
  wait_until_cancelled f;
  boost::unique_lock<boost::mutex> l(state->mutex);
  boost::chrono::steady_clock::time_point time_point
    = boost::chrono::steady_clock::now() + state->retry_wait;
  f(l, state->work_cond_var, boost::bind(predicate(), state), time_point);
}

}

typedef assistant_detail::register_information register_information;
typedef assistant_detail::register_container register_container;
typedef assistant_detail::register_iterator register_iterator;
typedef assistant_detail::register_fail register_fail;

register_information construct_register_item(std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> const& item);

void register_component(idl_or::OfferRegistry_var offer_registry
                        , register_iterator& reg_current, register_iterator reg_last
                        , logger::logger& logging);


void register_relogin(boost::shared_ptr<assistant_detail::shared_state> state)
{
  boost::unique_lock<boost::mutex> lock(state->mutex);
  state->new_queued_components = true;
  state->relogin = true;
  state->work_cond_var.notify_one();
}

namespace {

void work_thread_function(boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope work_thread_log(state->logging, logger::debug_level, "work_thread_function");
  assistant_detail::exception_logging ex_l(work_thread_log);
  try
  {
    {
      boost::function<void(std::string /*error*/)> login_error_callback;
      {
        boost::unique_lock<boost::mutex> lock(state->mutex);
        login_error_callback = state->login_error;
      }

      work_thread_log.level_log(logger::debug_level, "Creating connection and logging");
      std::auto_ptr<Connection> connection = assistant_detail::create_connection_and_login
        (state->orb, state->host, state->port, state->auth_info
         , state->logging, state
         , login_error_callback, boost::none);
      {
        work_thread_log.level_log(logger::debug_level, "Registering connection as default");
        openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
          (state->orb->resolve_initial_references("OpenbusConnectionManager"));
        assert(manager != 0);
        manager->setDefaultConnection(connection.get());
      }
      assert(!!connection.get());
      boost::unique_lock<boost::mutex> lock(state->mutex);
      assert(!state->connection.get());
      state->connection = connection;
      state->connection_ready = true;
      state->connection_ready_var.notify_one();
      lock.unlock();
      work_thread_log.level_log(logger::debug_level, "Saved connection");
    }

    boost::unique_lock<boost::mutex> lock(state->mutex);
    do
    {
      do
      {
        if(state->relogin)
        {
          work_thread_log.level_log(logger::debug_level, "The login has failed, will have to re-register every component");
          state->relogin = false;
          state->queued_components.insert(state->queued_components.end()
                                          , state->components.begin(), state->components.end());
          state->components.clear();
        }
        register_container components;
        std::transform(state->queued_components.begin()
                       , state->queued_components.end()
                       , std::back_inserter<register_container>(components)
                       , &construct_register_item);
        std::copy(state->queued_components.begin(), state->queued_components.end()
                  , std::back_inserter<std::vector<std::pair
                  <scs::core::IComponent_var, idl_or::ServicePropertySeq> > >(state->components));
        state->queued_components.clear();

        boost::function<void(scs::core::IComponent_var, idl_or::ServicePropertySeq
                             , std::string /*error*/)> register_error_callback
          = state->register_error;

        do
        {
          lock.unlock();
          try
          {
            work_thread_log.level_log(logger::debug_level, "Registering some components");
            register_iterator current = components.begin();
            assistant_detail::execute_with_retry
              (boost::bind(&register_component, state->connection->offers()
                           , boost::ref(current), components.end()
                           , boost::ref(state->logging))
               , register_fail(register_error_callback, current)
               , assistant_detail::wait_until_timeout_and_signal_exit(state)
               , state->logging);
          }
          catch(assistant_detail::timeout_error const&)
          {
            work_thread_log.level_log(logger::info_level, "Timeout'ed registering component");
          }
          lock.lock();
        }
        while(std::find_if(components.begin(), components.end()
                           , &assistant_detail::not_registered_predicate) != components.end()
              && !state->relogin);
      }
      while(state->relogin);
      lock.unlock();
      work_thread_log.level_log(logger::debug_level, "All components registered");
      
      lock.lock();
      while(!state->new_queued_components && !state->work_exit)
      {
        work_thread_log.level_log(logger::debug_level, "Waiting for newer components to be registered");
        state->work_cond_var.wait(lock);
      }
      state->new_queued_components = false;
      if(state->work_exit)
        return;
    }
    while(true);
  }
  catch(std::bad_alloc const& e)
  {
    logger::log_scope l(state->logging, logger::error_level, "Worker thread std::bad_alloc catch");
    assistant_detail::exception_logging ex_l(l);
  }
  catch(assistant_detail::wait_predicate_signalled const& e)
  {
    logger::log_scope l(state->logging, logger::info_level
                        , "Worker thread was cancelled because shutdown was called");
  }
  catch(std::exception const& e)
  {
    logger::log_scope l(state->logging, logger::error_level, "Worker thread std::exception catch");
    l.vlog("The typeid(e).name() of the exception is: %s", typeid(e).name());
    assistant_detail::exception_logging ex_l(l);
    try
    {
      boost::unique_lock<boost::mutex> lock(state->mutex);
      boost::function<void(const char*)> fatal_error = state->fatal_error;
      lock.unlock();
      if(fatal_error)
        fatal_error(e.what());
    }
    catch(...)
    {}
  }
  catch(...)
  {
    logger::log_scope l(state->logging, logger::error_level, "Worker thread all catch");
    assistant_detail::exception_logging ex_l(l);
    try
    {
      boost::unique_lock<boost::mutex> lock(state->mutex);
      boost::function<void(const char*)> fatal_error = state->fatal_error;
      lock.unlock();
      if(fatal_error)
        fatal_error("Unknown exception was thrown");
    }
    catch(...)
    {}
  }
}

}

void create_threads(boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope l(state->logging, logger::info_level, "create_threads");
  state->work_thread = boost::thread(boost::bind(&assistant::work_thread_function, state));
  l.log("Created work thread");
}

void AssistantImpl::shutdown()
{
  boost::unique_lock<boost::mutex> lock(state->mutex);
  state->work_exit = true;
  state->work_cond_var.notify_one();
  state->work_thread.join();
}

void AssistantImpl::registerService(scs::core::IComponent_var component, idl_or::ServicePropertySeq properties)
{
  boost::unique_lock<boost::mutex> l(state->mutex);
  state->queued_components.push_back(std::make_pair(component, properties));
  state->new_queued_components = true;
  state->work_cond_var.notify_one();
}

namespace assistant_detail {

void wait_login(boost::shared_ptr<assistant_detail::shared_state> state)
{
  boost::unique_lock<boost::mutex> l(state->mutex);
  while(!state->connection_ready)
    state->connection_ready_var.wait(l);
  assert(state->connection_ready);
}

}

struct assistant_access
{
  static boost::shared_ptr<assistant_detail::shared_state> state(Assistant a)
  {
    return a.state;
  }
};

void waitLogin(Assistant a)
{
  assistant_detail::wait_login(assistant_access::state(a));
}

void AssistantImpl::onLoginError(boost::function<void(std::string /*error*/)> f)
{
  assert(!!state);
  boost::unique_lock<boost::mutex> lock(state->mutex);
  state->login_error = f;
}

void AssistantImpl::onRegisterError(boost::function<void(scs::core::IComponent_var
                                                         , idl_or::ServicePropertySeq
                                                         , std::string /*error*/)> f)
{
  assert(!!state);
  boost::unique_lock<boost::mutex> lock(state->mutex);
  state->register_error = f;
}

void AssistantImpl::onFatalError(boost::function<void(const char* /*error*/)> f)
{
  assert(!!state);
  boost::unique_lock<boost::mutex> lock(state->mutex);
  state->fatal_error = f;
}

} }

