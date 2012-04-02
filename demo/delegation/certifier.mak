PROJNAME=certifier
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

SRC= src/certifier.cpp stubs/service.cc

IDLS= idl/service.idl

STUBS= stubs/service.cc stubs/service.h

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../idl/service.idl

genstubs: $(STUBS)
