ifeq "$(ASSISTANT_SDK_MULTITHREAD)" "Yes"
  ifeq "$(DBG)" "Yes"
    PROJNAME=assistant-micomultithread-debug
  else
    PROJNAME=assistant-micomultithread
  endif
else
  ifeq "$(DBG)" "Yes"
    PROJNAME=assistant-micosinglethread-debug
  else
    PROJNAME=assistant-micosinglethread
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

  ifeq "$(ASSISTANT_SDK_MULTITHREAD)" "Yes"
    CPPFLAGS+=-mt
    LFLAGS+=-mt
    STDLFLAGS=-mt -m64 -xar -o
  endif
endif

ifeq "$(DBG)" "Yes"
  MICO_DEBUG=-debug
endif

ifeq "$(ASSISTANT_SDK_MULTITHREAD)" "Yes"
  MICO_BIN=${MICODIR}/bin/mico-${MICOVERSION}-multithread${MICO_DEBUG}
  MICO_INC=${OPENBUS_HOME}/include/mico-${MICOVERSION}-multithread${MICO_DEBUG}
  MICO_LIB=${OPENBUS_HOME}/lib/mico-${MICOVERSION}-multithread${MICO_DEBUG}
  DEFINES+=OPENBUS_SDK_MULTITHREAD
  DEFINES+=ASSISTANT_SDK_MULTITHREAD
  DEFINES+=SCS_THREADING_ENABLED
else
  MICO_BIN=${MICODIR}/bin/mico-${MICOVERSION}-singlethread${MICO_DEBUG}
  MICO_INC=${OPENBUS_HOME}/include/mico-${MICOVERSION}-singlethread${MICO_DEBUG}
  MICO_LIB=${OPENBUS_HOME}/lib/mico-${MICOVERSION}-singlethread${MICO_DEBUG}
endif

OPENBUS_INC=${OPENBUS_HOME}/include
OPENBUS_LDIR=${OPENBUS_HOME}/lib
ifeq "$(ASSISTANT_SDK_MULTITHREAD)" "Yes"
  LIBS+= boost_thread
  LIBS+= logger-multithread${MICO_DEBUG}
  LIBS+= openbus-micomultithread${MICO_DEBUG}
  LDIR+= ${OPENBUS_HOME}/lib/boost
else
  DEFINES+=LOGGER_DISABLE_THREADS
  LIBS+= logger-singlethread${MICO_DEBUG}
  LIBS+= openbus-micosinglethread${MICO_DEBUG}
endif

OBJROOT=obj
TARGETROOT=lib

INCLUDES=../legacy/stubs \
         ../stubs \
         ../include \
         include \
         ${MICO_INC} \
         ${OPENBUS_INC} \
         ${OPENBUS_INC}/openssl-1.0.0 \
         ${OPENBUS_INC}/boost \
         ${OPENBUS_INC}/openbus/cpp \
         ${OPENBUS_INC}/openbus/cpp/stubs \
         ${OPENBUS_INC}/scs \
         ${OPENBUS_INC}/scs/stubs/mico

LDIR+=${MICO_LIB} ${OPENBUS_LDIR}

ifeq "$(ASSISTANT_SDK_MULTITHREAD)" "Yes"
  LIBS+=mico${MICOVERSION}
else
  LIBS+=mico${MICOVERSION} crypto dl
endif

USE_NODEPEND=Yes

SRC=src/assistant_common.cpp

ifeq "$(ASSISTANT_SDK_MULTITHREAD)" "Yes"
  SRC+= src/assistant_mt.cpp
else
  SRC+= src/assistant_st.cpp
endif