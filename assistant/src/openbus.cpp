
#include <openbus/extension/openbus.h>
#include <openbus/extension/AssociativePropertySeq.h>

#include <boost/bind.hpp>
#include <boost/utility/result_of.hpp>

#include <iterator>

namespace openbus { namespace extension {

namespace {

const char* exception_message(CORBA::TRANSIENT const&)
{
  return "CORBA::TRANSIENT";
}

const char* exception_message(CORBA::COMM_FAILURE const&)
{
  return "CORBA::COMM_FAILURE";
}

const char* exception_message(CORBA::OBJECT_NOT_EXIST const&)
{
  return "CORBA::OBJECT_NOT_EXIST";
}

const char* exception_message(idl::services::ServiceFailure const&)
{
  return "tecgraf::openbus::core::v2_0::services::ServiceFailure";
}

const char* exception_message(idl::services::UnauthorizedOperation const&)
{
  return "tecgraf::openbus::core::v2_0::services::UnauthorizedOperation";
}

template <typename F, typename E>
typename boost::result_of<F()>::type execute_with_retry(F f, E error)
{
  do
  {
    try
    {
      return f();
    }
    catch(CORBA::TRANSIENT const& e)
    {
      error(e);
    }
    catch(CORBA::COMM_FAILURE const& e)
    {
      error(e);
    }
    catch(CORBA::OBJECT_NOT_EXIST const& e)
    {
      error(e);
    }
    catch(idl::services::ServiceFailure const& e)
    {
      error(e);
    }
    catch(idl::services::UnauthorizedOperation const& e)
    {
      error(e);
    }

    unsigned int t = 30;
    do { t = sleep(t); } while(t);
  }
  while(true);
}

struct error_creating_connection
{
  typedef void result_type;
  template <typename Exception>
  result_type operator()(Exception const& e) const
  {
  }
};

std::auto_ptr<Connection> create_connection_simple(CORBA::ORB_var orb, std::string const& host, unsigned short port)
{
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
    (orb->resolve_initial_references("OpenbusConnectionManager"));
  assert(manager != 0);

  return manager->createConnection(host.c_str(), port);
}

std::auto_ptr<Connection> create_connection(CORBA::ORB_var orb, std::string const& host, unsigned short port)
{
  return execute_with_retry(boost::bind(&create_connection_simple, orb, host, port), error_creating_connection());
}

void login_simple(Connection& c, extension_detail::authentication_info const& info)
{
  assert(boost::get<extension_detail::password_authentication_info const>(&info)
         || boost::get<extension_detail::certificate_authentication_info const>(&info));
  if(extension_detail::password_authentication_info const* p
     = boost::get<extension_detail::password_authentication_info const>(&info))
  {
    c.loginByPassword(p->username.c_str(), p->password.c_str());
  }
  else if(extension_detail::certificate_authentication_info const* p
          = boost::get<extension_detail::certificate_authentication_info const>(&info))
  {
    c.loginByCertificate(p->entity.c_str(), p->private_key);
  }
}

struct login_error
{
  boost::function<void(std::string /*error*/)> callback;

  login_error(boost::function<void(std::string /*error*/)> callback)
    : callback(callback) {}

  typedef void result_type;
  template <typename Exception>
  result_type operator()(Exception const& e) const
  {
    try
    {
      if(callback)
        callback(exception_message(e));
    }
    catch(...)
    {}
  }
};

std::auto_ptr<Connection> create_connection_and_login
  (CORBA::ORB_var orb, std::string const& host, unsigned short port
   , extension_detail::authentication_info const& info
   , login_error error)
{
  std::auto_ptr<Connection> c = create_connection(orb, host, port);
  execute_with_retry(boost::bind(&login_simple, boost::ref(*c), boost::ref(info))
                     , error);
  return c;
}

struct register_information
{
  scs::core::IComponent_var component;
  extension_detail::idl_or::ServicePropertySeq properties;
  bool registered;
};

typedef std::vector<register_information> register_container;
typedef register_container::iterator register_iterator;

void register_component(extension_detail::idl_or::OfferRegistry_var offer_registry
                        , register_iterator& reg_current, register_iterator reg_last)
{
  while(reg_current != reg_last)
  {
    register_iterator current = reg_current++;
    if(!current->registered)
    {
      offer_registry->registerService(current->component, current->properties);
      current->registered = true;
    }
  }
}

struct register_fail
{
  boost::function<void(scs::core::IComponent_var
                       , idl_or::ServicePropertySeq, std::string /*error*/)> register_error_callback;
  register_iterator* next_current;

  register_fail(boost::function<void(scs::core::IComponent_var
                                     , idl_or::ServicePropertySeq
                                     , std::string /*error*/)> register_error_callback
                , register_iterator& next_current)
    : register_error_callback(register_error_callback), next_current(&next_current) {}

  typedef void result_type;
  template <typename Exception>
  result_type operator()(Exception const& e) const
  {
    try
    {
      if(register_error_callback)
      {
        register_iterator current = boost::prior(*next_current);
        register_error_callback(current->component, current->properties
                                , exception_message(e));
      }
    }
    catch(...) {}
  }
};

register_information construct_register_item(std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> const& item)
{
  register_information r = {item.first, item.second, false};
  return r;
}

bool not_registered_predicate(register_information const& info)
{
  return !info.registered;
}

void work_thread_function(boost::shared_ptr<extension_detail::shared_state> state)
{
  try
  {
    {
      boost::function<void(std::string /*error*/)> login_error_callback;
      {
        boost::unique_lock<boost::mutex> lock(state->mutex);
        login_error_callback = state->login_error;
      }

      std::auto_ptr<Connection> connection = create_connection_and_login
        (state->orb, state->host, state->port, state->auth_info
         , login_error(login_error_callback));
      {
        openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
          (state->orb->resolve_initial_references("OpenbusConnectionManager"));
        assert(manager != 0);
        manager->setDispatcher(*connection);
      }
      assert(!!connection.get());
      boost::unique_lock<boost::mutex> lock(state->mutex);
      assert(!state->connection.get());
      state->connection = connection;
    }

    boost::unique_lock<boost::mutex> lock(state->mutex);
    do
    {
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
      lock.unlock();

      do
      {
        register_iterator current = components.begin();
        execute_with_retry(boost::bind(&register_component, state->connection->offers()
                                       , boost::ref(current), components.end())
                           , register_fail(register_error_callback, current));
      }
      while(std::find_if(components.begin(), components.end()
                         , &not_registered_predicate) != components.end());
      
      lock.lock();
      while(!state->new_queued_components && !state->work_exit)
      {
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
  }
  catch(std::exception const& e)
  {
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

void orb_thread_function(CORBA::ORB_var orb
                         , boost::shared_ptr<extension_detail::shared_state> state)
{
  orb->run();
  {
    boost::unique_lock<boost::mutex> lock(state->mutex);
    state->work_exit = true;
    state->work_cond_var.notify_one();
  }
}

}

Openbus Openbus::startByPassword(const char* username, const char* password
                                 , const char* host, unsigned short port
                                 , int argc, const char** argv)
{
  CORBA::ORB_var orb = ORBInitializer(argc, const_cast<char**>(argv));
  extension_detail::password_authentication_info info = {username, password};
  return Openbus(orb, host, port, info);
}
 
Openbus Openbus::startByCertificate(const char* entity, const idl::OctetSeq privKey
                                    , const char* host, unsigned short port
                                    , int argc, const char** argv)
{
  CORBA::ORB_var orb = ORBInitializer(argc, const_cast<char**>(argv));
  extension_detail::certificate_authentication_info info = {entity, privKey};
  return Openbus(orb, host, port, info);
}

Openbus::Openbus(CORBA::ORB_var orb, const char* host, unsigned short port
                 , extension_detail::authentication_info info)
{
  state.reset(new extension_detail::shared_state(orb, info, host, port));
#ifdef EXTENSION_SDK_MULTITHREAD
  state->orb_thread = boost::thread(boost::bind(&extension::orb_thread_function, orb, state));
  state->work_thread = boost::thread(boost::bind(&extension::work_thread_function, state));
#endif
}

Openbus::~Openbus()
{
}

void Openbus::addOffer(scs::core::IComponent_var component, idl_or::ServicePropertySeq properties)
{
  boost::unique_lock<boost::mutex> l(state->mutex);
  state->queued_components.push_back(std::make_pair(component, properties));
  state->new_queued_components = true;
  state->work_cond_var.notify_one();
}

void Openbus::shutdown()
{
  state->orb->shutdown(true);
}

void Openbus::wait()
{
  state->work_thread.join();
}

idl_or::ServicePropertySeq Openbus::createFacetAndEntityProperty(const char* facet, const char* entity)
{
  idl_or::ServicePropertySeq properties;
  properties.length(2);
  properties[0].name = "openbus.component.facet";
  properties[0].value = facet;
  properties[1].name = "openbus.offer.entity";
  properties[1].value = entity;
  return properties;
}  

idl_or::ServiceOfferDescSeq Openbus::findOffers(idl_or::ServicePropertySeq properties, int timeout) const
{
  idl_or::ServicePropertySeq x;
  AssociativePropertySeq s(x);
  
  return idl_or::ServiceOfferDescSeq();
}

idl_or::ServiceOfferDescSeq Openbus::filterWorkingOffers(idl_or::ServiceOfferDescSeq offers)
{
  return offers;
}

} }
