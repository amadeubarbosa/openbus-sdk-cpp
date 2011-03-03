PROJNAME=server
APPNAME=${PROJNAME}

# Descomente a linha abaixo para compilar em 64 Bits.
DEFINES+=MICO_64

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

ifeq "$(TEC_UNAME)" "SunOS510"
  USE_CC=Yes

  CPPFLAGS= -m64 -library=stlport4
  LFLAGS= -m64 -library=stlport4 
  LIBS= nsl socket

  # Multithread
  CPPFLAGS+= -mt
  LFLAGS+= -mt
endif

TARGETROOT=bin
OBJROOT=obj

INCLUDES= . \
  stubs \
  ${OPENBUSINC}/mico-${MICOVERSION} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/openbus/cpp/stubs/mico \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}/logger

LDIR= ${OPENBUSLIB}

LIBS+= mico${MICOVERSION} dl crypto ssl

SLIB= ${OPENBUSLIB}/libopenbusmico.a \
      ${OPENBUSLIB}/libscsmico.a \
      ${OPENBUSLIB}/liblogger.a

USE_LUA51= YES
USE_NODEPEND= Yes

SRC= server.cpp \
     stubs/hello.cc 

IDLS= ../idl/hello.idl

STUBS= stubs/hello.cc stubs/hello.h

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${OPENBUS_HOME}/bin/${TEC_UNAME}/idl --poa ../../idl/hello.idl

genstubs: $(STUBS)
