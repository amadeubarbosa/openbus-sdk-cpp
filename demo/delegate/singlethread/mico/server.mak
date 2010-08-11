PROJNAME=server
APPNAME=${PROJNAME}

# Descomente a linha abaixo para utilizar a versão multithread.
# A versão multithread do Openbus deve ser utilizada com Mico
# compilado com suporte a multithread.
DEFINES+=MULTITHREAD

# Descomente a linha abaixo para utilizar o Openbus Multithread.
LIBS=pthread

# Descomente a linha abaixo para ativar o modo debug.
#DBG=YES

# Descomente a linha abaixo para o uso em Valgrind.
# p.s.: O modo debug(DBG) deve estar ativado.
#CPPFLAGS= -fno-inline

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

EXTRA_CONFIG=config

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

TARGETROOT=bin
OBJROOT=obj

INCLUDES= . \
  stubs \
  ${MICO_INC} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/openbus/cpp/stubs/mico \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}/logger

LDIR= ${MICO_LIB} ${OPENBUSLIB}

LIBS+= mico${MICOVERSION} dl crypto ssl

SLIB= ${OPENBUSLIB}/libopenbusmico.a \
      ${OPENBUSLIB}/libscsmico.a \
      ${OPENBUSLIB}/liblogger.a

USE_LUA51= YES
USE_NODEPEND= Yes

SRC= server.cpp \
     stubs/delegate.cc 

IDLS= ../../idl/delegate.idl

STUBS= stubs/delegate.cc stubs/delegate.h

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../../../idl/delegate.idl

genstubs: $(STUBS)

