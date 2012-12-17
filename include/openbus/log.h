// -*- coding: iso-8859-1 -*-
/**
* API - SDK Openbus C++
* \file openbus/log.h
*/

#ifndef TECGRAF_SDK_OPENBUS_LOG_H_
#define TECGRAF_SDK_OPENBUS_LOG_H_

#include <CORBA.h>
#include <log/output/streambuf_output.h>
#include <log/logger.h>

#include <CORBA.h>

namespace openbus {
#ifndef OPENBUS_DOXYGEN
typedef logger::log_scope log_scope;
typedef logger::scope_token scope_token;
#endif
using logger::error_level;
using logger::warning_level;
using logger::info_level;
using logger::debug_level;
#ifndef OPENBUS_DOXYGEN
#ifdef OPENBUS_SDK_MULTITHREAD
namespace detail {

struct mico_thread_formatter : logger::formatter_base
{
  void format(logger::logger const&, logger::level, scope_token const&, std::string& string) const
  {
    std::stringstream s;
    s << "(thread " << reinterpret_cast<void*>(MICOMT::Thread::self()) << ") ";
    std::string tmp = s.str();
    string.insert(string.begin(), tmp.begin(), tmp.end());
  }
  mico_thread_formatter* clone() const { return new mico_thread_formatter(*this); }
};

}
#endif
#endif

/**
 * \brief Classe que contém funções auxiliares para configuração de
 * log do Openbus
 */
struct log_type
{
#ifndef OPENBUS_DOXYGEN
  log_type()
  {
    set_level(error_level);
    add_output(logger::output::make_streambuf_output(std::cout));
#ifdef OPENBUS_SDK_MULTITHREAD
    std::auto_ptr<logger::formatter_base> mico_thread_formatter
      (new detail::mico_thread_formatter);
    add_formatter(mico_thread_formatter);
#endif
  }
#endif
  /**
   * \brief Adiciona um output de log. Esses outputs podem ser
   * construidos com as bibliotecas de log
   */
  void add_output(std::auto_ptr<logger::output_base> output)
  {
    std::auto_ptr<logger::output_base> tmp1(output->clone());
    std::auto_ptr<logger::output_base> tmp2(output->clone());
    general_log.add_output(output);
    ci_log.add_output(tmp1);
    si_log.add_output(tmp2);
  }
  /**
   * \brief Adiciona um formatador de log. Esses formatadores podem ser
   * construidos com as bibliotecas de log
   */
  void add_formatter(std::auto_ptr<logger::formatter_base> formatter)
  {
    std::auto_ptr<logger::formatter_base> tmp1(formatter->clone());
    std::auto_ptr<logger::formatter_base> tmp2(formatter->clone());
    general_log.add_formatter(formatter);
    ci_log.add_formatter(tmp1);
    si_log.add_formatter(tmp2);
  }
  /**
   * \brief Modifica o nível de log para o Openbus
   */
  void set_level(logger::level lev)
  {
    general_log.set_level(lev);
    ci_log.set_level(lev);
    si_log.set_level(lev);
  }

#ifndef OPENBUS_DOXYGEN
  logger::logger& general_logger() { return general_log; }
  logger::logger& client_interceptor_logger() { return ci_log; }
  logger::logger& server_interceptor_logger() { return si_log; }
#endif
private:
  logger::logger general_log, ci_log, si_log;
};

/**
 * \brief Instância global de log_type que permite acesso pelo usuário
 * das configurações de log do Openbus
 */
extern log_type log;

}

#endif
