#
# Exemplo de arquivo makefile.
#

include config

CPPFLAGS= -Wall -DSCS_ORBIX -DOPENBUS_ORBIX

OPENBUSINC= ${OPENBUS_HOME}/incpath
OPENBUSLIB= ${OPENBUS_HOME}/libpath/${TEC_UNAME}

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

bin/${TEC_UNAME}/server: server.cpp
	mkdir -p bin/${TEC_UNAME}
	g++ ${CPPFLAGS} -o bin/${TEC_UNAME}/server server.cpp stubs/helloC.cxx stubs/helloS.cxx \
		${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

bin/${TEC_UNAME}/client: client.cpp
	mkdir -p bin/${TEC_UNAME}
	g++ ${CPPFLAGS} -o bin/${TEC_UNAME}/client client.cpp stubs/helloC.cxx \
	 	${INCLUDES} ${LDIR} ${SLIB} ${LIBS}

server: bin/${TEC_UNAME}/server
	
client: bin/${TEC_UNAME}/client
	
IDLS= ../idl/hello.idl

STUBS= stubs/helloC.cxx stubs/helloS.cxx stubs/hello.hh

${STUBS}: ${IDLS}
	mkdir -p stubs
	cd stubs ; ${ORBIXBIN}/idl -base -poa ../../idl/hello.idl

genstubs: ${STUBS}

clean:
	rm -rf bin/${TEC_UNAME}
	rm -rf stubs
