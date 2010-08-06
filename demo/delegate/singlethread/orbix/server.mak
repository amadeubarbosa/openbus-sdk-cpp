PROJNAME=server
APPNAME=${PROJNAME}

#Descomente as duas linhas abaixo para o uso em Valgrind.
#DBG=YES
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

SRC= server.cpp \
     stubs/delegateC.cxx \
     stubs/delegateS.cxx

IDLS= ../../idl/delegate.idl

STUBS= stubs/delegateC.cxx stubs/delegateS.cxx stubs/delegate.hh

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${ORBIXBIN}/idl -base -poa ../../../idl/delegate.idl

genstubs: $(STUBS)
