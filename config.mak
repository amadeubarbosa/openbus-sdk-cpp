ifeq "$(MULTITHREAD)" "Yes"
  PROJNAME=openbus_mt
else
  PROJNAME=openbus_st
endif
LIBNAME=${PROJNAME}

# Descomente a primeira linha abaixo e comente a segunda para ativar o modo debug.
DBG=Yes
#OPT=Yes

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

  ifeq "$(MULTITHREAD)" "Yes"
    CPPFLAGS+=-mt
    LFLAGS+=-mt
    STDLFLAGS=-mt -m64 -xar -o
  endif
endif

ifeq "$(DBG)" "Yes"
  MICO_DEBUG=-debug
endif

ifeq "$(MULTITHREAD)" "Yes"
  MICO_BIN=${MICODIR}/bin/mico-${MICOVERSION}-multithread${MICO_DEBUG}
  MICO_INC=${OPENBUS_HOME}/include/mico-${MICOVERSION}-multithread${MICO_DEBUG}
  MICO_LIB=${OPENBUS_HOME}/lib/mico-${MICOVERSION}-multithread${MICO_DEBUG}
  DEFINES+=MULTITHREAD
  DEFINES+=SCS_THREADING_ENABLED
else
  MICO_BIN=${MICODIR}/bin/mico-${MICOVERSION}-singlethread${MICO_DEBUG}
  MICO_INC=${OPENBUS_HOME}/include/mico-${MICOVERSION}-singlethread${MICO_DEBUG}
  MICO_LIB=${OPENBUS_HOME}/lib/mico-${MICOVERSION}-singlethread${MICO_DEBUG}
endif

OPENBUSINC=${OPENBUS_HOME}/include
OPENBUSLIB=${OPENBUS_HOME}/lib

OBJROOT=obj
TARGETROOT=lib

INCLUDES=. \
          stubs \
          include \
          ${MICO_INC} \
          ${OPENBUSINC}/openssl-0.9.9

LDIR=${MICO_LIB} ${OPENBUSLIB}

ifeq "$(MULTITHREAD)" "Yes"
  LIBS=mico${MICOVERSION} 
  #LIBS=mico${MICOVERSION} scsmicoMT crypto dl logger
else
  LIBS=mico${MICOVERSION} crypto dl
  #LIBS=mico${MICOVERSION} scsmicoST crypto dl logger
endif

USE_NODEPEND=Yes

SRC=stc/openbus.cpp \
    src/connection.cpp \
    src/interceptors/orbInitializer.cpp \
    src/interceptors/clientInterceptor.cpp \
    src/interceptors/serverInterceptor.cpp \
    src/util/tickets.c \
    stubs/core.cc \
    stubs/scs.cc \
    stubs/credential.cc \
    stubs/access_control.cc \
    stubs/offer_registry.cc

STUBS=stubs/core.h stubs/core.cc \
       stubs/scs.h stubs/scs.cc \
       stubs/credential.h stubs/credential.cc \
       stubs/access_control.h stubs/access_control.cc \
       stubs/offer_registry.h stubs/offer_registry.cc

IDLS=${OPENBUS_HOME}/idl/v2_00/core.idl \
${OPENBUS_HOME}/idl/v2_00/scs.idl \
${OPENBUS_HOME}/idl/v2_00/credential.idl \
${OPENBUS_HOME}/idl/v2_00/access_control.idl \
${OPENBUS_HOME}/idl/v2_00/offer_registry.idl


$(STUBS): $(IDLS)
	mkdir -p stubs
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

genstubs: $(STUBS)
