// -*- coding: iso-8859-1-unix -*-

#include <mockC.h>
#include <openbus.hpp>
#include <util.hpp>
#include <boost/program_options.hpp>
#include <boost/tuple/tuple.hpp>

const std::string entity("interop_protocol_cpp_client");
std::string bus_host;
unsigned short bus_port;

using namespace tecgraf::openbus::interop;

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

typedef CORBA::ULong session;
typedef protocol::Server::EncryptedData &challenge;
typedef CORBA::ULong expected;
typedef boost::tuple<session, challenge, expected> credential_reset_case;
std::vector<credential_reset_case> credential_reset_cases;

typedef CORBA::ULong raised;
typedef CORBA::ULong expected;
typedef boost::tuple<raised, expected> no_permission_case;
std::vector<no_permission_case> no_permission_cases;

int main(int argc, char **argv)
{
  load_options(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(
    openbus::ORBInitializer(argc, argv));
  openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
  std::auto_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(bus_host, bus_port));
  bus_ctx->setDefaultConnection(conn.get());
  conn->loginByPassword(entity, entity, "testing");

  openbus::idl::offers::ServicePropertySeq props;
  props.length(2);
  props[0u].name = "offer.domain";
  props[0u].value = "Interoperability Tests";
  props[1u].name = "openbus.component.interface";
  props[1u].value = "IDL:tecgraf/openbus/interop/protocol/Server:1.0";

  protocol::Server::EncryptedData encrypted_data;
  std::memset(encrypted_data, '\171', 256);
  credential_reset_cases.push_back(
    credential_reset_case(
      4294967295, encrypted_data, openbus::idl::access::InvalidRemoteCode));

  no_permission_cases.push_back(no_permission_case(0, 0));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::InvalidCredentialCode,
      openbus::idl::access::InvalidRemoteCode));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::InvalidChainCode,
      openbus::idl::access::InvalidChainCode));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::UnverifiedLoginCode,
      openbus::idl::access::UnverifiedLoginCode));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::UnknownBusCode,
      openbus::idl::access::UnknownBusCode));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::InvalidPublicKeyCode,
      openbus::idl::access::InvalidPublicKeyCode));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::NoCredentialCode,
      openbus::idl::access::NoCredentialCode));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::NoLoginCode,
      openbus::idl::access::InvalidRemoteCode));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::InvalidRemoteCode,
      openbus::idl::access::InvalidRemoteCode));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::UnavailableBusCode,
      openbus::idl::access::InvalidRemoteCode));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::InvalidTargetCode,
      openbus::idl::access::InvalidRemoteCode));
  no_permission_cases.push_back(
    no_permission_case(
      openbus::idl::access::InvalidLoginCode,
      openbus::idl::access::InvalidRemoteCode));
  
  openbus::idl::offers::ServiceOfferDescSeq_var offers(
    find_offers(bus_ctx, props));
  for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
  {
    CORBA::Object_var o(offers[idx].service_ref->getFacetByName("Server"));
    protocol::Server *server(protocol::Server::_narrow(o));
    server->NonBusCall();
    for (std::vector<credential_reset_case>::iterator it(
           credential_reset_cases.begin());
         it != credential_reset_cases.end(); ++it)
    {
      try
      {
        server->ResetCredentialWithChallenge((*it).get<0>(), (*it).get<1>());
        std::abort();
      }
      catch (const CORBA::NO_PERMISSION &e)
      {
        if (e.minor() != (*it).get<2>()) std::abort();
        if (e.completed() != CORBA::COMPLETED_NO) std::abort();
      }
    }

    for (std::vector<no_permission_case>::iterator it(
           no_permission_cases.begin());
         it != no_permission_cases.end(); ++it)
    {
      try
      {
        server->RaiseNoPermission((*it).get<0>());
        std::abort();
      }
      catch (const CORBA::NO_PERMISSION &e)
      {
        if (e.minor() != (*it).get<1>()) std::abort();
        if (e.completed() != CORBA::COMPLETED_NO) std::abort();        
      }
    }
}

  return 0; //MSVC
}
