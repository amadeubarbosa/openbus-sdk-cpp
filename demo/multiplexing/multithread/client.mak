PROJNAME=client
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

SLIB= ${OPENBUSLIB}/libopenbus-micomultithread.a \
      ${OPENBUSLIB}/libscs-micomultithread.a \
      ${OPENBUSLIB}/liblogger.a

USE_NODEPEND= Yes

SRC= src/client.cpp stubs/hello.cc
