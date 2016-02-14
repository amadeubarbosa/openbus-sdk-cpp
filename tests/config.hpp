// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_TESTS_CONFIG_HPP
#define TECGRAF_SDK_OPENBUS_TESTS_CONFIG_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <tao/ORB.h>
#include <tao/LocalObject.h>
#pragma clang diagnostic pop
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/program_options.hpp>
#pragma clang diagnostic pop
#include <fstream>
#include <iostream>
#include <cstdlib>

namespace openbus { namespace tests { namespace config
{
  
std::string bus_host_name,
  bus2_host_name,
  bus_reference_path,
  bus2_reference_path,
  system_private_key,
  user_password_domain,
  user_entity_name,
  user_password,
  system_entity_name,
  system_sharedauth;
  
unsigned short bus_host_port,
  bus2_host_port,
  login_lease_time,
  password_penalty_time,
  password_penalty_tries;
  
bool openbus_test_verbose;

CORBA::Object_var _get_bus_ref(CORBA::ORB_ptr orb, std::string bus_ref)
{
  std::ifstream bus_ior_file(bus_ref.c_str());
  if (!bus_ior_file)
  {
    std::cerr << "Nao foi possivel abrir o arquivo com o IOR do barramento."
              << std::endl;
    return CORBA::Object::_nil();
  }
  std::stringstream stream;
  stream << bus_ior_file.rdbuf();
  std::string bus_ior(stream.str());
  return orb->string_to_object(bus_ior.c_str());
}
    
CORBA::Object_var get_bus_ref(CORBA::ORB_ptr orb)
{
  return _get_bus_ref(orb, bus_reference_path);
}

CORBA::Object_var get_bus2_ref(CORBA::ORB_ptr orb)
{
  return _get_bus_ref(orb, bus2_reference_path);
}

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description generic("Command line options");
  generic.add_options()("help", "Help");
  
  po::options_description config("Configuration");
  config.add_options()
    ("openbus.test.verbose"
     ,po::value<bool>()->default_value(true)
     ,"yes|no")
    ("login.lease.time"
     ,po::value<unsigned short>()->default_value(1)
     ,"")
    ("bus.host.name"
     ,po::value<std::string>()->default_value("localhost")
     ,"Host to OpenBus")
    ("bus.host.port"
     ,po::value<unsigned short>()->default_value(2089)
     ,"Port to OpenBus")
    ("bus.reference.path"
     ,po::value<std::string>()->default_value("BUS01.ior")
     ,"")
    ("bus2.host.name"
     ,po::value<std::string>()->default_value("localhost")
     ,"Host to OpenBus")
    ("bus2.host.port"
     ,po::value<unsigned short>()->default_value(2090)
     ,"Port to OpenBus")
    ("bus2.reference.path"
     ,po::value<std::string>()->default_value("BUS02.ior")
     ,"")
    ("user.password.domain"
     ,po::value<std::string>()->default_value("testing")
     ,"Domain")
    ("user.entity.name"
     ,po::value<std::string>()->default_value("testuser")
     ,"")
    ("user.password"
     ,po::value<std::string>()->default_value("testuser")
     ,"")
    ("password.penalty.time"
     ,po::value<unsigned short>()->default_value(1)
     ,"")
    ("password.penalty.tries"
     ,po::value<unsigned short>()->default_value(3)
     ,"")
    ("system.entity.name"
     ,po::value<std::string>()->default_value("testsyst")
     ,"")
    ("system.private.key"
     ,po::value<std::string>()->default_value("testsyst.key")
     ,"Path to system private key")
    ("system.sharedauth"
     ,po::value<std::string>()->default_value("sharedauth.dat")
     ,"");

  po::options_description cmdline_opts;
  cmdline_opts.add(generic).add(config);
  
  po::options_description cfgfile_opts;
  cfgfile_opts.add(config);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
            .options(cmdline_opts).allow_unregistered().run(), vm);
  
  if (vm.count("help")) 
  {
    std::cout << cmdline_opts << std::endl;
    std::exit(0);
  }

  const char *cfgfile(std::getenv("OPENBUS_TESTCFG"));
  if (cfgfile == 0)
    cfgfile = "test.properties";
  
  std::ifstream ifs(cfgfile);
  if (ifs)
  {
    po::store(po::parse_config_file(ifs, cfgfile_opts, true), vm);
    po::notify(vm);
  }

  if (vm.count("openbus.test.verbose"))
    openbus_test_verbose = vm["openbus.test.verbose"].as<bool>();
  if (vm.count("login.lease.time"))
    login_lease_time = vm["login.lease.time"].as<unsigned short>();
  if (vm.count("bus.host.name"))
    bus_host_name = vm["bus.host.name"].as<std::string>();
  if (vm.count("bus.host.port"))
    bus_host_port = vm["bus.host.port"].as<unsigned short>();
  if (vm.count("bus.reference.path"))
    bus_reference_path = vm["bus.reference.path"].as<std::string>();
  if (vm.count("bus2.host.name"))
    bus2_host_name = vm["bus2.host.name"].as<std::string>();
  if (vm.count("bus2.host.port"))
    bus2_host_port = vm["bus2.host.port"].as<unsigned short>();
  if (vm.count("bus2.reference.path"))
    bus2_reference_path = vm["bus2.reference.path"].as<std::string>();
  if (vm.count("user.password.domain"))
    user_password_domain = vm["user.password.domain"].as<std::string>();
  if (vm.count("user.entity.name"))
    user_entity_name = vm["user.entity.name"].as<std::string>();
  if (vm.count("user.password"))
    user_password = vm["user.password"].as<std::string>();
  if (vm.count("password.penalty.time"))
    password_penalty_time = vm["password.penalty.time"].as<unsigned short>();
  if (vm.count("password.penalty.tries"))
    password_penalty_tries = vm["password.penalty.tries"].as<unsigned short>();
  if (vm.count("system.entity.name"))
    system_entity_name = vm["system.entity.name"].as<std::string>();
  if (vm.count("system.private.key"))
    system_private_key = vm["system.private.key"].as<std::string>();
  if (vm.count("system.sharedauth"))
    system_sharedauth = vm["system.sharedauth"].as<std::string>();
}

}}}
#endif
