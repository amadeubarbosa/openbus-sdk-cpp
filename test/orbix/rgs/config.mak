PROJNAME= RGSTester
APPNAME= rgs

DEFINES=SCS_ORBIX
DEFINES+=OPENBUS_ORBIX

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
USE_NODEPEND= YES

SRC= runner.cpp \
     stubs/RGSTestC.cxx \
     stubs/RGSTestS.cxx \
     RGSTestSuite.h

STUBS= stubs/RGSTest.h stubs/RGSTest.cc

IDLS: ../../idl/RGSTest.idl

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ../../../../shell/orbix/idl -base -poa ../../../idl/RGSTest.idl

genstubs: $(STUBS)

runner.cpp: RGSTestSuite.h
	cxxtestgen.pl --runner=ErrorPrinter --abort-on-fail -o runner.cpp RGSTestSuite.h

cxxtest: runner.cpp
