PROJNAME=server
APPNAME=${PROJNAME}

include config

OPENBUSINC= ${OPENBUS_HOME}/include
OPENBUSLIB= ${OPENBUS_HOME}/lib

TARGETROOT=bin
OBJROOT=obj

INCLUDES= . \
  stubs \
  ${MICO_INC} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/openbus/cpp/stubs \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}

LDIR= ${OPENBUSLIB} ${MICO_LIB}

LIBS+= mico${MICOVERSION} dl crypto ssl

SLIB= ${OPENBUSLIB}/libopenbus-micosinglethread.a \
      ${OPENBUSLIB}/libscs-micosinglethread.a \
      ${OPENBUSLIB}/liblogger.a

USE_NODEPEND= Yes

SRC= src/server.cpp stubs/hello.cc

IDLS= idl/hello.idl

STUBS= stubs/hello.cc stubs/hello.h

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../idl/hello.idl

genstubs: $(STUBS)