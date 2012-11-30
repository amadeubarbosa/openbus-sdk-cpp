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

SLIB= ${OPENBUSLIB}/libopenbusmicoST.a \
      ${OPENBUSLIB}/libscsmicoST.a \
      ${OPENBUSLIB}/liblogger.a

ifeq "$(OPENBUS_SDK_FT_ENABLED)" "Yes"
USE_LUA51= YES
endif
USE_NODEPEND= Yes

SRC= client.cpp \
     stubs/delegate.cc 

