#
# Exemplo de arquivo makefile.
#

# Descomente a linha abaixo para compilar em 64 Bits.
MICO_64= -DMICO_64

CPPFLAGS= -Wall ${MICO_64}

OPENBUSINC= ${OPENBUS_HOME}/include
OPENBUSLIB= ${OPENBUS_HOME}/lib

SLIB= ${OPENBUSLIB}/libopenbusmico.a \
      ${OPENBUSLIB}/libscsmico.a \
      ${OPENBUSLIB}/liblogger.a

INCLUDES= -I. \
				  -Istubs \
				  -I${OPENBUSINC}/mico-${MICOVERSION} \
				  -I${OPENBUSINC}/openbus/cpp \
				  -I${OPENBUSINC}/openbus/cpp/stubs/mico \
				  -I${OPENBUSINC}/scs \
				  -I${OPENBUSINC}/logger \
          -I${OPENBUSINC}/lua5.1.3

LDIR= -L${MICO_LIB} \
		  -L${OPENBUSLIB}

LIBS= -lmico${MICOVERSION} -ldl -lcrypto -lssl -llua5.1

all: genstubs server client

bin/server: server.cpp
	mkdir -p bin
	g++ ${CPPFLAGS} -o bin/server server.cpp stubs/delegate.cc ${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

bin/client: client.cpp
	mkdir -p bin
	g++ ${CPPFLAGS} -o bin/client client.cpp stubs/delegate.cc ${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

server: bin/server
	
client: bin/client
	
IDLS= ../../idl/delegate.idl

STUBS= stubs/delegate.cc stubs/delegate.h

${STUBS}: ${IDLS}
	mkdir -p stubs
	cd stubs ; ${OPENBUS_HOME}/bin/idl --poa ../../../idl/delegate.idl

genstubs: ${STUBS}

clean:
	rm -rf bin
	rm -rf stubs
