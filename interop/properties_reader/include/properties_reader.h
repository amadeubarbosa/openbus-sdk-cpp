#ifndef OPENBUS_CPP_INTEROP_PROPERTIES_READER_H
#define OPENBUS_CPP_INTEROP_PROPERTIES_READER_H

#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/utility.hpp>

struct properties
{
  properties()
  {
    std::ifstream config("test.properties");

    if(!config.is_open())
      throw std::runtime_error("Couldn't open properties 'test.properties' file");

    std::string line;
    while(std::getline(config, line))
    {
      std::string::iterator
        split_iterator = std::find(line.begin(), line.end(), '=');
      if(split_iterator != line.end())
      {
        std::string name(line.begin(), split_iterator);
        boost::trim(name);
        std::string value(boost::next(split_iterator), line.end());
        boost::trim(value);
        match(name, value);
      }
    }

    for(std::vector<bus>::const_iterator first = buses.begin()
          , last = buses.end(); first != last; ++first)
    {
      if(first->host.empty() || first->port == 0)
        throw std::runtime_error("Error in properties file. Some properties of a bus weren't set");
    }
  }

  struct bus
  {
    std::string host;
    unsigned short port;
    bus() : port(0) {}
  };

  std::string openbus_log_file;
  std::vector<bus> buses;
private:
  void match(std::string const& name, std::string const& value)
  {
    if(name == "openbus.log.file")
    {
      openbus_log_file = value;
    }
    else
    {
      std::string::const_iterator dot_iterator = 
        std::find(name.begin(), name.end(), '.');
      if(dot_iterator != name.end())
      {
        std::string bus_name(name.begin(), dot_iterator);
        int i = -1;
        if(bus_name == "bus")
          i = 0;
        else if(bus_name == "bus2")
          i = 1;
        else if(bus_name == "bus3")
          i = 2;
        if(i != -1)
        {
          std::size_t index = i; // To avoid warnings in GCC about comparisons
                                 // between signed and unsigned types
          if(buses.size() < index + 1)
            buses.resize(index+1);
          std::string bus_prop(boost::next(dot_iterator), name.end());
          if(bus_prop == "host.name")
            buses[index].host = value;
          else if(bus_prop == "host.port")
            buses[index].port = atoi(value.c_str());
        }
      }
    }
  }
};

#endif
