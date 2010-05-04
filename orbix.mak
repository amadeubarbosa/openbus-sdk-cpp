PROJNAME= openbusorbix
LIBNAME= ${PROJNAME}

#Descomente as duas linhas abaixo para o uso em Valgrind.
#DBG=YES
#CPPFLAGS= -fno-inline

ifeq ($(TEC_WORDSIZE), TEC_64)
  ORBIXLDIR=${ORBIX_HOME}/lib/lib64
else
  ORBIXLDIR=${ORBIX_HOME}/lib
endif

ifeq "$(TEC_SYSNAME)" "SunOS"
  USE_CC=Yes
  CPPFLAGS= -g +p -KPIC -mt -D_REENTRANT -library=stlport4
  ifeq ($(TEC_WORDSIZE), TEC_64)
    CPPFLAGS+= -m64
    ORBIXLDIR=${ORBIX_HOME}/lib/sparcv9
  endif
endif

ORBIX_HOME= ${IT_PRODUCT_DIR}/asp/6.3
ORBIXINC= ${ORBIX_HOME}/include

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

OBJROOT= obj
TARGETROOT= lib

INCLUDES= . ${ORBIXINC} ${OPENBUSINC}/scs ${OPENBUSINC}/openssl-0.9.9 ${OPENBUSINC}/logger
LDIR= ${ORBIXLDIR} ${OPENBUSLIB} ${ORBIXLDIR}

LIBS= it_poa it_art it_ifc it_portable_interceptor scsorbix crypto logger

USE_LUA51=YES

SRC= openbus/interceptors/ClientInterceptor.cpp \
     openbus/interceptors/ServerInterceptor.cpp \
     openbus/interceptors/ORBInitializerImpl.cpp \
     stubs/orbix/access_control_serviceC.cxx \
     stubs/orbix/registry_serviceC.cxx \
     stubs/orbix/session_serviceC.cxx \
     stubs/orbix/fault_toleranceC.cxx \
     stubs/orbix/coreC.cxx \
     openbus.cpp \
     openbus/util/Helper.cpp \
     FaultToleranceManager.cpp

genstubs:
	mkdir -p stubs/orbix
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base  ${OPENBUS_HOME}/idlpath/v1_05/fault_tolerance.idl 
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base  ${OPENBUS_HOME}/idlpath/v1_05/access_control_service.idl 
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base  ${OPENBUS_HOME}/idlpath/v1_05/registry_service.idl
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base  ${OPENBUS_HOME}/idlpath/v1_05/session_service.idl
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base  ${OPENBUS_HOME}/idlpath/v1_05/core.idl
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base -poa ${OPENBUS_HOME}/idlpath/v1_05/scs.idl
	
sunos: $(OBJS)
	rm -f lib/$(TEC_UNAME)/libopenbusorbix.a
	CC $(CPPFLAGS) -xar -instances=extern -o lib/$(TEC_UNAME)/libopenbusorbix.a $(OBJS)
	rm -f lib/$(TEC_UNAME)/libopenbusorbix.so
	CC $(CPPFLAGS) -G -instances=extern -o lib/$(TEC_UNAME)/libopenbusorbix.so $(OBJS)

