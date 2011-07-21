PROJNAME= RGSTester
APPNAME= rgs

include ../config

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

INCLUDES= . \
  stubs \
  ${MICO_INC} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/openbus/cpp/stubs/mico \
  ${OPENBUSINC} \
  ${OPENBUSINC}/logger \
  ${OPENBUSINC}/cxxtest

LDIR= ${OPENBUSLIB} ${MICO_LIB}

LIBS+= mico${MICOVERSION} dl crypto ssl

ifeq "$(MULTITHREAD)" "Yes"
  SLIB= ${OPENBUSLIB}/libopenbusmicoMT.a \
        ${OPENBUSLIB}/libscsmicoMT.a \
        ${OPENBUSLIB}/liblogger.a
else
  SLIB= ${OPENBUSLIB}/libopenbusmicoST.a \
        ${OPENBUSLIB}/libscsmicoST.a \
        ${OPENBUSLIB}/liblogger.a
endif

USE_LUA51= YES
USE_NODEPEND= Yes

SRC= runner.cpp stubs/RGSTest.cc

STUBS= stubs/RGSTest.h stubs/RGSTest.cc

IDLS: ../../idl/RGSTest.idl

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../../../idl/RGSTest.idl

genstubs: $(STUBS)

runner.cpp: RGSTestSuite.h
	cxxtestgen.pl --runner=ErrorPrinter --abort-on-fail -o runner.cpp RGSTestSuite.h

cxxtest: runner.cpp


