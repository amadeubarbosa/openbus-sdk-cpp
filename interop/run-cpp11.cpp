// -*- coding: iso-8859-1-unix -*-

#include <boost/system/system_error.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <set>

using namespace boost::process;
using namespace boost::process::initializers;
using namespace boost::filesystem;

const std::string legacy_sdk_path(
  current_path().generic_string() + "/../openbus-legacy");
const std::string current_sdk_path(current_path().generic_string());

const std::string stage_interop("/stage-interop/");
typedef std::string flavor;
typedef std::string flavor_tag;

const std::map<flavor, flavor_tag> flavors = 
{
  // {"multithread, debug and shared", "mt-d"},
  // {"multithread, release and shared", "mt"},
  {"multithread, debug and static", "mt-s-d"},
  {"multithread, release and static", "mt-s"},
};

child exec(
  const std::string &interop,
  const flavor_tag &flavor_tag,
  const std::string &process,
  const std::string &sdk_path)
{
  boost::system::error_code ec;
  boost::iostreams::file_descriptor_sink out_sink(
    sdk_path +
    stage_interop + interop + "/" +
    interop + "_" + process + "-" + flavor_tag + ".out");
  boost::iostreams::file_descriptor_sink err_sink(
    sdk_path +
    stage_interop + interop + "/" +
    interop + "_" + process + "-" + flavor_tag + ".err");
  path rel_path(stage_interop + interop + "/" +
                interop + "_" + process + "-" +
                flavor_tag
#ifdef _WIN32
                + ".exe"
#endif
    );
  auto exe_path(sdk_path);
  exe_path += rel_path.generic_string();
  auto start_path(current_sdk_path);
  std::cout << "-->Running '" << exe_path << "'." << std::endl;
#ifndef _WIN32
  std::set<std::string> env;
  env.insert("DYLD_LIBRARY_PATH=" +
             sdk_path + "/install/deps");
  env.insert("LD_LIBRARY_PATH=" +
             sdk_path + "/install/deps");
  env.insert("OPENBUS_TESTCFG=" +
             current_sdk_path + "/test.properties");
#endif
  try
  {
    auto child(
      execute(
        run_exe(exe_path),
        throw_on_error(),
        start_in_dir(start_path),
        bind_stdout(out_sink),
        bind_stderr(err_sink)
#ifdef _WIN32
        ));
#else
    ,set_env(env)));
#endif
    return std::move(child);
  }
  catch (const boost::system::system_error &e)
  {
    std::cout << exe_path << ":" << e.code().message() << std::endl;
  }
}

typedef std::string service_name;
typedef std::string sdk_path;
typedef std::pair<service_name, sdk_path> service_exec;

void remove_tmp_files(
  const std::vector<path> &tmp_files)
{
  for (auto tmp_file : tmp_files)
  {
    remove(tmp_file);
  }
}

void run_interop(
  const flavor_tag &tag,
  const std::string &interop,
  std::vector<std::string> services,
  const std::vector<path> &tmp_files,
  std::vector<service_exec> execs,
  std::vector<child> &service_childs,
  const std::string &client)
{
  if (!services.empty())
  {
    auto service(services.back());
    services.pop_back();
    execs.push_back(service_exec(service, current_sdk_path));
    service_childs.push_back(exec(interop, tag, service, current_sdk_path));
    run_interop(tag, interop, services, tmp_files, execs, service_childs, client);

    execs.pop_back();

    for (auto e : execs)
    {
      service_childs.push_back(exec(interop, tag, e.first, e.second));
    }
    
    execs.push_back(service_exec(service, legacy_sdk_path));
    service_childs.push_back(exec(interop, tag, service, legacy_sdk_path));
    run_interop(tag, interop, services, tmp_files, execs, service_childs, client);
    execs.pop_back();
    return;
  }
  std::vector<child> client_childs;
  try
  {
    wait_for_exit(exec(interop, tag, client, current_sdk_path));
  
    for (auto &child : service_childs)
    {
      terminate(child);
    }
  } catch (boost::system::system_error &)
  {
  }
  service_childs.clear();
  std::cout << std::endl;
  remove_tmp_files(tmp_files);
}

void run_interop(
  const std::string &interop,
  const std::vector<std::string> &services,
  const std::string &client = "client",
  const std::vector<path> &tmp_files = {})
{
  for (auto flavor : flavors)
  {    
    std::cout << std::endl << "->Interop '"
              << interop << "' "
              << "with flavor "
              << flavor.first << "." << std::endl;
    std::vector<service_exec> execs = {};
    std::vector<child> childs = {};
    remove_tmp_files(tmp_files);
    run_interop(flavor.second, interop, services, tmp_files, execs, childs,
                client);
  }
}

int main()
{
  run_interop("simple", {"server",});
  run_interop("sharedauth", {"sharing"}, "consuming",
              {"sharedauth.dat"});
  run_interop("multiplexing", { "server" });
  run_interop("reloggedjoin", {"server", "proxy"});
  run_interop("delegation", { "broadcaster", "forwarder", "messenger" });
  return 0; //MSVC
}
