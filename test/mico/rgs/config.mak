PROJNAME= RGSTester
APPNAME= rgs

DEFINES=SCS_MICO
DEFINES+=OPENBUS_MICO

#Descomente a linha abaixo para utilizar o Openbus Multithread.
#LIBS=pthread

#Descomente as duas linhas abaixo para o uso em Valgrind.
#DBG=YES
#CPPFLAGS= -fno-inline

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

EXTRA_CONFIG=../config

ifeq "$(TEC_UNAME)" "SunOS58"
  USE_CC=Yes
  CPPFLAGS= -g +p -KPIC -xarch=v8  -mt -D_REENTRANT
endif

INCLUDES= . ./stubs ../../../stubs/mico\
  ${MICO_INC} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}/cxxtest \
  ${OPENBUSINC}/openssl-0.9.9 \
  ${OPENBUSINC}/logger

LDIR= ${MICO_LIB} \
  ${OPENBUSLIB}

LIBS= crypto mico${MICOVERSION} dl ssl

SLIB= ${OPENBUSLIB}/libopenbusmico.a \
  ${OPENBUSLIB}/libscsmico.a \
  ${OPENBUSLIB}/liblogger.a

USE_LUA51= YES

SRC= runner.cpp \
     stubs/RGSTest.cc \
     RGSTestSuite.cpp

STUBS= stubs/RGSTest.h stubs/RGSTest.cc

IDLS: ../../idl/RGSTest.idl

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../../../idl/RGSTest.idl

genstubs: $(STUBS)

runner.cpp: RGSTestSuite.cpp
	cxxtestgen.pl --runner=ErrorPrinter --abort-on-fail -o runner.cpp RGSTestSuite.cpp

cxxtest: runner.cpp


