PROJNAME=client
APPNAME=${PROJNAME}

DEFINES=SCS_ORBIX
DEFINES+=OPENBUS_ORBIX

# Descomente a linha abaixo para ativar o modo debug.
#DBG=YES

# Descomente a linha abaixo para o uso em Valgrind.
# p.s.: O modo debug(DBG) deve estar ativado.
#CPPFLAGS= -fno-inline

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

include config

TARGETROOT=bin
OBJROOT=obj

INCLUDES= . ${ORBIXINC} ${OPENBUSINC}/openbus/cpp ${OPENBUSINC}/scs ${OPENBUSINC}/logger
LDIR= ${ORBIXLDIR} ${OPENBUSLIB}

LIBS+= it_poa it_art it_ifc it_portable_interceptor crypto

SLIB= ${OPENBUSLIB}/libopenbusorbix.a \
      ${OPENBUSLIB}/libscsorbix.a \
      ${OPENBUSLIB}/liblogger.a

USE_LUA51= YES
USE_NODEPEND= YES

SRC= client.cpp \
     stubs/delegateC.cxx

