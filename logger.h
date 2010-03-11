/**
* \file logger.h
*/

#ifndef LOGGER_H_
#define LOGGER_H_

#include <fstream>
#include <string>
#include <sstream>
#include <map>

#define ENUM_TO_STR(enumValue) #enumValue

using namespace std;

namespace logger {

/**
* \brief N�vel de mensagem.
* O n�vel ALL e OFF possuem um comportamento distinto 
* perante os demais. ALL representa  todos  os demais 
* n�veis com exce��o do OFF. O  n�vel  OFF representa
* a aus�ncia de todos os demais n�veis com exce��o do 
* ALL.
*/
  enum Level {
    ALL,
    ERROR,
    INFO,
    WARNING,
    OFF
  };

/**
* \brief Representa um objeto do tipo Logger  a   ser 
* utilizado como mecanismo de log.
*/ 
  class Logger {
    private:
    /**
    * Rela��o entre os n�veis como elementos de enumera��o
    * com as representa��es dos n�veis em strings.
    */
      static const char* levelStr[];

    /**
    * Singleton do Logger.
    */
      static Logger* logger;

    /**
    * Mapeamento que descreve quais n�veis est�o ativados.
    */
      map<Level, bool> levelFlag; 

    /**
    * Contador de edenta��o utilizado para se gerar   uma
    * sa�da hier�rquica conforme a pilha de chamadas.
    */
      short numIndent;

    /**
    * Path relativo ou absoluto do arquivo  de  sa�da  do
    * Logger.
    */
      char* filename;
      ostream* output;

      Logger();
      ~Logger();
    public:
    /**
    * Fornece a �nica inst�ncia do Logger.
    *
    * @return Logger
    */
      static Logger* getInstance();
   
    /**
    * Define a sa�da do logger.
    *
    * O padr�o � sa�da padr�o stdout.
    *
    * @param[in] filename Caminho relativo ou absoluto do arquivo de sa�da 
    *   do logger. O valor 0 (NULL) representa a sa�da padr�o stdout,
    */
      void setOutput(char* filename);

    /**
    * Ativa ou desativa um determinado n�vel de mensagem.
    *
    * Se o n�vel est� desativado, a chamada a este m�todo ativar� o mesmo, 
    * e vice-versa.
    *
    * @param [in] N�vel de mensagem.
    */
      void setLevel(Level level);

    /**
    * Informa se um n�vel est� ativo ou n�o.
    *
    * @return True se o n�vel est� ativo, caso contr�rio retorna false.
    */
      bool getLevel(Level level);

    /**
    * Registra uma mensagem utilizando um determinado n�vel.
    *
    * @param [in] level N�vel de mensagem.
    * @param [in] message Mensagem.
    */
      void log(Level level, string message);

    /**
    * Edenta em um n�vel as mensagens seguintes.
    *
    * Abre-se um novo escopo para as mensagens seguintes a esta chamada.
    */
      void indent(); 

    /**
    * Edenta em um n�vel as mensagens seguintes.
    *
    * Registra uma mensagem e abre um novo escopo para as mensagens 
    * seguintes a esta chamada.
    *
    * @param [in] level N�vel de mensagem.
    * @param [in] message Mensagem.
    */
      void indent(Level level, string message);

    /**
    * Desfaz uma edenta��o.
    *
    * Fecha o escopo atual de mensagens.
    */
      void dedent();

    /**
    * Desfaz uma edenta��o.
    *
    * Fecha o escopo atual de mensagens e logo ap�s registra uma 
    * mensagem.
    *
    * @param [in] level N�vel de mensagem.
    * @param [in] message Mensagem.
    */
      void dedent(Level level, string message);
  };
}

#endif

