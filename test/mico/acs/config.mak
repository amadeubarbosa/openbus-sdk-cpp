PROJNAME= ACSTester
APPNAME= acs

include ../config

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

INCLUDES= . \
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

SRC= runner.cpp

runner.cpp: ACSTestSuite.h
	cxxtestgen.pl --runner=ErrorPrinter --abort-on-fail -o runner.cpp ACSTestSuite.h

cxxtest: runner.cpp
