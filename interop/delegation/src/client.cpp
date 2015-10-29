// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "messagesC.h"
#pragma clang diagnostic pop
#include <util.hpp>
#include <config.hpp>
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
#include <boost/regex.hpp>
#pragma clang diagnostic pop

namespace delegation = tecgraf::openbus::interop::delegation;
namespace cfg = openbus::test::config;

void mysleep()
{
#ifndef _WIN32
  unsigned int t(cfg::login_lease_time);
  do { t = sleep(t); } while(t);
#else
  Sleep(cfg::login_lease_time * 1000);
#endif
}

const std::string entity("interop_delegation_cpp_client");

int main(int argc, char** argv) {
  try {
    cfg::load_options(argc, argv);
    if (cfg::openbus_test_verbose)
    {
      openbus::log().set_level(openbus::debug_level);
    }
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
    bus_ctx->setDefaultConnection(conn);
    
    conn->loginByPassword(entity, entity, cfg::user_password_domain);

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

          conn->loginByPassword("bill", "bill", cfg::user_password_domain);
          forwarder->setForward("willian");
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("paul", "paul", cfg::user_password_domain);
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("mary", "mary", cfg::user_password_domain);
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("steve", "steve", cfg::user_password_domain);
          broadcaster->subscribe();
          broadcaster->post("Testing the list!");
          conn->logout();

          mysleep();

          const char* names[] = {"willian", "bill", "paul", "mary", "steve"};
          for(const char** first = &names[0]; 
              first != &names[sizeof(names)/sizeof(names[0])];
              ++first)
          {
            conn->loginByPassword(*first, *first, cfg::user_password_domain);
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

          conn->loginByPassword("bill", "bill", cfg::user_password_domain);
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
