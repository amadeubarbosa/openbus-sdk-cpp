PROJNAME=server
APPNAME=${PROJNAME}

include config

OPENBUSINC = ${OPENBUS_HOME}/include
OPENBUSLIB = ${OPENBUS_HOME}/lib

TARGETROOT=bin
OBJROOT=obj

INCLUDES= . \
  stubs \
  ${MICO_INC} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/openbus/cpp/stubs/mico \
  ${OPENBUSINC} \
  ${OPENBUSINC}/logger

LDIR= ${OPENBUSLIB} ${MICO_LIB}

LIBS+= mico${MICOVERSION} dl crypto ssl pthread

SLIB= ${OPENBUSLIB}/libopenbusmicoMT.a \
      ${OPENBUSLIB}/libscsmicoMT.a \
      ${OPENBUSLIB}/liblogger.a

DEFINES+=SCS_THREADING_ENABLED
DEFINES+=LOGGER_MULTITHREAD

USE_LUA51= YES
USE_NODEPEND= YES

SRC= server.cpp \
     stubs/delegate.cc 

IDLS= ../../idl/delegate.idl

STUBS= stubs/delegate.cc stubs/delegate.h

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../../../idl/delegate.idl

genstubs: $(STUBS)

