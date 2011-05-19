PROJNAME=client
APPNAME=${PROJNAME}

include config

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

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
USE_NODEPEND= YES

SRC= client.cpp \
     stubs/delegate.cc 

