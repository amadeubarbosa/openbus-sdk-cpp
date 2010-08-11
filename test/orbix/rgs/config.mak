PROJNAME= RGSTester
APPNAME= rgs

DEFINES=SCS_ORBIX
DEFINES+=OPENBUS_ORBIX

# Descomente a linha abaixo para ativar o modo debug.
#DBG=YES

# Descomente a linha abaixo para o uso em Valgrind.
# p.s.: O modo debug(DBG) deve estar ativado.
#CPPFLAGS= -fno-inline

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

include ../config

INCLUDES= . \
  ${ORBIXINC} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}/cxxtest \
  ${OPENBUSINC}/openssl-0.9.9 \
  ${OPENBUSINC}/logger

LDIR= ${ORBIXLDIR} \
  ${OPENBUSLIB}

LIBS+= crypto it_poa it_art it_ifc it_portable_interceptor logger

SLIB= ${OPENBUSLIB}/libopenbusorbix.a \
  ${OPENBUSLIB}/libscsorbix.a \
  ${OPENBUSLIB}/liblogger.a

USE_LUA51= YES

SRC= runner.cpp \
     stubs/RGSTestC.cxx \
     stubs/RGSTestS.cxx \
     RGSTestSuite.cpp

STUBS= stubs/RGSTest.h stubs/RGSTest.cc

IDLS: ../../idl/RGSTest.idl

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${ORBIXBIN}/idl -base -poa ../../../idl/RGSTest.idl

genstubs: $(STUBS)

runner.cpp: RGSTestSuite.cpp
	cxxtestgen.pl --runner=ErrorPrinter --abort-on-fail -o runner.cpp RGSTestSuite.cpp

cxxtest: runner.cpp
