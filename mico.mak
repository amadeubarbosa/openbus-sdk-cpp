PROJNAME= openbusmico
LIBNAME= ${PROJNAME}

#Descomente a linha abaixo para utilizar a versão multithread.
# A versão multithread do Openbus deve ser utilizada com Mico
# compilado com suporte a multithread.
#DEFINES+=MULTITHREAD

#Descomente as duas linhas abaixo para o uso em Valgrind.
#DBG=YES
#CPPFLAGS= -fno-inline

ifeq "$(TEC_UNAME)" "SunOS510_64"
  CPPFLAGS= -m64
  LFLAGS= -m64
  STDLFLAGS= -m64
endif

ifeq "$(TEC_SYSNAME)" "SunOS"
  USE_CC=Yes
  AR= CC

  # Multithread
  CPPFLAGS+= -mt
  STDLFLAgS+= -mt	
  LFLAGS+= -mt
  
  CPPFLAGS+= -KPIC -library=stlport4
  STDLFLAGS+= -KPIC -xar -o	
  LFLAGS+= -library=stlport4 
endif

ifeq ($(TEC_WORDSIZE), TEC_64)
  CPPFLAGS+= -m64
endif

MICO_BIN= ${MICODIR}/bin/${TEC_UNAME}
MICO_INC= ${MICODIR}/include
MICO_LIB=${MICODIR}/lib/${TEC_UNAME}

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

OBJROOT= obj
TARGETROOT= lib

INCLUDES= . ./stubs/mico ${MICO_INC} ${OPENBUSINC}/scs ${OPENBUSINC}/openssl-0.9.9 ${OPENBUSINC}/logger
LDIR= ${MICO_LIB} ${OPENBUSLIB} ${MICO_LIB}

LIBS= mico${MICOVERSION} scsmico crypto dl logger

USE_LUA51= YES
USE_NODEPEND= Yes

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

STUBS= stubs/mico/core.h stubs/mico/core.cc \
stubs/mico/scs.h stubs/mico/scs.cc \
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

genstubs: $(STUBS)
	
sunosShared: $(OBJS)
	rm -f lib/$(TEC_UNAME)/libopenbusmico.so
	CC $(LFLAGS) -G -instances=extern -o lib/$(TEC_UNAME)/libopenbusmico.so $(OBJS)

