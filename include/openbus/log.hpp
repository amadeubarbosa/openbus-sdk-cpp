// -*- coding: iso-8859-1-unix -*-

/**
* API do OpenBus SDK C++
* \file openbus/log.hpp
*/

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_LOG_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_LOG_HPP

#include "openbus/detail/decl.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <tao/ORB.h>
#pragma clang diagnostic pop
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <log/output/streambuf_output.h>
#pragma clang diagnostic pop
#include <log/logger.h>
#include <boost/thread.hpp>
#include <cstring>

namespace openbus 
{
#ifndef OPENBUS_DOXYGEN
typedef logger::log_scope log_scope;
typedef logger::scope_token scope_token;
#endif
using logger::error_level;
using logger::warning_level;
using logger::info_level;
using logger::debug_level;

#ifndef OPENBUS_DOXYGEN

namespace detail 
{
struct mico_thread_formatter : logger::formatter_base
{
  void format(logger::logger const &, logger::level, scope_token const&, 
              std::string &string) const
  {
    std::size_t id;
    boost::thread::id tid = boost::this_thread::get_id();
    std::memcpy(&id, &tid, (std::min)(sizeof(id), sizeof(tid)));
    std::stringstream s;
    s << "(thread " << std::hex << id << ") ";
    std::string tmp = s.str();
    string.insert(string.begin(), tmp.begin(), tmp.end());
  }
  mico_thread_formatter *clone() const 
  { 
    return new mico_thread_formatter(*this); 
  }
};
}
#endif

/**
 * \brief Classe que cont�m fun��es auxiliares para configura��o de
 * log do Openbus
 */
struct log_type
{
  #ifndef OPENBUS_DOXYGEN
  log_type()
  {
    set_level(error_level);
    add_output(logger::output::make_streambuf_output(std::cout));
    std::auto_ptr<logger::formatter_base> mico_thread_formatter 
      (new detail::mico_thread_formatter);
    add_formatter(mico_thread_formatter);
  }
  #endif

  /**
   * \brief Adiciona um output de log. Esses outputs podem ser
   * construidos com as bibliotecas de log
   */
  void add_output(std::auto_ptr<logger::output_base> output)
  {
    general_log.add_output(output);
  }

  /**
   * \brief Adiciona um formatador de log. Esses formatadores podem ser
   * construidos com as bibliotecas de log
   */
  void add_formatter(std::auto_ptr<logger::formatter_base> formatter)
  {
    general_log.add_formatter(formatter);
  }

  /**
   * \brief Modifica o n�vel de log para o Openbus
   */
  void set_level(logger::level lev)
  {
    general_log.set_level(lev);
  }

  #ifndef OPENBUS_DOXYGEN
  logger::logger &general_logger()
  { 
    return general_log; 
  }
  #endif
private:
  logger::logger general_log;
};

/**
 * \brief Inst�ncia global de log_type que permite acesso pelo usu�rio
 * das configura��es de log do Openbus
 */
  OPENBUS_SDK_DECL log_type * log();

}

#endif
