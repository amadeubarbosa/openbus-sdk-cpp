PROJNAME= openbusorbix
LIBNAME= ${PROJNAME}

DEFINES=SCS_ORBIX
DEFINES+=OPENBUS_ORBIX
DEFINES+=LOGGER_MULTITHREAD

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

ifeq "$(TEC_UNAME)" "SunOS510_64"
  USE_CC=Yes
  AR= CC
  
  ifeq "$(DBG)" "YES"
    FLAGS= -g
    CPPFLAGS= -g
    STDLFLAGS= -g
    LFLAGS= -g
  else
    FLAGS=
    CPPFLAGS=
    STDLFLAGS=
    LFLAGS=
  endif

  FLAGS+= -m64 -xcode=pic32 -mt=yes 
  CPPFLAGS+= -m64 -KPIC -mt -library=stlport4
  STDLFLAGS+= -mt -m64 -xar -o
  LFLAGS+= -mt -m64 -instances=extern -library=stlport4
  NO_LOCAL_LD=Yes
  
  ORBIXLDIR=${ORBIX_HOME}/lib/sparcv9
endif

ORBIX_HOME= ${IT_PRODUCT_DIR}/asp/6.3
ORBIXINC= ${ORBIX_HOME}/include

OPENBUSINC = ${OPENBUS_HOME}/include
OPENBUSLIB = ${OPENBUS_HOME}/lib

OBJROOT= obj/orbix
TARGETROOT= lib
PRECMP_DIR= obj/orbix

INCLUDES= . \
  include \
  ./stubs/orbix \
  ${PRECMP_DIR} \
  ${ORBIXINC} \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}/openssl-0.9.9 \
  ${OPENBUSINC}/logger

LDIR= ${ORBIXLDIR} ${OPENBUSLIB} ${ORBIXLDIR}

LIBS= it_poa it_art it_ifc it_portable_interceptor scsorbix crypto logger

USE_LUA51=YES
USE_NODEPEND=YES

SRC= src/interceptors/ClientInterceptor.cpp \
     src/interceptors/ServerInterceptor.cpp \
     src/interceptors/ORBInitializerImpl.cpp \
     stubs/orbix/access_control_serviceC.cxx \
     stubs/orbix/registry_serviceC.cxx \
     stubs/orbix/session_serviceC.cxx \
     stubs/orbix/fault_toleranceC.cxx \
     stubs/orbix/coreC.cxx \
     src/openbus.cpp \
     src/util/Helper.cpp \
     src/FaultToleranceManager.cpp \
     ${PRECMP_DIR}/IOR.c

IDLS= ${OPENBUS_HOME}/idl/v1_05/core.idl \
${OPENBUS_HOME}/idl/v1_05/scs.idl \
${OPENBUS_HOME}/idl/v1_05/access_control_service.idl \
${OPENBUS_HOME}/idl/v1_05/registry_service.idl \
${OPENBUS_HOME}/idl/v1_05/fault_tolerance.idl \
${OPENBUS_HOME}/idl/v1_05/session_service.idl 

STUBS= stubs/orbix/coreC.cxx stubs/orbix/core.hh \
stubs/orbix/scsC.cxx stubs/orbix/scs.hh \
stubs/orbix/access_control_serviceC.cxx stubs/orbix/access_control_service.hh \
stubs/orbix/registry_serviceC.cxx stubs/orbix/registry_service.hh \
stubs/orbix/fault_toleranceC.cxx stubs/orbix/fault_tolerance.hh \
stubs/orbix/session_serviceC.cxx stubs/orbix/session_service.hh

$(STUBS): $(IDLS)
	mkdir -p stubs/orbix
	cd stubs/orbix ; ../../shell/orbix/idl -base  ${OPENBUS_HOME}/idl/v1_05/fault_tolerance.idl 
	cd stubs/orbix ; ../../shell/orbix/idl -base  ${OPENBUS_HOME}/idl/v1_05/access_control_service.idl 
	cd stubs/orbix ; ../../shell/orbix/idl -base  ${OPENBUS_HOME}/idl/v1_05/registry_service.idl
	cd stubs/orbix ; ../../shell/orbix/idl -base  ${OPENBUS_HOME}/idl/v1_05/session_service.idl
	cd stubs/orbix ; ../../shell/orbix/idl -base  ${OPENBUS_HOME}/idl/v1_05/core.idl
	cd stubs/orbix ; ../../shell/orbix/idl -base -poa ${OPENBUS_HOME}/idl/v1_05/scs.idl

genstubs: $(STUBS)
	
precompile:
	mkdir -p ${PRECMP_DIR}
	${LUA51}/bin/${TEC_UNAME}/lua5.1 precompiler.lua -d ${PRECMP_DIR} -l lua -f IOR IOR.lua
