#
# Exemplo de arquivo makefile.
#

# Descomente a linha abaixo para compilar em 64 Bits.
MICO_64= -DMICO_64

# Descomente as duas linhas abaixo para utilizar a versão multithread.
# A versão multithread do Openbus deve ser utilizada com Mico
# compilado com suporte a multithread.
MULTITHREAD= -DMULTITHREAD
PTHREAD= -lpthread

CPPFLAGS= -Wall ${MICO_64} ${MULTITHREAD}

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

LDIR= -L${OPENBUSLIB}

LIBS= -lmico${MICOVERSION} -ldl -lcrypto -lssl -llua5.1 ${PTHREAD}

all: genstubs server client

bin/server: server.cpp
	mkdir -p bin
	g++ ${CPPFLAGS} -o bin/server server.cpp stubs/hello.cc ${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

bin/client: client.cpp
	mkdir -p bin
	g++ ${CPPFLAGS} -o bin/client client.cpp stubs/hello.cc ${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

server: bin/server
	
client: bin/client
	
IDLS= ../idl/hello.idl

STUBS= stubs/hello.cc stubs/hello.h

${STUBS}: ${IDLS}
	mkdir -p stubs
	cd stubs ; ${OPENBUS_HOME}/bin/idl --poa ../../idl/hello.idl

genstubs: ${STUBS}

clean:
	rm -rf bin
	rm -rf stubs
