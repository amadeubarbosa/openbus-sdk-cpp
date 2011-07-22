ifeq "$(MULTITHREAD)" "Yes"
  PROJNAME=openbusmicoMT
else
  PROJNAME=openbusmicoST
endif
LIBNAME= ${PROJNAME}

# Descomente a linha abaixo para ativar o modo debug.
#DBG=YES

# Descomente a linha abaixo para o uso em Valgrind.
# p.s.: O modo debug(DBG) deve estar ativado.
#CPPFLAGS= -fno-inline

ifeq "$(TEC_UNAME)" "SunOS510_64"
  USE_CC=Yes
  AR= CC
  
  ifeq "$(DBG)" "YES"
    CPPFLAGS= -g
    STDLFLAGS= -g
    LFLAGS= -g
  else
    FLAGS=
    CPPFLAGS=
    STDLFLAGS=
    LFLAGS=
  endif

  CPPFLAGS= -m64 -KPIC -library=stlport4
  STDLFLAGS= -m64 -xar -o  
  LFLAGS= -m64 -instances=extern -library=stlport4 
  LIBS= nsl socket
  NO_LOCAL_LD=Yes

  ifeq "$(MULTITHREAD)" "Yes"
    CPPFLAGS+= -mt
    LFLAGS+= -mt
    STDLFLAGS= -mt -m64 -xar -o
  endif
endif

ifeq "$(MULTITHREAD)" "Yes"
  MICO_BIN= ${MICODIR}/bin/${TEC_UNAME}/mico-${MICOVERSION}-multithread
  MICO_INC= ${OPENBUS_HOME}/incpath/mico-${MICOVERSION}-multithread/${TEC_UNAME}
  MICO_LIB= ${OPENBUS_HOME}/libpath/mico-${MICOVERSION}-multithread/${TEC_UNAME}
  DEFINES+=MULTITHREAD
  DEFINES+=SCS_THREADING_ENABLED
else
  MICO_BIN= ${MICODIR}/bin/${TEC_UNAME}/mico-${MICOVERSION}-singlethread
  MICO_INC= ${OPENBUS_HOME}/incpath/mico-${MICOVERSION}-singlethread/${TEC_UNAME}
  MICO_LIB= ${OPENBUS_HOME}/libpath/mico-${MICOVERSION}-singlethread/${TEC_UNAME}
endif

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

OBJROOT= obj/mico
TARGETROOT= lib

INCLUDES= . ./stubs/mico ${MICO_INC} ${OPENBUSINC}/scs ${OPENBUSINC}/openssl-0.9.9 ${OPENBUSINC}/logger
LDIR= ${MICO_LIB} ${OPENBUSLIB}

ifeq "$(MULTITHREAD)" "Yes"
  LIBS= mico${MICOVERSION} scsmicoMT crypto dl logger
else
  LIBS= mico${MICOVERSION} scsmicoST crypto dl logger
endif

USE_LUA51= YES
USE_NODEPEND= YES

SRC= openbus/interceptors/ClientInterceptor.cpp \
     openbus/interceptors/ServerInterceptor.cpp \
     openbus/interceptors/ORBInitializerImpl.cpp \
     stubs/mico/access_control_service.cc \
     stubs/mico/registry_service.cc \
     stubs/mico/session_service.cc \
     stubs/mico/fault_tolerance.cc \
     stubs/mico/core.cc \
     openbus.cpp \
     openbus/util/Helper.cpp \
     FaultToleranceManager.cpp

STUBS= stubs/mico/core.h stubs/mico/core.cc \
stubs/mico/scs.h \
stubs/mico/access_control_service.h stubs/mico/access_control_service.cc \
stubs/mico/registry_service.h stubs/mico/registry_service.cc \
stubs/mico/fault_tolerance.h stubs/mico/fault_tolerance.cc \
stubs/mico/session_service.h stubs/mico/session_service.cc

IDLS= ${OPENBUS_HOME}/idlpath/v1_05/core.idl \
${OPENBUS_HOME}/idlpath/v1_05/scs.idl \
${OPENBUS_HOME}/idlpath/v1_05/access_control_service.idl \
${OPENBUS_HOME}/idlpath/v1_05/registry_service.idl \
${OPENBUS_HOME}/idlpath/v1_05/fault_tolerance.idl \
${OPENBUS_HOME}/idlpath/v1_05/session_service.idl 

$(STUBS): $(IDLS)
	mkdir -p stubs/mico
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths --any --typecode \
	  ${OPENBUS_HOME}/idlpath/v1_05/access_control_service.idl
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idlpath/v1_05/fault_tolerance.idl
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idlpath/v1_05/registry_service.idl
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idlpath/v1_05/session_service.idl
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idlpath/v1_05/core.idl
	cd stubs/mico ; ${MICO_BIN}/idl --no-paths \
	  ${OPENBUS_HOME}/idlpath/v1_05/scs.idl

genstubs: $(STUBS)
	
