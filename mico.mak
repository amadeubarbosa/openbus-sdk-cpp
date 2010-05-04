PROJNAME= openbusmico
LIBNAME= ${PROJNAME}

DEFINES=SCS_MICO
DEFINES+=OPENBUS_MICO

#Descomente as duas linhas abaixo para o uso em Valgrind.
#DBG=YES
#CPPFLAGS= -fno-inline

ifeq "$(TEC_SYSNAME)" "SunOS"
  USE_CC=Yes
  CPPFLAGS= -g +p -KPIC -mt -D_REENTRANT
endif

ifeq ($(TEC_WORDSIZE), TEC_64)
  CPPFLAGS+= -m64
endif

MICO_BIN= ${MICODIR}/bin
MICO_INC= ${MICODIR}/include
MICO_LIB=${MICODIR}/lib

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

OBJROOT= obj
TARGETROOT= lib

INCLUDES= . ${MICO_INC} ${OPENBUSINC}/scs ${OPENBUSINC}/openssl-0.9.9 ${OPENBUSINC}/logger
LDIR= ${MICO_LIB} ${OPENBUSLIB} ${MICO_LIB}

LIBS= mico2.3.11 scsmico crypto dl logger

USE_LUA51= YES

SRC= openbus/interceptors/ClientInterceptor.cpp \
     openbus/interceptors/ServerInterceptor.cpp \
     openbus/interceptors/ORBInitializerImpl.cpp \
     stubs/mico/access_control_service.cc \
     stubs/mico/registry_service.cc \
     stubs/mico/session_service.cc \
     stubs/mico/fault_tolerance.cc \
     stubs/mico/core.cc \
     stubs/mico/scs.cc \
     openbus.cpp \
     openbus/util/Helper.cpp \
     FaultToleranceManager.cpp

genstubs:
	mkdir -p stubs/mico
	ln -fs ${OPENBUS_HOME}/idlpath/v1_05/core.idl stubs/mico
	ln -fs ${OPENBUS_HOME}/idlpath/v1_05/scs.idl stubs/mico
	ln -fs ${OPENBUS_HOME}/idlpath/v1_05/access_control_service.idl stubs/mico
	ln -fs ${OPENBUS_HOME}/idlpath/v1_05/registry_service.idl stubs/mico
	ln -fs ${OPENBUS_HOME}/idlpath/v1_05/fault_tolerance.idl stubs/mico
	ln -fs ${OPENBUS_HOME}/idlpath/v1_05/session_service.idl stubs/mico
	cd stubs/mico ; ${MICO_BIN}/idl --any --typecode access_control_service.idl 
	cd stubs/mico ; ${MICO_BIN}/idl fault_tolerance.idl
	cd stubs/mico ; ${MICO_BIN}/idl registry_service.idl
	cd stubs/mico ; ${MICO_BIN}/idl session_service.idl
	cd stubs/mico ; ${MICO_BIN}/idl core.idl
	cd stubs/mico ; ${MICO_BIN}/idl scs.idl
	
sunos: $(OBJS)
	rm -f lib/$(TEC_UNAME)/libopenbusmico.a
	CC $(CPPFLAGS) -xar -instances=extern -o lib/$(TEC_UNAME)/libopenbusmico.a $(OBJS)
	rm -f lib/$(TEC_UNAME)/libopenbusmico.so
	CC $(CPPFLAGS) -G -instances=extern -o lib/$(TEC_UNAME)/libopenbusmico.so $(OBJS)

