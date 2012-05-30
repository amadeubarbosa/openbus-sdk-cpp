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
  MICO_DEBUG=-debug
endif

ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
  MICO_BIN=${MICODIR}/bin/mico-${MICOVERSION}-multithread${MICO_DEBUG}
  MICO_INC=${OPENBUS_HOME}/include/mico-${MICOVERSION}-multithread${MICO_DEBUG}
  MICO_LIB=${OPENBUS_HOME}/lib/mico-${MICOVERSION}-multithread${MICO_DEBUG}
  DEFINES+=OPENBUS_SDK_MULTITHREAD
  DEFINES+=SCS_THREADING_ENABLED
else
  MICO_BIN=${MICODIR}/bin/mico-${MICOVERSION}-singlethread${MICO_DEBUG}
  MICO_INC=${OPENBUS_HOME}/include/mico-${MICOVERSION}-singlethread${MICO_DEBUG}
  MICO_LIB=${OPENBUS_HOME}/lib/mico-${MICOVERSION}-singlethread${MICO_DEBUG}
endif

OPENBUS_INC=${OPENBUS_HOME}/include
OPENBUS_LDIR=${OPENBUS_HOME}/lib
ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
  LIBS+= boost_thread
  LDIR+= ${OPENBUS_HOME}/lib/boost
else
  DEFINES+=LOGGER_DISABLE_THREADS
endif
LIBS+= logger


OBJROOT=obj
TARGETROOT=lib

INCLUDES=. \
         legacy/stubs \
         stubs \
         include \
         ${MICO_INC} \
         ${OPENBUS_INC} \
         ${OPENBUS_INC}/openssl-0.9.9 \
         ${OPENBUS_INC}/boost
 
LDIR+=${MICO_LIB} ${OPENBUS_LDIR}

ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
  LIBS+=mico${MICOVERSION}
else
  LIBS+=mico${MICOVERSION} crypto dl
endif

USE_NODEPEND=Yes

SRC=src/openbus.cpp \
    src/manager.cpp \
    src/connection.cpp \
    src/connection_impl.cpp \
    src/interceptors/orbInitializer.cpp \
    src/interceptors/clientInterceptor.cpp \
    src/interceptors/serverInterceptor.cpp \
    src/util/tickets.c \
    stubs/core.cc \
    stubs/scs.cc \
    stubs/credential.cc \
    stubs/access_control.cc \
    stubs/offer_registry.cc \
    legacy/stubs/credential_v1_05.cc 

STUBS=stubs/core.h stubs/core.cc \
      stubs/scs.h stubs/scs.cc \
      stubs/credential.h stubs/credential.cc \
      stubs/access_control.h stubs/access_control.cc \
      stubs/offer_registry.h stubs/offer_registry.cc \
      legacy/stubs/credential_v1_05.h legacy/stubs/credential_v1_05.cc

IDLS=${OPENBUS_HOME}/idl/v2_00/core.idl \
     ${OPENBUS_HOME}/idl/v2_00/scs.idl \
     ${OPENBUS_HOME}/idl/v2_00/credential.idl \
     ${OPENBUS_HOME}/idl/v2_00/access_control.idl \
     ${OPENBUS_HOME}/idl/v2_00/offer_registry.idl \
     legacy/idl/credential_v1_05.idl

$(STUBS): $(IDLS)
	mkdir -p stubs
	mkdir -p legacy/stubs
	cd stubs ; ${MICO_BIN}/idl --no-paths --any --typecode \
	  ${OPENBUS_HOME}/idl/v2_00/access_control.idl
	cd stubs ; ${MICO_BIN}/idl --no-paths --any --typecode \
	  ${OPENBUS_HOME}/idl/v2_00/credential.idl
	cd stubs ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idl/v2_00/offer_registry.idl
	cd stubs ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idl/v2_00/core.idl
	cd stubs ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idl/v2_00/scs.idl
	cd legacy/stubs ; ${MICO_BIN}/idl --no-paths --any --typecode ../idl/credential_v1_05.idl

genstubs: $(STUBS)
