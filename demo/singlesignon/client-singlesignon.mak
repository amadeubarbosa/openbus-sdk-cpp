PROJNAME=client-singlesignon
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
  ${OPENBUSINC}

LDIR= ${OPENBUSLIB} ${MICO_LIB}

LIBS+= mico${MICOVERSION} dl crypto ssl

ifeq "$(MULTITHREAD)" "Yes"
  SLIB= ${OPENBUSLIB}/libopenbus-micomultithread.a \
        ${OPENBUSLIB}/libscs-micomultithread.a \
        ${OPENBUSLIB}/liblogger.a
else
  SLIB= ${OPENBUSLIB}/libopenbus-micosinglethread.a \
        ${OPENBUSLIB}/libscs-micosinglethread.a \
        ${OPENBUSLIB}/liblogger.a
endif

USE_NODEPEND= Yes

SRC= src/client-singlesignon.cpp stubs/hello.cc

IDLS= ../../hello/idl/hello.idl

STUBS= stubs/hello.cc stubs/hello.h

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../../hello/idl/hello.idl

genstubs: $(STUBS)

