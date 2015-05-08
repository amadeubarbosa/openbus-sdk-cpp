// -*- coding: iso-8859-1-unix -*-

#include "messagesC.h"
#include <util.hpp>
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>
#include <scs/ComponentContext.h>
#include <log/output/file_output.h>

#include <tao/PortableServer/PortableServer.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <boost/program_options.hpp>

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
std::string bus_host;
unsigned short bus_port;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) 
  {
    std::cout << desc << std::endl;
    std::exit(1);
  }
  if (vm.count("bus.host.name"))
  {
    bus_host = vm["bus.host.name"].as<std::string>();
  }
  if (vm.count("bus.host.port"))
  {
    bus_port = vm["bus.host.port"].as<unsigned short>();
  }
}

int main(int argc, char** argv) {
  try {
    load_options(argc, argv);
    openbus::log().set_level(openbus::debug_level);
    openbus::orb_ctx orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    std::auto_ptr <openbus::Connection> conn(
      bus_ctx->createConnection(bus_host, bus_port));
    bus_ctx->setDefaultConnection(conn.get());
    
    conn->loginByPassword(entity, entity);

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name  = "openbus.component.interface";
    props[static_cast<CORBA::ULong>(1)].value = delegation::_tc_Messenger->id();

    openbus::idl_or::ServiceOfferDescSeq_var offers = 
      find_offers(bus_ctx, props);
    if (offers->length() > 0)
    {
      CORBA::Object_var o(
        offers[static_cast<CORBA::ULong> (0)]
        .service_ref->getFacetByName("messenger"));
      delegation::Messenger_var m = delegation::Messenger::_narrow(o);
      props[1].value = delegation::_tc_Forwarder->id();
      offers = find_offers(bus_ctx, props);
      if(offers->length() > 0)
      {
        o = offers[static_cast<CORBA::ULong> (0)]
          .service_ref->getFacetByName("forwarder");
        delegation::Forwarder_var forwarder = delegation::Forwarder::_narrow(o);
        props[1].value = delegation::_tc_Broadcaster->id();
        offers = find_offers(bus_ctx, props);
        if(offers->length() > 0)
        {
          o = offers[static_cast<CORBA::ULong> (0)]
            .service_ref->getFacetByName("broadcaster");
          delegation::Broadcaster_var broadcaster(
            delegation::Broadcaster::_narrow(o));
          conn->logout();

          conn->loginByPassword("bill", "bill");
          forwarder->setForward("willian");
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("paul", "paul");
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("mary", "mary");
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("steve", "steve");
          broadcaster->subscribe();
          broadcaster->post("Testing the list!");
          conn->logout();

          mysleep();

          const char* names[] = {"willian", "bill", "paul", "mary", "steve"};
          for(const char** first = &names[0]; 
              first != &names[sizeof(names)/sizeof(names[0])];
              ++first)
          {
            conn->loginByPassword(*first, *first);
            delegation::PostDescSeq_var posts(m->receivePosts());
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
                  && (std::string("steve->interop_delegation_cpp_broadcaster")
                      != posts[i].from.in()))
              {
                std::cerr << "Error: steve->interop_delegation_cpp_broadcaster"
                          << " != " << posts[i].from.in()
                          << std::endl;
                std::abort();
              }
              else if(std::string(*first) == "willian"
                  && (std::string("interop_delegation_cpp_forwarder")
                      != posts[i].from.in()))
              {
                std::cerr << "Error: interop_delegation_cpp_forwarder"
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

          conn->loginByPassword("bill", "bill");
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
