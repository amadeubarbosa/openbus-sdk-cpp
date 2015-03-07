#include <boost/system/system_error.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <iostream>
#include <map>
#include <vector>

using namespace boost::process;
using namespace boost::process::initializers;

const std::string stage_interop("../stage-interop/");
typedef std::string flavor;
typedef std::string flavor_tag;
const std::map<flavor, flavor_tag> flavors = 
{
  {"multithread, debug and shared", "mt-d"},
  {"multithread, release and shared", "mt"},
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
    interop + "_" + process + "-" + flavor_tag + ".out");
  boost::filesystem::path path(stage_interop + interop + "/" +
                               interop + "_" + process + "-" +
                               flavor_tag + ".exe");
  std::cout << "-->Running " << type << " '" << process << "'." << std::endl;
  try
  {
    child c(
      execute(
        run_exe(path),
        throw_on_error(),
        start_in_dir(stage_interop + interop),
        bind_stdout(out_sink)));
    childs.push_back(std::move(c));
  }
  catch (const boost::system::system_error &e)
  {
    std::cout << e.code().message() << std::endl;
  }
}

void run_interop(
  const std::string &interop,
  const std::vector<std::string> &services,
  const std::string &client)
{
  for (auto flavor : flavors)
  {    
    std::cout << "->Running interop '" << interop << "' "
              << "with flavor " << flavor.first << "." << std::endl;
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
  {
    std::vector<std::string> simple_services = { "server" };
    run_interop("simple", simple_services, "client");
  }
  {
    std::vector<std::string> simple_services = { "server", "sharedauth" };
    run_interop("sharedauth", simple_services, "client");
  }
  {
    std::vector<std::string> simple_services = { "broadcaster", "forwarder",
                                                 "messenger" };
    run_interop("delegation", simple_services, "client");
  }
  {
    std::vector<std::string> simple_services = { "server" };
    run_interop("multiplexing", simple_services, "client");
  }
  {
    std::vector<std::string> reloggedjoin_services = { "server", "proxy" };
    run_interop("reloggedjoin", reloggedjoin_services, "client");
  }
  return 0; //MSVC
}
