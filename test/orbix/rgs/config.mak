PROJNAME= RGSTester
APPNAME= rgs

#Descomente as duas linhas abaixo para o uso em Valgrind.
#DBG=YES
#CPPFLAGS= -fno-inline

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

include ../config

INCLUDES= . \
  ${ORBIXINC} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/scs \
  ${OPENBUSINC}/cxxtest \
  ${OPENBUSINC}/openssl-0.9.9

LDIR= ${ORBIXLDIR} \
  ${OPENBUSLIB}

LIBS+= crypto it_poa it_art it_ifc it_portable_interceptor

SLIB= ${OPENBUSLIB}/libopenbusorbix.a \
  ${OPENBUSLIB}/libscsorbix.a

USE_LUA51= YES

SRC= runner.cpp \
     RGSTestSuite.cpp

cxxtest:
	cxxtestgen.pl --runner=StdioPrinter -o runner.cpp RGSTestSuite.cpp
