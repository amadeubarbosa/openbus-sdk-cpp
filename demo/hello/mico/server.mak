PROJNAME=server
APPNAME=${PROJNAME}

include config

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

TARGETROOT=bin
OBJROOT=obj

INCLUDES= . \
  stubs \
  ${OPENBUSINC}/mico-${MICOVERSION}/${TEC_UNAME} \
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
