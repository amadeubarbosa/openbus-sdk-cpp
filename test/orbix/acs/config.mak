PROJNAME= ACSTester
APPNAME= acs

DEFINES=SCS_ORBIX
DEFINES+=OPENBUS_ORBIX

# Descomente a linha abaixo para ativar o modo debug.
#DBG=YES

# Descomente a linha abaixo para o uso em Valgrind.
# p.s.: O modo debug(DBG) deve estar ativado.
#CPPFLAGS= -fno-inline

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

include ../config

ifeq "$(TEC_SYSNAME)" "SunOS"
  USE_CC=Yes
  CPPFLAGS= -g +p -KPIC -xarch=v8  -mt -D_REENTRANT
  LFLAGS= $(CPPFLAGS) -xildoff
endif

INCLUDES= . \
  ${ORBIXINC} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}/cxxtest \
  ${OPENBUSINC}/openssl-0.9.9 \
  ${OPENBUSINC}/logger

LDIR= ${ORBIXLDIR} ${OPENBUSLIB}

LIBS= crypto it_poa it_art it_ifc it_portable_interceptor logger

SLIB= ${OPENBUSLIB}/libopenbusorbix.a \
      ${OPENBUSLIB}/libscsorbix.a \
      ${OPENBUSLIB}/liblogger.a

USE_LUA51= YES
USE_NODEPEND= YES

SRC= runner.cpp

runner.cpp: ACSTestSuite.cpp
	cxxtestgen.pl --runner=ErrorPrinter --abort-on-fail -o runner.cpp ACSTestSuite.cpp

cxxtest: runner.cpp
