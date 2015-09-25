// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "messagesC.h"
#pragma clang diagnostic pop
#include <util.hpp>
#include <openbus.hpp>
#include <scs/ComponentContext.h>
#include <log/output/file_output.h>

#include <tao/PortableServer/PortableServer.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#pragma clang diagnostic pop

void mysleep()
{
#ifndef _WIN32
  unsigned int t = 10u;
  do { t = sleep(t); } while(t);
#else
  Sleep(10000);
#endif
}

namespace delegation = tecgraf::openbus::interop::delegation;

const std::string entity("interop_delegation_cpp_client");
std::string bus_host, domain;
unsigned short bus_port;
bool debug;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("debug", po::value<bool>()->default_value(true) , "yes|no")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus")
    ("user.password.domain", po::value<std::string>()->default_value("testing"),
     "Password domain");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) 
  {
    std::cout << desc << std::endl;
    std::exit(1);
  }
  if (vm.count("debug"))
  {
    debug = vm["debug"].as<bool>();
  }
  if (vm.count("bus.host.name"))
  {
    bus_host = vm["bus.host.name"].as<std::string>();
  }
  if (vm.count("bus.host.port"))
  {
    bus_port = vm["bus.host.port"].as<unsigned short>();
  }
  if (vm.count("user.password.domain"))
  {
    domain = vm["user.password.domain"].as<std::string>();
  }
}

int main(int argc, char** argv) {
  try {
    load_options(argc, argv);
    if (debug)
    {
      openbus::log().set_level(openbus::debug_level);
    }
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(bus_host, bus_port));
    bus_ctx->setDefaultConnection(conn);
    
    conn->loginByPassword(entity, entity, domain);

    openbus::idl::offers::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name  = "openbus.component.interface";
    props[static_cast<CORBA::ULong>(1)].value = delegation::_tc_Messenger->id();

    openbus::idl::offers::ServiceOfferDescSeq_var offers = 
      find_offers(bus_ctx, props);
    if (offers->length() > 0)
    {
      CORBA::Object_var o(
        offers[static_cast<CORBA::ULong> (0)]
        .service_ref->getFacet(delegation::_tc_Messenger->id()));
      delegation::Messenger_var m = delegation::Messenger::_narrow(o);
      props[1].value = delegation::_tc_Forwarder->id();
      offers = find_offers(bus_ctx, props);
      if(offers->length() > 0)
      {
        o = offers[static_cast<CORBA::ULong> (0)]
          .service_ref->getFacet(delegation::_tc_Forwarder->id());
        delegation::Forwarder_var forwarder = delegation::Forwarder::_narrow(o);
        props[1].value = delegation::_tc_Broadcaster->id();
        offers = find_offers(bus_ctx, props);
        if(offers->length() > 0)
        {
          o = offers[static_cast<CORBA::ULong> (0)]
            .service_ref->getFacet(delegation::_tc_Broadcaster->id());
          delegation::Broadcaster_var broadcaster(
            delegation::Broadcaster::_narrow(o));
          conn->logout();

          conn->loginByPassword("bill", "bill", domain);
          forwarder->setForward("willian");
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("paul", "paul", domain);
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("mary", "mary", domain);
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("steve", "steve", domain);
          broadcaster->subscribe();
          broadcaster->post("Testing the list!");
          conn->logout();

          mysleep();

          const char* names[] = {"willian", "bill", "paul", "mary", "steve"};
          for(const char** first = &names[0]; 
              first != &names[sizeof(names)/sizeof(names[0])];
              ++first)
          {
            conn->loginByPassword(*first, *first, domain);
            delegation::PostDescSeq_var posts(m->receivePosts());
            std::cout << "user "
                      << std::string(*first)
                      << " got posts: "
                      << posts->length()
                      << std::endl;
            if (std::string(*first) != "bill" && posts->length() != 1)
            {
              std::cerr << "Error: Retrieving messages for '"
                        << *first << "': posts.length() != 1"
                        << std::endl;
              std::abort();
            }
            else if (std::string(*first) == "bill" && posts->length() != 0)
            {
              std::cerr << "Error: Retrieving messages for 'bill' \
                            : posts.length() != 0"
                        << std::endl;
              std::abort();
              
            }
            for(CORBA::ULong i(0); i != posts->length(); ++i)
            {
              if (std::string(*first) != "willian"
                  &&
                  !boost::regex_match(
                    posts[i].from.in(),
                    boost::regex("steve->interop_delegation_.+_broadcaster")))
              {
                std::cerr << "Error: steve->interop_delegation_*_broadcaster"
                          << " != " << posts[i].from.in()
                          << std::endl;
                std::abort();
              }
              else if(std::string(*first) == "willian"
                      &&
                      !boost::regex_match(
                        posts[i].from.in(),
                        boost::regex("interop_delegation_.+_forwarder")))
              {
                std::cerr << "Error: interop_delegation_*_forwarder"
                          << " != " << posts[i].from.in()
                          << std::endl;
                std::abort();
              }                
              std::cout << "message(" << i << ") (from) " << (*posts)[i].from 
                        << " (body) " << (*posts)[i].message << std::endl;
            }
            broadcaster->unsubscribe();
            conn->logout();
          }

          conn->loginByPassword("bill", "bill", domain);
          forwarder->cancelForward("willian");
          conn->logout();
        }
      }
    } 
    else
    {
      std::cout << "nenhuma oferta encontrada." << std::endl;
    }
  } 
  catch(std::exception const& e) 
  {
    std::cout << "[error (std::exception)] " << e.what() << std::endl;
    return -1;
  } 
  catch (const CORBA::Exception& e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } 
  catch (...) 
  {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0; //MSVC
}
