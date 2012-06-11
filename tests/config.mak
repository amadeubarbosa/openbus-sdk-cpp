
PROJNAME= SDKTests

ifeq "$(TEC_UNAME)" "SunOS510_64"
  USE_CC=Yes
  AR= CC
  
  ifeq "$(DBG)" "YES"
    CPPFLAGS= -g
    STDLFLAGS= -g
    LFLAGS= -g
  endif

  CPPFLAGS+= -erroff=badargtype2w -m64 -library=stlport4
  STDLFLAGS+= -m64 -xar -o  
  LFLAGS+= -m64 -library=stlport4 
  LIBS= nsl socket
  NO_LOCAL_LD=Yes

  ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
    CPPFLAGS+= -mt
    LFLAGS+= -mt
    STDLFLAGS= -mt -m64 -xar -o
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

ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
LIBS+= boost_thread
LDIR+= ${OPENBUSLIB}/boost
else
DEFINES+=LOGGER_DISABLE_THREADS
endif

OPENBUSINC= ${OPENBUS_HOME}/include
OPENBUSLIB= ${OPENBUS_HOME}/lib

LDIR+= ${OPENBUSLIB} ${MICO_LIB}

LIBS+= mico${MICOVERSION} dl crypto ssl

ifeq "$(OPENBUS_SDK_MULTITHREAD)" "Yes"
  SLIB= ${OPENBUSLIB}/libopenbus-micomultithread${MICO_DEBUG}.a \
        ${OPENBUSLIB}/libscs-micomultithread${MICO_DEBUG}.a \
        ${OPENBUSLIB}/liblogger-multithread${MICO_DEBUG}.a
else
  SLIB= ${OPENBUSLIB}/libopenbus-micosinglethread${MICO_DEBUG}.a \
        ${OPENBUSLIB}/libscs-micosinglethread${MICO_DEBUG}.a \
        ${OPENBUSLIB}/liblogger$-singlethread{MICO_DEBUG}.a
endif

#USE_LUA51= YES

OBJROOT= obj
TARGETROOT= bin

STUBS=stubs/hello.h stubs/hello.cc
IDLS=idl/hello.idl

INCLUDES=. .. \
  ../legacy/stubs \
	stubs \
  ../stubs \
  ../include \
	${MICO_INC} \
	${OPENBUSINC}/openssl-0.9.9 \
	${OPENBUSINC} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/openbus/cpp/stubs \
  ${OPENBUSINC}/scs ${OPENBUSINC} ${OPENBUSINC}/boost

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; $(MICO_BIN)/idl --no-paths --any --typecode ../idl/hello.idl

