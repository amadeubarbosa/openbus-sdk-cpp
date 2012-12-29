#ifndef TECGRAF_TESTS_CONFIGURATION_H_
#define TECGRAF_TESTS_CONFIGURATION_H_

#include <boost/lexical_cast.hpp>

#include <openbus/log.hpp>

namespace openbus {

struct configuration
{
  configuration(int argc, char** argv)
    : port_(2089), host_("localhost"), user_("test")
    , password_("test"), certificate_user_("TestCppHello")
  {
    assert(argc >= 1);
    int i = 1;
    while(i != argc)
    {
      if(!std::strcmp("-port", argv[i]))
      {
        if(++i != argc)
        {
          try
          {
            port_ = boost::lexical_cast<unsigned short>(argv[i]);
          }
          catch(std::exception const& e)
          {
            std::cout << "Invalid argument for -port: " << argv[i] << std::endl;
          }
        }
        else
          std::cout << "Command -port requires an argument" << std::endl;
      }
      else if(!std::strcmp("-wrong-port", argv[i]))
      {
        if(++i != argc)
        {
          try
          {
            wrong_port_ = boost::lexical_cast<unsigned short>(argv[i]);
          }
          catch(std::exception const& e)
          {
            std::cout << "Invalid argument for -wrong-port: " << argv[i] << std::endl;
          }
        }
        else
          std::cout << "Command -wrong-port requires an argument" << std::endl;
      }
      else if(!std::strcmp("-host", argv[i]))
      {
        if(++i != argc)
          host_ = argv[i];
        else
          std::cout << "Command -host requires an argument" << std::endl;
      }
      else if(!std::strcmp("-user", argv[i]))
      {
        if(++i != argc)
          user_ = argv[i];
        else
          std::cout << "Command -user requires an argument" << std::endl;
      }
      else if(!std::strcmp("-password", argv[i]))
      {
        if(++i != argc)
          password_ = argv[i];
        else
          std::cout << "Command -password requires an argument" << std::endl;
      }
      else if(!std::strcmp("-certificate-user", argv[i]))
      {
        if(++i != argc)
          certificate_user_ = argv[i];
        else
          std::cout << "Command -certificate-user requires an argument" << std::endl;
      }
      else if(!std::strcmp("-key", argv[i]))
      {
        if(++i != argc)
          key_ = argv[i];
        else
          std::cout << "Command -key requires an argument" << std::endl;
      }
      else if(!std::strcmp("-log", argv[i]))
      {
        if(++i != argc)
        {
          if(!std::strcmp("error", argv[i]))
          {
            openbus::log.set_level(openbus::error_level);
          }
          else if(!std::strcmp("warning", argv[i]))
          {
            openbus::log.set_level(openbus::warning_level);
          }
          else if(!std::strcmp("info", argv[i]))
          {
            openbus::log.set_level(openbus::info_level);
          }
          else if(!std::strcmp("debug", argv[i]))
          {
            openbus::log.set_level(openbus::debug_level);
          }
          else
          {
            std::cout << "Invalid argument for option -logg: " << argv[i] << std::endl;
          }
        }
        else
          std::cout << "Command -log requires an argument" << std::endl;
      }
      ++i;
    }
  }

  unsigned short port() const
  {
    return port_;
  }
  unsigned short wrong_port() const
  {
    return wrong_port_;
  }
  std::string host() const
  {
    return host_;
  }

  std::string user() const
  {
    return user_;
  }
  std::string password() const
  {
    return password_;
  }
  std::string certificate_user() const
  {
    return certificate_user_;
  }
  std::string key() const
  {
    return key_;
  }

private:
  unsigned short port_, wrong_port_;
  std::string host_, user_, password_, certificate_user_, key_;
};

}

#endif
