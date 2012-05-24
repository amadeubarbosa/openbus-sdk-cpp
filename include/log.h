#ifndef TECGRAF_OPENBUS_LOG_H_
#define TECGRAF_OPENBUS_LOG_H_

#include <log/output/streambuf_output.h>
#include <log/logger.h>

namespace openbus {

typedef logger::log_scope log_scope;
typedef logger::scope_token scope_token;
using logger::error_level;
using logger::warning_level;
using logger::info_level;
using logger::debug_level;

struct log_type
{
  log_type()
  {
    general_log.set_level(error_level);
    general_log.add_output(logger::output::make_streambuf_output(std::cout));
    ci_log.set_level(error_level);
    ci_log.add_output(logger::output::make_streambuf_output(std::cout));
    si_log.set_level(error_level);
    si_log.add_output(logger::output::make_streambuf_output(std::cout));
  }
  void add_output(std::auto_ptr<logger::output_base> output)
  {
    std::auto_ptr<logger::output_base> tmp1(output->clone());
    std::auto_ptr<logger::output_base> tmp2(output->clone());
    general_log.add_output(output);
    ci_log.add_output(tmp1);
    si_log.add_output(tmp2);
  }
  void add_formatter(std::auto_ptr<logger::formatter_base> formatter)
  {
    std::auto_ptr<logger::formatter_base> tmp1(formatter->clone());
    std::auto_ptr<logger::formatter_base> tmp2(formatter->clone());
    general_log.add_formatter(formatter);
    ci_log.add_formatter(tmp1);
    si_log.add_formatter(tmp2);
  }
  void set_level(logger::level lev)
  {
    general_log.set_level(lev);
    ci_log.set_level(lev);
    si_log.set_level(lev);
  }

  logger::logger& general_logger() { return general_log; }
  logger::logger& client_interceptor_logger() { return ci_log; }
  logger::logger& server_interceptor_logger() { return si_log; }
private:
  logger::logger general_log, ci_log, si_log;
};

extern log_type log;

}

#endif
