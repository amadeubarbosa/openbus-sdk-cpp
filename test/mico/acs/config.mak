PROJNAME= ACSTester
APPNAME= acs

# Descomente a linha abaixo para compilar em 64 Bits.
#DEFINES+=MICO_64

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

ifeq "$(TEC_UNAME)" "SunOS510_64"
  CPPFLAGS= -m64
  LFLAGS= -m64
  STDLFLAGS= -m64
endif

ifeq "$(TEC_SYSNAME)" "SunOS"
  USE_CC=Yes

  # Multithread
  CPPFLAGS+= -mt
  LFLAGS+= -mt
  LIBS=
  
  CPPFLAGS+= -library=stlport4
  LFLAGS+= -library=stlport4 
  LIBS= nsl socket
endif

INCLUDES= . ../../../stubs/mico \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}/cxxtest \
  ${OPENBUSINC}/openssl-0.9.9 \
  ${OPENBUSINC}/logger \
  ${OPENBUSINC}/mico-${MICOVERSION} \
  ${MICO_INC}

LDIR= ${OPENBUSLIB} ${MICO_LIB}

LIBS+= crypto mico${MICOVERSION} dl ssl

SLIB= ${OPENBUSLIB}/libopenbusmico.a \
      ${OPENBUSLIB}/libscsmico.a \
      ${OPENBUSLIB}/liblogger.a

USE_LUA51= YES

SRC= runner.cpp

runner.cpp: ACSTestSuite.cpp
	cxxtestgen.pl --runner=ErrorPrinter --abort-on-fail -o runner.cpp ACSTestSuite.cpp

cxxtest: runner.cpp
