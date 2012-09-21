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

LIBS+= mico${MICOVERSION} dl crypto ssl

ifeq "$(DBG)" "Yes"
  LIB_DEBUG=-debug
endif

ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
  SLIB= ${OPENBUSLIB}/libopenbus-micomultithread$(LIB_DEBUG).a \
        ${OPENBUSLIB}/libscs-micomultithread$(LIB_DEBUG).a \
        ${OPENBUSLIB}/liblogger-multithread$(LIB_DEBUG).a
else
  SLIB= ${OPENBUSLIB}/libopenbus-micosinglethread$(LIB_DEBUG).a \
        ${OPENBUSLIB}/libscs-micosinglethread$(LIB_DEBUG).a \
        ${OPENBUSLIB}/liblogger-singlethread$(LIB_DEBUG).a
endif

USE_LUA51= YES
USE_NODEPEND= Yes

SRC= client.cpp \
     stubs/hello.cc 

