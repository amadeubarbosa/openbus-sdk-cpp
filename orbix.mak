PROJNAME= openbusorbix
LIBNAME= ${PROJNAME}

DEFINES=SCS_ORBIX
DEFINES+=OPENBUS_ORBIX

# Descomente a linha abaixo para ativar o modo debug.
#DBG=YES

# Descomente a linha abaixo para o uso em Valgrind.
# p.s.: O modo debug(DBG) deve estar ativado.
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
PRECMP_DIR= obj/${TEC_UNAME}

INCLUDES= . $(PRECMP_DIR) ${ORBIXINC} ${OPENBUSINC}/scs ${OPENBUSINC}/openssl-0.9.9 ${OPENBUSINC}/logger
LDIR= ${ORBIXLDIR} ${OPENBUSLIB} ${ORBIXLDIR}

LIBS= it_poa it_art it_ifc it_portable_interceptor scsorbix crypto logger

USE_LUA51=YES
USE_NODEPEND= Yes

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
     FaultToleranceManager.cpp \
     $(PRECMP_DIR)/IOR.c

IDLS= ${OPENBUS_HOME}/idlpath/v1_05/core.idl \
${OPENBUS_HOME}/idlpath/v1_05/scs.idl \
${OPENBUS_HOME}/idlpath/v1_05/access_control_service.idl \
${OPENBUS_HOME}/idlpath/v1_05/registry_service.idl \
${OPENBUS_HOME}/idlpath/v1_05/fault_tolerance.idl \
${OPENBUS_HOME}/idlpath/v1_05/session_service.idl 

STUBS= stubs/orbix/coreC.cxx stubs/orbix/core.hh \
stubs/orbix/scsC.cxx stubs/orbix/scs.hh \
stubs/orbix/access_control_serviceC.cxx stubs/orbix/access_control_service.hh \
stubs/orbix/registry_serviceC.cxx stubs/orbix/registry_service.hh \
stubs/orbix/fault_toleranceC.cxx stubs/orbix/fault_tolerance.hh \
stubs/orbix/session_serviceC.cxx stubs/orbix/session_service.hh

$(STUBS): $(IDLS)
	mkdir -p stubs/orbix
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base  ${OPENBUS_HOME}/idlpath/v1_05/fault_tolerance.idl 
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base  ${OPENBUS_HOME}/idlpath/v1_05/access_control_service.idl 
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base  ${OPENBUS_HOME}/idlpath/v1_05/registry_service.idl
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base  ${OPENBUS_HOME}/idlpath/v1_05/session_service.idl
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base  ${OPENBUS_HOME}/idlpath/v1_05/core.idl
	cd stubs/orbix ; ${ORBIX_HOME}/bin/idl -base -poa ${OPENBUS_HOME}/idlpath/v1_05/scs.idl

genstubs: $(STUBS)
	
sunos: $(OBJS)
	rm -f lib/$(TEC_UNAME)/libopenbusorbix.a
	CC $(CPPFLAGS) -xar -instances=extern -o lib/$(TEC_UNAME)/libopenbusorbix.a $(OBJS)
	rm -f lib/$(TEC_UNAME)/libopenbusorbix.so
	CC $(CPPFLAGS) -G -instances=extern -o lib/$(TEC_UNAME)/libopenbusorbix.so $(OBJS)

precompile:
	${LUA51}/bin/${TEC_UNAME}/lua5.1 precompiler.lua -l lua -f IOR IOR.lua
