PROJNAME= ACSTester
APPNAME= acs

# Descomente a linha abaixo para utilizar a versão multithread.
# A versão multithread do Openbus deve ser utilizada com Mico
# compilado com suporte a multithread.
#DEFINES+=MULTITHREAD

# Descomente a linha abaixo para utilizar o Openbus Multithread.
#LIBS=pthread

# Descomente a linha abaixo para ativar o modo debug.
#DBG=YES

# Descomente a linha abaixo para o uso em Valgrind.
# p.s.: O modo debug(DBG) deve estar ativado.
#CPPFLAGS= -fno-inline

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

EXTRA_CONFIG=../config

ifeq "$(TEC_UNAME)" "SunOS58"
  USE_CC=Yes
  CPPFLAGS= -g +p -KPIC -xarch=v8  -mt -D_REENTRANT
endif

INCLUDES= . ../../../stubs/mico \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}/cxxtest \
  ${OPENBUSINC}/openssl-0.9.9 \
  ${OPENBUSINC}/logger \
  ${MICO_INC}

LDIR= ${OPENBUSLIB} ${MICO_LIB}

LIBS= crypto mico${MICOVERSION} dl ssl

SLIB= ${OPENBUSLIB}/libopenbusmico.a \
      ${OPENBUSLIB}/libscsmico.a \
      ${OPENBUSLIB}/liblogger.a

USE_LUA51= YES

SRC= runner.cpp

runner.cpp: ACSTestSuite.cpp
	cxxtestgen.pl --runner=ErrorPrinter --abort-on-fail -o runner.cpp ACSTestSuite.cpp

cxxtest: runner.cpp
