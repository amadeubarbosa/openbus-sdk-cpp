PROJNAME=server
APPNAME=${PROJNAME}

include config

OPENBUSINC = ${OPENBUS_HOME}/incpath
OPENBUSLIB = ${OPENBUS_HOME}/libpath/${TEC_UNAME}

TARGETROOT=bin
OBJROOT=obj

INCLUDES= . \
  stubs \
  ${MICO_INC} \
  ${OPENBUSINC}/openbus/cpp \
  ${OPENBUSINC}/openbus/cpp/stubs/mico \
  ${OPENBUSINC} \
  ${OPENBUSINC}/logger

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

SRC= server.cpp \
     stubs/hello.cc 

IDLS= ../idl/hello.idl

STUBS= stubs/hello.cc stubs/hello.h

$(STUBS): $(IDLS)
	mkdir -p stubs
	cd stubs ; ${MICO_BIN}/idl --poa ../../idl/hello.idl

genstubs: $(STUBS)
