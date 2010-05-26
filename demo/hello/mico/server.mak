PROJNAME=server
APPNAME=${PROJNAME}

DEFINES=SCS_MICO
DEFINES+=OPENBUS_MICO

#Descomente as duas linhas abaixo para o uso em Valgrind.
DBG=YES
CPPFLAGS= -fno-inline

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

EXTRA_CONFIG=config

ifeq "$(TEC_UNAME)" "SunOS58"
  USE_CC=Yes
  CPPFLAGS= -g +p -KPIC -xarch=v8  -mt -D_REENTRANT
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

LIBS= mico2.3.11 dl crypto ssl pthread

SLIB= ${OPENBUSLIB}/libopenbusmico.a \
      ${OPENBUSLIB}/libscsmico.a \
      ${OPENBUSLIB}/liblogger.a

USE_LUA51= YES

SRC= server.cpp \
     stubs/hello.cc 

genstubs:
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../../idl/hello.idl

