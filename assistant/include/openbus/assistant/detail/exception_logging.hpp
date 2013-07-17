// -*- coding: iso-8859-1-unix -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_EXCEPTION_LOGGING_H
#define OPENBUS_ASSISTANT_DETAIL_EXCEPTION_LOGGING_H

namespace openbus { namespace assistant { namespace assistant_detail {

struct exception_logging
{
  logger::log_scope& l;
  std::string error_message;
  exception_logging(logger::log_scope& l, std::string error_message = std::string())
    : l(l), error_message(error_message) {}
  ~exception_logging()
  {
    try
    {
      if(std::uncaught_exception())
      {
        if(error_message.empty())
          l.level_log(logger::error_level, "A exception was thrown");
        else
          l.level_vlog(logger::error_level, "A exception was thrown: %s", error_message.c_str());
      }
    }
    catch(std::exception const&) {}
  }
};

} } }

#endif
