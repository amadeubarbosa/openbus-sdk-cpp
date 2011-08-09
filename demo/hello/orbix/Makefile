#
# Exemplo de arquivo makefile.
#

include config

CPPFLAGS= -Wall -DSCS_ORBIX -DOPENBUS_ORBIX

OPENBUSINC= ${OPENBUS_HOME}/include
OPENBUSLIB= ${OPENBUS_HOME}/lib

SLIB= ${OPENBUSLIB}/libopenbusorbix.a \
      ${OPENBUSLIB}/libscsorbix.a \
      ${OPENBUSLIB}/liblogger.a

INCLUDES= -I. \
				  -Istubs \
				  -I${ORBIXINC} \
				  -I${OPENBUSINC}/openbus/cpp \
				  -I${OPENBUSINC}/openbus/cpp/stubs/mico \
				  -I${OPENBUSINC} \
				  -I${OPENBUSINC}/logger \
          -I${OPENBUSINC}/lua5.1.3

LDIR= -L${ORBIXLDIR} \
      -L${OPENBUSLIB}

LIBS= -lit_poa -lit_art -lit_ifc -lit_portable_interceptor -ldl -lcrypto -lssl -llua5.1

all: genstubs server client

bin/server: server.cpp
	mkdir -p bin
	g++ ${CPPFLAGS} -o bin/server server.cpp stubs/helloC.cxx stubs/helloS.cxx \
		${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

bin/client: client.cpp
	mkdir -p bin
	g++ ${CPPFLAGS} -o bin/client client.cpp stubs/helloC.cxx \
	 	${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

server: bin/server
	
client: bin/client
	
IDLS= ../idl/hello.idl

STUBS= stubs/helloC.cxx stubs/helloS.cxx stubs/hello.hh

${STUBS}: ${IDLS}
	mkdir -p stubs
	cd stubs ; ${OPENBUS_HOME}/specs/shell/orbix/idl -base -poa ../../idl/hello.idl

genstubs: ${STUBS}

clean:
	rm -rf bin
	rm -rf stubs
