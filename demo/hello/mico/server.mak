PROJNAME=server
APPNAME=${PROJNAME}

#Descomente a linha abaixo para utilizar a versão multithread.
# A versão multithread do Openbus deve ser utilizada com Mico
# compilado com suporte a multithread.
#DEFINES+=MULTITHREAD

#Descomente a linha abaixo para utilizar o Openbus Multithread.
#LIBS=pthread

#Descomente as duas linhas abaixo para o uso em Valgrind.
#DBG=YES
#CPPFLAGS= -fno-inline

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

EXTRA_CONFIG=config

ifeq "$(TEC_UNAME)" "SunOS510"
  USE_CC=Yes
  CPPFLAGS= -library=stlport4
  LFLAGS= -library=stlport4
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
     stubs/hello.cc 

IDLS= ../idl/hello.idl

STUBS= stubs/hello.cc stubs/hello.h

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../../idl/hello.idl

genstubs: $(STUBS)
