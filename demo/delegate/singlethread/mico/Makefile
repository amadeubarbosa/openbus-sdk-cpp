#
# Exemplo de arquivo makefile.
#

# Descomente a linha abaixo para compilar em 64 Bits.
MICO_64= -DMICO_64

CPPFLAGS= -Wall ${MICO_64}

OPENBUSINC= ${OPENBUS_HOME}/incpath
OPENBUSLIB= ${OPENBUS_HOME}/libpath/${TEC_UNAME}

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

bin/${TEC_UNAME}/server: server.cpp
	mkdir -p bin/${TEC_UNAME}
	g++ ${CPPFLAGS} -o bin/${TEC_UNAME}/server server.cpp stubs/delegate.cc ${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

bin/${TEC_UNAME}/client: client.cpp
	mkdir -p bin/${TEC_UNAME}
	g++ ${CPPFLAGS} -o bin/${TEC_UNAME}/client client.cpp stubs/delegate.cc ${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

server: bin/${TEC_UNAME}/server
	
client: bin/${TEC_UNAME}/client
	
IDLS= ../../idl/delegate.idl

STUBS= stubs/delegate.cc stubs/delegate.h

${STUBS}: ${IDLS}
	mkdir -p stubs
	cd stubs ; ${OPENBUS_HOME}/bin/${TEC_UNAME}/idl --poa ../../../idl/delegate.idl

genstubs: ${STUBS}

clean:
	rm -rf bin/${TEC_UNAME}
	rm -rf stubs
