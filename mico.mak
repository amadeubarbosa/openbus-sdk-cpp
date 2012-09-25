ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
  ifeq "$(DBG)" "Yes"
    PROJNAME=openbus-micomultithread-debug
  else
    PROJNAME=openbus-micomultithread
  endif
else
  ifeq "$(DBG)" "Yes"
    PROJNAME=openbus-micosinglethread-debug
  else
    PROJNAME=openbus-micosinglethread
  endif
endif
LIBNAME=${PROJNAME}

# Descomente a linha abaixo para o uso em Valgrind.
# p.s.: O modo debug(DBG) deve estar ativado.
#CPPFLAGS=-fno-inline

ifeq "$(TEC_UNAME)" "SunOS510_64"
  USE_CC=Yes
  AR=CC
  
  ifeq "$(DBG)" "Yes"
    CPPFLAGS=-g
    STDLFLAGS=-g
    LFLAGS=-g
  else
    FLAGS=
    CPPFLAGS=
    STDLFLAGS=
    LFLAGS=
  endif

  CPPFLAGS+=-erroff=badargtype2w -m64 -KPIC -library=stlport4
  STDLFLAGS+=-m64 -xar -o  
  LFLAGS+=-m64 -instances=extern -library=stlport4 
  LIBS=nsl socket
  NO_LOCAL_LD=Yes

  ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
    CPPFLAGS+=-mt
    LFLAGS+=-mt
    STDLFLAGS=-mt -m64 -xar -o
  endif
endif

ifeq "$(DBG)" "Yes"
  LIB_DEBUG=-debug
endif

ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
  MICO_BIN=${MICODIR}/bin/mico-${MICOVERSION}-multithread${LIB_DEBUG}
  MICO_INC=${MICODIR}/include/mico-${MICOVERSION}-multithread${LIB_DEBUG}
  MICO_LIB=${MICODIR}/lib/mico-${MICOVERSION}-multithread${LIB_DEBUG}
  DEFINES+=OPENBUS_SDK_MULTITHREAD
  DEFINES+=SCS_THREADING_ENABLED
  DEFINES+=LOGGER_MULTITHREAD
else
  MICO_BIN=${MICODIR}/bin/mico-${MICOVERSION}-singlethread${LIB_DEBUG}
  MICO_INC=${MICODIR}/include/mico-${MICOVERSION}-singlethread${LIB_DEBUG}
  MICO_LIB=${MICODIR}/lib/mico-${MICOVERSION}-singlethread${LIB_DEBUG}
endif

OPENBUSINC = ${OPENBUS_HOME}/include
OPENBUSLIB = ${OPENBUS_HOME}/lib

OBJROOT= obj/mico
TARGETROOT= lib

INCLUDES= . \
  include \
  stubs/mico \
  ${MICO_INC} \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}/openssl-0.9.9 \
  ${OPENBUSINC}/logger
  
LDIR= ${MICO_LIB} ${OPENBUSLIB}

ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
  LIBS= mico${MICOVERSION} scs-micomultithread$(LIB_DEBUG) crypto dl logger-multithread$(LIB_DEBUG)
else
  LIBS= mico${MICOVERSION} scs-micosinglethread$(LIB_DEBUG) crypto dl logger-singlethread$(LIB_DEBUG)
endif

USE_LUA51= YES
USE_NODEPEND= YES

SRC= src/interceptors/ClientInterceptor.cpp \
     src/interceptors/ServerInterceptor.cpp \
     src/interceptors/ORBInitializerImpl.cpp \
     stubs/mico/access_control_service.cc \
     stubs/mico/registry_service.cc \
     stubs/mico/session_service.cc \
     stubs/mico/fault_tolerance.cc \
     stubs/mico/core.cc \
     src/openbus.cpp \
     src/util/Helper.cpp \
     src/FaultToleranceManager.cpp

STUBS= stubs/mico/core.h stubs/mico/core.cc \
stubs/mico/scs.h \
stubs/mico/access_control_service.h stubs/mico/access_control_service.cc \
stubs/mico/registry_service.h stubs/mico/registry_service.cc \
stubs/mico/fault_tolerance.h stubs/mico/fault_tolerance.cc \
stubs/mico/session_service.h stubs/mico/session_service.cc

IDLS= ${OPENBUS_HOME}/idl/v1_05/core.idl \
${OPENBUS_HOME}/idl/v1_05/scs.idl \
${OPENBUS_HOME}/idl/v1_05/access_control_service.idl \
${OPENBUS_HOME}/idl/v1_05/registry_service.idl \
${OPENBUS_HOME}/idl/v1_05/fault_tolerance.idl \
${OPENBUS_HOME}/idl/v1_05/session_service.idl 

$(STUBS): $(IDLS)
	mkdir -p stubs/mico
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths --any --typecode \
	  ${OPENBUS_HOME}/idl/v1_05/access_control_service.idl
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idl/v1_05/fault_tolerance.idl
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idl/v1_05/registry_service.idl
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idl/v1_05/session_service.idl
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idl/v1_05/core.idl
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idl/v1_05/scs.idl

genstubs: $(STUBS)
	
