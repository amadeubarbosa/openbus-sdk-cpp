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
				  -I${OPENBUSINC}/scs \
				  -I${OPENBUSINC}/logger \
          -I${OPENBUSINC}/lua5.1.3

LDIR= -L${ORBIXLDIR} \
		  -L${OPENBUSLIB}

LIBS= -lit_poa -lit_art -lit_ifc -lit_portable_interceptor -ldl -lcrypto -lssl -llua5.1

all: genstubs server client

bin/server: server.cpp
	mkdir -p bin
	g++ ${CPPFLAGS} -o bin/server server.cpp stubs/delegateS.cxx stubs/delegateC.cxx \
		${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

bin/client: client.cpp
	mkdir -p bin
	g++ ${CPPFLAGS} -o bin/client client.cpp stubs/delegateC.cxx \
	 	${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

server: bin/server
	
client: bin/client
	
IDLS= ../../idl/delegate.idl

STUBS= stubs/delegateS.cxx stubs/delegateC.cxx

${STUBS}: ${IDLS}
	mkdir -p stubs
	cd stubs ; ${ORBIXBIN}/idl -poa -base ../../../idl/delegate.idl

genstubs: ${STUBS}

clean:
	rm -rf bin
	rm -rf stubs
