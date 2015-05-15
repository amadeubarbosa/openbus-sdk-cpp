// -*- coding: iso-8859-1-unix -*-

#include <boost/system/system_error.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/container/vector.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <set>

using namespace boost::process;
using namespace boost::process::initializers;
using namespace boost::filesystem;

const std::string stage_interop("/stage-interop/");
typedef std::string flavor;
typedef std::string flavor_tag;
typedef std::map<flavor, flavor_tag> flavors_t;
typedef std::map<flavor, flavor_tag>::iterator it_flavors_t;
flavors_t flavors;

typedef boost::container::vector<child> childs_t;
typedef boost::container::vector<child>::iterator it_childs_t;

void exec(
  const std::string &interop,
  const flavor_tag &flavor_tag,
  const std::string &type,
  const std::string &process,
  childs_t &childs)
{
  boost::system::error_code ec;
  boost::iostreams::file_descriptor_sink out_sink(
    current_path().generic_string() +
    stage_interop + interop + "/" +
    interop + "_" + process + "-" + flavor_tag + ".out");
  path rel_path(stage_interop + interop + "/" +
                                   interop + "_" + process + "-" +
                                   flavor_tag
#ifdef _WIN32
                                   + ".exe"
#endif
    );
  path exe_path(current_path());
  exe_path += rel_path;
  path start_path(current_path());
  start_path += path(stage_interop + interop);
  std::cout << "-->Running " << type << " '" << process << "'." << std::endl;
#ifndef _WIN32
  std::set<std::string> env;
  env.insert("DYLD_LIBRARY_PATH=" +
             current_path().generic_string() + "/install/deps");
  env.insert("LD_LIBRARY_PATH=" +
             current_path().generic_string() + "/install/deps");
#endif
  try
  {
    child c(
      execute(
        run_exe(exe_path),
        throw_on_error(),
        start_in_dir(start_path.generic_string()),
        bind_stdout(out_sink)
#ifdef _WIN32
        ));
#else
    ,set_env(env)));
#endif
    childs.push_back(boost::move(c));
  }
  catch (const boost::system::system_error &e)
  {
    std::cout << exe_path << ":" << e.code().message() << std::endl;
  }
}

void run_interop(
  const std::string &interop,
  const std::vector<std::string> &services,
  const std::vector<path> &tmp_files = std::vector<path>())
{
  for (it_flavors_t flavor(flavors.begin());flavor != flavors.end(); ++flavor)
  {    
    std::cout << "->Running interop '" << interop << "' "
              << "with flavor " << (*flavor).first << "." << std::endl;
    childs_t service_childs;
    for (std::vector<path>::const_iterator path(tmp_files.begin());
         path != tmp_files.end(); ++path)
    {
      remove(*path);
    }
    for (std::vector<std::string>::const_iterator service(services.begin());
         service != services.end(); ++service)
    {
      exec(interop, (*flavor).second, "service", (*service), service_childs);
    }
    childs_t client_childs;
    exec(interop, (*flavor).second, "client", "client", client_childs);
    for (it_childs_t child(client_childs.begin());
         child != client_childs.end(); ++child)
    {
      wait_for_exit(*child);
    }
    for (it_childs_t child(service_childs.begin());
         child != service_childs.end(); ++child)
    {
      terminate(*child);
    }
  }
}

int main()
{
  flavors["multithread, debug and shared"] = "mt-d";
  flavors["multithread, release and shared"] = "mt";
  flavors["multithread, debug and static"] = "mt-s-d";
  flavors["multithread, release and static"] = "mt-s";
  {
    std::vector<std::string> services;
    services.push_back("server");
    run_interop("simple", services);
  }
  {
    std::vector<std::string> services;
    services.push_back("server");
    services.push_back("sharedauth");
    std::vector<path> tmp_files;
    tmp_files.push_back("stage-interop/sharedauth/.secret");
    run_interop("sharedauth", services, tmp_files);
  }
  {
    std::vector<std::string> services;
    services.push_back("broadcaster");
    services.push_back("forwarder");
    services.push_back("messenger");
    run_interop("delegation", services);
  }
  {
    std::vector<std::string> services;
    services.push_back("server");
    run_interop("multiplexing", services);
  }
  {
    std::vector<std::string> services;
    services.push_back("server");
    services.push_back("proxy");
    run_interop("reloggedjoin", services);
  }
  return 0; //MSVC
}
