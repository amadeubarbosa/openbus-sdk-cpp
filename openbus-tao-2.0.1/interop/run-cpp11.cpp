// -*- coding: iso-8859-1-unix -*-

#include <boost/system/system_error.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
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

const std::map<flavor, flavor_tag> flavors = 
{
  {"multithread, debug and shared", "mt-d"},
  {"multithread, release and shared", "mt"},
  {"multithread, debug and static", "mt-s-d"},
  {"multithread, release and static", "mt-s"},
};

void exec(
  const std::string &interop,
  const flavor_tag &flavor_tag,
  const std::string &type,
  const std::string &process,
  std::vector<child> &childs)
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
  auto exe_path(current_path());
  exe_path += rel_path;
  auto start_path(current_path());
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
    auto child(
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
    childs.push_back(std::move(child));
  }
  catch (const boost::system::system_error &e)
  {
    std::cout << exe_path << ":" << e.code().message() << std::endl;
  }
}

void run_interop(
  const std::string &interop,
  const std::vector<std::string> &services,
  const std::vector<path> &tmp_files = {})
{
  for (auto flavor : flavors)
  {    
    std::cout << "->Running interop '" << interop << "' "
              << "with flavor " << flavor.first << "." << std::endl;
    for (auto tmp_file : tmp_files)
    {
      remove(tmp_file);
    }
    std::vector<child> service_childs;
    for (auto service : services)
    {
      exec(interop, flavor.second, "service", service, service_childs);
    }
    std::vector<child> client_childs;
    exec(interop, flavor.second, "client", "client", client_childs);
    for (auto &child : client_childs)
    {
      wait_for_exit(child);
    }
    for (auto &child : service_childs)
    {
      terminate(child);
    }
  }
}

int main()
{
  run_interop("simple", {"server"});
  run_interop("sharedauth", { "server", "sharedauth" },
              { "stage-interop/sharedauth/.secret",
		"stage-interop/sharedauth/.secret.lock" });
  run_interop("delegation", { "broadcaster", "forwarder", "messenger" });
  run_interop("multiplexing", { "server" });
  run_interop("reloggedjoin", { "server", "proxy" });
  return 0; //MSVC
}
