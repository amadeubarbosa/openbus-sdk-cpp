// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "messagesS.h"
#pragma clang diagnostic pop
#include <util.hpp>
#include <tests/config.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>
#include <scs/ComponentContext.h>
#include <log/output/file_output.h>

#include <iostream>
#include <map>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#pragma clang diagnostic pop

namespace delegation = tecgraf::openbus::interop::delegation;
namespace cfg = openbus::tests::config;

void ORBRun(CORBA::ORB_var orb)
{
  orb->run();
}

struct MessengerImpl : 
  virtual public POA_tecgraf::openbus::interop::delegation::Messenger
{
  MessengerImpl(openbus::OpenBusContext& c)
    : ctx(c) {}

  void post(const char* to, const char* message) 
  {
    openbus::CallerChain chain(ctx.getCallerChain());
    std::string from;
    for(CORBA::ULong i(0); i < chain.originators().length(); ++i)
    {
      from = from + chain.originators()[i].entity.in() + "->";
    }
    from += chain.caller().entity.in();
    std::cout << "Post to " << to << " by " << from << std::endl;
    std::cout << " Message content: " << message << std::endl;
    delegation::PostDesc desc = {from.c_str(), message};
    inbox.insert(std::make_pair(to, desc));
  }

  delegation::PostDescSeq* receivePosts()
  {
    openbus::CallerChain chain(ctx.getCallerChain());
    std::string owner(chain.caller().entity);
    if (chain.originators().length() > 0)
    {
      owner = chain.originators()[0].entity;
    } 
    std::string by;
    for(CORBA::ULong i(0); i < chain.originators().length(); ++i)
    {
      by = by + chain.originators()[i].entity.in() + "->";
    }
    by += chain.caller().entity.in();
    std::cout << "Retrieving messages of " << owner
              << " by " << by
              << std::endl;
    typedef std::multimap<std::string, delegation::PostDesc>::iterator
      iterator;
    std::pair<iterator, iterator> range = inbox.equal_range(owner);
    delegation::PostDescSeq_var posts (new delegation::PostDescSeq);
    std::cout << "Retrieving " << std::distance(range.first, range.second) 
              << " messages" << std::endl;
    posts->length(static_cast<CORBA::ULong>(
                    std::distance(range.first, range.second)));
    CORBA::ULong index(0);
    for(iterator first = range.first; first != range.second; ++first, ++index)
    {
      (*posts)[index] = first->second;
    }
    inbox.erase(range.first, range.second);
    return posts._retn();
  }

  openbus::OpenBusContext& ctx;
  std::multimap<std::string, delegation::PostDesc> inbox;
};

const std::string entity("interop_delegation_cpp_messenger");

int main(int argc, char** argv) {
  try {
    cfg::load_options(argc, argv);
    if (cfg::openbus_test_verbose)
    {
      openbus::log()->set_level(openbus::debug_level);
    }
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    CORBA::Object_var bus_ref(cfg::get_bus_ref(orb_ctx->orb()));
    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByReference(bus_ref));
    bus_ctx->setDefaultConnection(conn);

    boost::thread orb_run(ORBRun, bus_ctx->orb());

    scs::core::ComponentId componentId;
    componentId.name = "Messenger";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "C++";
    scs::core::ComponentContext
      messenger_component(bus_ctx->orb(), componentId);
    MessengerImpl messenger_servant(*bus_ctx);
    messenger_component.addFacet(
      "messenger", delegation::_tc_Messenger->id(), &messenger_servant);

    openbus::idl::offers::ServicePropertySeq props;
    props.length(1);
    openbus::idl::offers::ServiceProperty property;
    props[static_cast<CORBA::ULong>(0)].name = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";

    EVP_PKEY *priv_key(
      openbus::demo::openssl::read_priv_key(cfg::system_private_key));
    if (!priv_key)
    {
      std::cerr << "Chave privada invalida." << std::endl;
      return -1;
    }
    conn->loginByCertificate(entity, priv_key);

    bus_ctx->getOfferRegistry()->registerService(
      messenger_component.getIComponent(), props);
    std::cout << "Messenger no ar" << std::endl;

    orb_run.join();
  } 
  catch(const std::exception &e) 
  {
    std::cerr << "[error (std::exception)] " << e.what() << std::endl;
    return -1;
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cerr << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } 
  catch (...) 
  {
    std::cerr << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0; //MSVC
}
