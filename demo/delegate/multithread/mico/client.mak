PROJNAME=client
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

USE_LUA51= YES
USE_NODEPEND= YES

SRC= client.cpp \
     stubs/delegate.cc 

